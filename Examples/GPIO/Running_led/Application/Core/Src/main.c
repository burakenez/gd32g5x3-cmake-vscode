/*!
    \file    main.c
    \brief   running LED

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
#include "gd32g553q_eval.h"

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

#if (defined(GD32G553))
    /* enable the LED GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOE);

    /* configure LED GPIO pin */ 
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
    /* reset LED GPIO pin */
    gpio_bit_reset(GPIOE, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
#else
    /* enable the LED GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* configure LED GPIO pin */ 
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED1_PIN);
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED2_PIN | LED3_PIN | LED4_PIN);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, LED1_PIN);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, LED2_PIN | LED3_PIN | LED4_PIN);
    /* reset LED GPIO pin */
    gpio_bit_reset(GPIOE, LED1_PIN);
    gpio_bit_reset(GPIOB, LED2_PIN | LED3_PIN | LED4_PIN);
#endif /* (defined(GD32G553)) */

    while(1) {
        /* turn on LED1, turn off LED4 */
        gpio_bit_set(LED1_GPIO_PORT, LED1_PIN);
        gpio_bit_reset(LED4_GPIO_PORT, LED4_PIN);
        delay_1ms(500);

        /* turn on LED2, turn off LED1 */
        gpio_bit_set(LED2_GPIO_PORT, LED2_PIN);
        gpio_bit_reset(LED1_GPIO_PORT, LED1_PIN);
        delay_1ms(500);

        /* turn on LED3, turn off LED2 */
        gpio_bit_set(LED3_GPIO_PORT, LED3_PIN);
        gpio_bit_reset(LED2_GPIO_PORT, LED2_PIN);
        delay_1ms(500);

        /* turn on LED4, turn off LED3 */
        gpio_bit_set(LED4_GPIO_PORT, LED4_PIN);
        gpio_bit_reset(LED3_GPIO_PORT, LED3_PIN);
        delay_1ms(500);
    }
}
