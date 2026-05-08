/*!
    \file    main.c
    \brief   low voltage detector

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
#include "gd32g553q_eval.h"

#define PA10_ANALOG_INPUT 0

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure NVIC */
    nvic_irq_enable(LVD_VAVD_VOVD_VUVD_IRQn, 0U, 0U);
    /* enable clock */
    rcu_periph_clock_enable(RCU_PMU);

#if PA10_ANALOG_INPUT
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure GPIO for analog input */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLUP, GPIO_PIN_10);
#endif

    /* initialize LED1 */
    gd_eval_led_init(LED1);
    /* turn on LED1 */
    gd_eval_led_on(LED1);
    /* configure EXTI_16 */
    exti_init(EXTI_16, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
#if PA10_ANALOG_INPUT
    /* configure the LVD threshold to 1.2v */
    pmu_lvd_select(PMU_LVDT_7);
#else
    /* configure the LVD threshold to 3.0v */
    pmu_lvd_select(PMU_LVDT_6);
#endif
    pmu_lvd_enable();

    while(1) {
    }
}
