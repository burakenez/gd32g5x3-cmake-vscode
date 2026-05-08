/*!
    \file    main.c
    \brief   TRIGSEL select TRIGSEL IN0 as trigger source to TRIGSEL OUT0 and OUT1 example

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

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure PA0(TRIGSEL IN0) as alternate function */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_0);

    /* configure PA15(TRIGSEL OUT0) as alternate function */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_15);
    gpio_af_set(GPIOA, GPIO_AF_12, GPIO_PIN_15);

#if (defined(GD32G553))
    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure PF4(TRIGSEL OUT1) as alternate function */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_4);
    gpio_af_set(GPIOF, GPIO_AF_13, GPIO_PIN_4);
#else
    rcu_periph_clock_enable(RCU_GPIOB);

    /* configure PB14(TRIGSEL OUT1) as alternate function */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_14);
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_14);
#endif
    
    /* enable the TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* configure TRIGSEL OUT0 select TRIGSEL IN0 as trigger source */
    trigsel_init(TRIGSEL_OUTPUT_TRIGSEL_OUT0, TRIGSEL_INPUT_TRIGSEL_IN0);
    /* configure TRIGSEL OUT1 select TRIGSEL IN0 as trigger source */
    trigsel_init(TRIGSEL_OUTPUT_TRIGSEL_OUT1, TRIGSEL_INPUT_TRIGSEL_IN0);

    while(1) {
    }
}
