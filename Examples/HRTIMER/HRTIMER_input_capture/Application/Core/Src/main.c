/*!
    \file    main.c
    \brief   HRTIMER0 input capture

    \version 2026-02-04, V1.5.0, firmware for GD32G5x3
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32g5x3.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32g553q_eval.h"


hrtimer_capture_value_struct hrtimer_capture_value;
extern uint32_t percap0;

void gpio_config(void);
void hrtimer_config(void);

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);

    /* HRTIMER_EXEV5(PB5) */
    gpio_af_set(GPIOB,GPIO_AF_13,GPIO_PIN_5);
    gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_5);
    gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_100_220MHZ,GPIO_PIN_5);
}

/*!
    \brief      configure the HRTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void hrtimer_config(void)
{
/* -----------------------------------------------------------------------
    fHRTIMER_CK: 216MHz, from CK_SYS
    fHRTIMER_PSCCK: 216MHz
    external signal cycle range: (1/216us)~(65535/216us)
  ----------------------------------------------------------------------- */
    hrtimer_baseinit_parameter_struct baseinit_para;
    hrtimer_exeventcfg_parameter_struct exevcfg_para;
    hrtimer_timercfg_parameter_struct timercfg_para;

    /* clock config */
    rcu_periph_clock_enable(RCU_HRTIMER);
    rcu_hrtimer_clock_config(RCU_HRTIMERSRC_CKSYS);

    /* periodic DLL calibration */
    hrtimer_dll_calibration_start(HRTIMER0, HRTIMER_CALIBRATION_ONCE);
    while(RESET == hrtimer_common_flag_get(HRTIMER0, HRTIMER_FLAG_DLLCAL));

    /* Slave_TIMER0 time base clock config  */
    hrtimer_baseinit_struct_para_init(&baseinit_para);
    baseinit_para.period = 0xFFFE;
    baseinit_para.prescaler = HRTIMER_PRESCALER_DIV1;
    baseinit_para.repetitioncounter = 0;
    baseinit_para.counter_mode = HRTIMER_COUNTER_MODE_CONTINOUS;
    hrtimer_timers_base_init(HRTIMER0, HRTIMER_SLAVE_TIMER0, &baseinit_para);

    /* configure the general behavior of a Slave_TIMER0 which work in waveform mode */
    hrtimer_timercfg_struct_para_init(&timercfg_para);
    timercfg_para.balanced_mode = HRTIMER_STXBALANCEDMODE_DISABLED;
    timercfg_para.cnt_reset = HRTIMER_STXCNT_RESET_EEV_5;
    timercfg_para.deadtime_enable = HRTIMER_STXDEADTIME_DISABLED;
    timercfg_para.delayed_idle = HRTIMER_STXDELAYED_IDLE_DISABLED;
    timercfg_para.fault_enable = HRTIMER_STXFAULTENABLE_NONE;
    timercfg_para.fault_protect = HRTIMER_STXFAULT_PROTECT_READWRITE;
    timercfg_para.reset_update = HRTIMER_STXUPDATEONRESET_DISABLED;
    timercfg_para.update_source = HRTIMER_STXUPDATETRIGGER_NONE;
    hrtimer_slavetimer_waveform_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, &timercfg_para);

    /* configures the an external event */
    hrtimer_exeventcfg_struct_para_init(&exevcfg_para);
    exevcfg_para.digital_filter = 0x1;
    exevcfg_para.edge = HRTIMER_EXEV_EDGE_FALLING;
    exevcfg_para.polarity = HRTIMER_EXEV_EDGE_LEVEL;
    exevcfg_para.source = HRTIMER_EXEV_SRC0;
    hrtimer_exevent_config(HRTIMER0, HRTIMER_EXEVENT_5, &exevcfg_para);

    /* configure the capture source in Slave_TIMER */
    hrtimer_slavetimer_capture_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_CAPTURE_0, HRTIMER_CAPTURETRIGGER_EXEV_5);
    /* clear the Master_TIMER and Slave_TIMER interrupt flag */
    hrtimer_timers_interrupt_flag_clear(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_ST_INT_FLAG_CAP0);
    /* enable the Master_TIMER and Slave_TIMER interrupt */
    hrtimer_timers_interrupt_enable(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_ST_INT_CAP0);

    /* enable NVIC request */
    nvic_irq_enable(HRTIMER_IRQ1_IRQn, 0U, 0U);
    /* enable a counter */
    hrtimer_timers_counter_enable(HRTIMER0, HRTIMER_ST0_COUNTER);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_com_init(EVAL_COM);
    systick_config();
    gpio_config();
    hrtimer_config();

    while(1){
        delay_1ms(10);
        printf("the period of external signal is %d ns \n", hrtimer_capture_value.value);
    }
}
