/*!
    \file    main.c
    \brief   use CLA0 result as CLA1 input

    \\version 2026-02-04, V1.5.0, firmware for GD32G5x3
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

#if (defined(GD32G553))

void rcu_config(void);
void cla0_input_gpio_config();
void cla0_output_gpio_config(void);
void cla1_input_gpio_config();
void cla1_output_gpio_config(void);
void cla0_config(void);
void cla1_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();
    /* configure RCU */
    rcu_config();
    /* configure cla0 input GPIO */
    cla0_input_gpio_config();
    /* configure cla0 output GPIO */
    cla0_output_gpio_config();
    /* configure cla1 input GPIO */
    cla1_input_gpio_config();
    /* configure cla1 output GPIO */
    cla1_output_gpio_config();
    /* reset CLA */
    cla_deinit();
    /* CLA0 configuration */
    cla0_config();
    /* CLA1 configuration */
    cla1_config();

    while(1){
        delay_1ms(50);
        gpio_bit_toggle(GPIOB, GPIO_PIN_6|GPIO_PIN_0);
        gpio_bit_toggle(GPIOA, GPIO_PIN_8);
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
    \brief      configure the CLA0 input GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla0_input_gpio_config()
{
    /* configure PB5 as output */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_5);

    /* configure PB6 as output */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_6);

    /* PB5 outputs HIGH */
    gpio_bit_set(GPIOB, GPIO_PIN_5);
     /* PB6 outputs HIGH */
    gpio_bit_set(GPIOB, GPIO_PIN_6);
}

/*!
    \brief      configure the CLA0OUT GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla0_output_gpio_config(void)
{
    /* connect PA2 to CLA0OUT */
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_2);
    /* configure GPIO pins of CLA1 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ,GPIO_PIN_2);
}

/*!
    \brief      configure the CLA1 input GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla1_input_gpio_config()
{
    /* configure PB0 as output */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_0);

    /* configure PA8 as output */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_8);

    /* PB0 outputs HIGH */
    gpio_bit_set(GPIOB, GPIO_PIN_0);
    /* PA8 outputs LOW */
    gpio_bit_reset(GPIOA, GPIO_PIN_8);
}

/*!
    \brief      configure the CLA1OUT GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla1_output_gpio_config(void)
{
    /* connect PA7 to CLA1OUT */
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_7);
    /* configure GPIO pins of CLA1 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ,GPIO_PIN_7);
}

/*!
    \brief      configure the CLA0 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla0_config(void)
{
    /* select CLAIN4(PB6) as input of SIGS0 */
    cla_sigs_input_config(CLA0, SIGS0, CLA0SIGS0_CLAIN4);
    /* select CLAIN3(PB5) as input of MUX1 */
    cla_sigs_input_config(CLA0, SIGS1, CLA0SIGS1_CLAIN3);
    /* IN0 & IN1 */
    cla_lcu_control_config(CLA0, 0xC0);
    /* select flip-flop output as CLA output */
    cla_output_config(CLA0, FLIP_FLOP_OUTPUT);
    cla_flip_flop_clocksource_config(CLA0, HCLK);
    cla_flip_flop_clockpolarity_config(CLA0, CLA_CLOCKPOLARITY_POSEDGE);
    cla_flip_flop_output_reset(CLA0);
    cla_output_enable(CLA0);
    /* enable CLA0 */
    cla_enable(CLA0);
}

/*!
    \brief      configure the CLA1 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla1_config(void)
{
    /* select CLAIN8(PB0) as input of SIGS0 */
    cla_sigs_input_config(CLA1, SIGS0, CLA1SIGS0_CLAIN8);
    /* select CLAIN11(PA8) as input of SIGS1 */
    cla_sigs_input_config(CLA1, SIGS1, CLA1SIGS1_CLAIN11);
    /* IN0^IN1^IN2*/
    cla_lcu_control_config(CLA1, 0x96);
    /* select flip-flop result as CLA output */
    cla_output_config(CLA1, FLIP_FLOP_OUTPUT);
    cla_flip_flop_clocksource_config(CLA1, HCLK);
    cla_flip_flop_clockpolarity_config(CLA1, CLA_CLOCKPOLARITY_POSEDGE);
    cla_flip_flop_output_reset(CLA1);
    /* enable CLA1 output */
    cla_output_enable(CLA1);
    /* enable CLA1 */
    cla_enable(CLA1);
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
