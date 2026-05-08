/*!
    \file    main.c
    \brief   logic XOR function use TIMER_CH1 and GPIO as input

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

#if (defined(GD32G553))

void rcu_config(void);
void timer1_channel1_config(void);
void cla2_input_gpio_config();
void cla2_output_gpio_config(void);
void cla2_config(void);
void timer1_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure RCU */
    rcu_config();
    /* enable TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* select TIMER1 CH1 to trigger CLA_IN5 */
    trigsel_init(TRIGSEL_OUTPUT_CLA_IN5, TRIGSEL_INPUT_TIMER1_CH1);
    /* lock trigger register */
    trigsel_register_lock_set(TRIGSEL_OUTPUT_CLA_IN5);
    /* configure timer1 channel 1(PA1)*/
    timer1_channel1_config();
    /* configure cla2 input GPIO */
    cla2_input_gpio_config();
    /* configure cla2 output GPIO */
    cla2_output_gpio_config();
    /* configure the TIMER1 peripheral */
    timer1_config();
    /* reset CLA */
    cla_deinit();
    /* CLA2 configuration */
    cla2_config();
    while(1){
    }
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable CLA clock */
    rcu_periph_clock_enable(RCU_CLA);
}

/*!
    \brief      configure the CLA2 input GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla2_input_gpio_config()
{
    /* configure PB2 as input */
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_2);
}

/*!
    \brief      configure the CLA2OUT GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla2_output_gpio_config(void)
{
    /* connect PB5 to CLA2OUT */
    gpio_af_set(GPIOB, GPIO_AF_12, GPIO_PIN_5);
    /* configure GPIO pins of CLA2 */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ,GPIO_PIN_5);
}

/*!
    \brief      configure the CLA2 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla2_config(void)
{
    /* select TIMER1_CH1 as input of SIGS0 */
    cla_sigs_input_config(CLA2, SIGS0, CLA2SIGS0_TRIGSEL_CLA_IN5);
    /* select CLAIN10(PB2) as input of SIGS1 */
    cla_sigs_input_config(CLA2, SIGS1, CLA2SIGS1_CLAIN10);
    /* IN0^IN1 */
    cla_lcu_control_config(CLA2, 0x3C);
    /* select LCU result as CLA output */
    cla_output_config(CLA2, LCU_RESULT);
    /* enable CLA2 output */
    cla_output_enable(CLA2);
    /* enable CLA2 */
    cla_enable(CLA2);
}

/*!
    \brief      configure TIMER1 channel 1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer1_channel1_config(void)
{
    /*configure PA1(TIMER1 CH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ,GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_1);
}

/*!
    \brief      configure the TIMER1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer1_config(void)
{
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_SYSCFG);
    /* TIMER1 configuration */
    timer_deinit(TIMER1);
    /* initialize TIMER1 init parameter struct */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 2160 - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 20000 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* initialize TIMER1 channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER1 channel output function */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocinitpara);
    /* configure TIMER1 channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 10000);
    /* CH1 configuration in OC TOGGLE mode */
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    /* configure TIMER1 channel output shadow function */
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload shadow enable */
    timer_auto_reload_shadow_enable(TIMER1);

    /* TIMER1 counter enable */
    timer_enable(TIMER1);
}

#else

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    while(1){
    }
}

#endif /* (defined(GD32G553)) */
