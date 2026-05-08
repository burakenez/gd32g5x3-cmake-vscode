/*!
    \file    main.c
    \brief   HRTIMER0 timebase config

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


void hrtimer_config(void);

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
    period: 6912/(216M*32)=1us
  ----------------------------------------------------------------------- */
    hrtimer_baseinit_parameter_struct baseinit_para;
    rcu_hrtimer_clock_config(RCU_HRTIMERSRC_CKSYS);
    /* clock config */
    rcu_periph_clock_enable(RCU_HRTIMER);

    /* periodic DLL calibration */
    hrtimer_dll_calibration_start(HRTIMER0, HRTIMER_CALIBRATION_ONCE);
    while(RESET == hrtimer_common_flag_get(HRTIMER0, HRTIMER_FLAG_DLLCAL));

    /* Slave_TIMER0 time base clock config  */
    hrtimer_baseinit_struct_para_init(&baseinit_para);
    baseinit_para.period            = 384;
    baseinit_para.prescaler         = HRTIMER_PRESCALER_MUL32;
    baseinit_para.repetitioncounter = 0;
    baseinit_para.counter_mode      = HRTIMER_COUNTER_MODE_CONTINOUS;
    hrtimer_timers_base_init(HRTIMER0, HRTIMER_SLAVE_TIMER0, &baseinit_para);
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
    hrtimer_config();

    while(1);
}
