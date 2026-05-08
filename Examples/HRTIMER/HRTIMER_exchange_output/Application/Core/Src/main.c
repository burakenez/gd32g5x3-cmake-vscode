/*!
    \file    main.c
    \brief   HRTIMER0 regular output

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
#include <stdio.h>
#include "main.h"
#include "gd32g553q_eval.h"

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
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure HRTIMER_ST0CH0(PA8), HRTIMER_ST0CH1(PA9) */
    gpio_af_set(GPIOA,GPIO_AF_13,GPIO_PIN_8);
    gpio_af_set(GPIOA,GPIO_AF_13,GPIO_PIN_9);
    gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_8);
    gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_9);
    gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_100_220MHZ,GPIO_PIN_8);
    gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_100_220MHZ,GPIO_PIN_9);
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
    fHRTIMER_CK:216MHz, from CK_SYS
    waveform period: 6912/(216M*32)=1us
    ST0_CH0_O: set request at Slave_TIMER0 compare 0 event, reset request at Slave_TIMER0 period event
    ST0_CH1_O: set request at Slave_TIMER0 compare 1 event, reset request at Slave_TIMER0 period event
  ----------------------------------------------------------------------- */
    hrtimer_baseinit_parameter_struct baseinit_para;
    hrtimer_timerinit_parameter_struct timerinit_para;
    hrtimer_timercfg_parameter_struct timercfg_para;
    hrtimer_comparecfg_parameter_struct comparecfg_para;
    hrtimer_channel_outputcfg_parameter_struct outcfg_para;

    /* clock config */
    rcu_periph_clock_enable(RCU_HRTIMER);
    rcu_hrtimer_clock_config(RCU_HRTIMERSRC_CKSYS);

    /* periodic DLL calibration */
    hrtimer_dll_calibration_start(HRTIMER0, HRTIMER_CALIBRATION_ONCE);
    while(RESET == hrtimer_common_flag_get(HRTIMER0, HRTIMER_FLAG_DLLCAL));

    /* Slave_TIMER0 time base clock config  */
    hrtimer_baseinit_struct_para_init(&baseinit_para);
    baseinit_para.period = 6912;
    baseinit_para.prescaler = HRTIMER_PRESCALER_MUL32;
    baseinit_para.repetitioncounter = 0;
    baseinit_para.counter_mode = HRTIMER_COUNTER_MODE_CONTINOUS;
    hrtimer_timers_base_init(HRTIMER0, HRTIMER_SLAVE_TIMER0, &baseinit_para);

    /* initialize Slave_TIMER0 to work in waveform mode */
    hrtimer_timerinit_struct_para_init(&timerinit_para);
    timerinit_para.cnt_bunch = HRTIMER_TIMERBUNCHNMODE_MAINTAINCLOCK;
    timerinit_para.dac_trigger = HRTIMER_DAC_TRIGGER_NONE;
    timerinit_para.half_mode = HRTIMER_HALFMODE_DISABLED;
    timerinit_para.repetition_update = HRTIMER_UPDATEONREPETITION_ENABLED;
    timerinit_para.reset_sync = HRTIMER_SYNCRESET_DISABLED;
    timerinit_para.shadow = HRTIMER_SHADOW_ENABLED;
    timerinit_para.start_sync = HRTIMER_SYNISTART_DISABLED;
    timerinit_para.update_selection = HRTIMER_MT_ST_UPDATE_SELECTION_INDEPENDENT;
    hrtimer_timers_waveform_init(HRTIMER0, HRTIMER_SLAVE_TIMER0, &timerinit_para);

    /* configure the general behavior of a Slave_TIMER0 which work in waveform mode */
    hrtimer_timercfg_struct_para_init(&timercfg_para);
    timercfg_para.balanced_mode = HRTIMER_STXBALANCEDMODE_DISABLED;
    timercfg_para.cnt_reset = HRTIMER_STXCNT_RESET_NONE;
    timercfg_para.deadtime_enable = HRTIMER_STXDEADTIME_DISABLED;
    timercfg_para.delayed_idle = HRTIMER_STXDELAYED_IDLE_DISABLED;
    timercfg_para.fault_enable = HRTIMER_STXFAULTENABLE_NONE;
    timercfg_para.fault_protect = HRTIMER_STXFAULT_PROTECT_READWRITE;
    timercfg_para.reset_update = HRTIMER_STXUPDATEONRESET_DISABLED;
    timercfg_para.update_source = HRTIMER_STXUPDATETRIGGER_NONE;
    hrtimer_slavetimer_waveform_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, &timercfg_para);

    /* configures the compare unit of a Slave_TIMER0 which work in waveform mode */
    hrtimer_comparecfg_struct_para_init(&comparecfg_para);
    comparecfg_para.compare_value = 1000;
    hrtimer_slavetimer_waveform_compare_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_COMPARE0, &comparecfg_para);
    comparecfg_para.compare_value = 6000;
    hrtimer_slavetimer_waveform_compare_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_COMPARE1, &comparecfg_para);

    /* configures the ST0_CH0_O output of a Slave_TIMER0 work in waveform mode */
    hrtimer_channel_outputcfg_struct_para_init(&outcfg_para);
    outcfg_para.carrier_mode = HRTIMER_CHANNEL_CARRIER_DISABLED;
    outcfg_para.deadtime_bunch = HRTIMER_CHANNEL_BUNCH_ENTRY_REGULAR;
    outcfg_para.fault_state = HRTIMER_CHANNEL_FAULTSTATE_NONE;
    outcfg_para.idle_bunch = HRTIMER_CHANNEL_BUNCH_IDLE_DISABLE;
    outcfg_para.idle_state = HRTIMER_CHANNEL_IDLESTATE_INACTIVE;
    outcfg_para.polarity = HRTIMER_CHANNEL_POLARITY_HIGH;
    outcfg_para.reset_request = HRTIMER_CHANNEL_RESET_PER;
    outcfg_para.set_request = HRTIMER_CHANNEL_SET_CMP0;
    hrtimer_slavetimer_waveform_channel_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_ST0_CH0, &outcfg_para);

    /* configures the ST0_CH1_O output of a Slave_TIMER0 work in waveform mode */
    outcfg_para.reset_request = HRTIMER_CHANNEL_RESET_PER;
    outcfg_para.set_request = HRTIMER_CHANNEL_SET_CMP1;
    hrtimer_slavetimer_waveform_channel_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_ST0_CH1, &outcfg_para);

    hrtimer_software_update( HRTIMER0, HRTIMER_UPDATE_SW_ST0);
    /* enable output channel */
    hrtimer_output_channel_enable(HRTIMER0, HRTIMER_ST0_CH0);
    hrtimer_output_channel_enable(HRTIMER0, HRTIMER_ST0_CH1);
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
    gpio_config(); 
    hrtimer_config();
    hrtimer_output_exchange(HRTIMER0, HRTIMER_OUTPUT_EXCHANGE_ST0);
    while(1);
}
