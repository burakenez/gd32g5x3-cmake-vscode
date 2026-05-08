/*!
    \file  main.c
    \brief keyboard polling mode

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

    /* enable the LED2 GPIO clock */
    rcu_periph_clock_enable(LED2_GPIO_CLK);
    /* configure LED2 GPIO pin */ 
    gpio_mode_set(LED2_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED2_PIN);
    gpio_output_options_set(LED2_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, LED2_PIN);
    /* reset LED2 GPIO pin */
    gpio_bit_reset(LED2_GPIO_PORT, LED2_PIN);

    /* enable the key GPIO clock */
    rcu_periph_clock_enable(KEY_B_GPIO_CLK);
    /* configure key pin as input */
    gpio_mode_set(KEY_B_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, KEY_B_PIN);

    while(1) {
        /* check whether the key is pressed */
        if(RESET == gpio_input_bit_get(KEY_B_GPIO_PORT, KEY_B_PIN)) {
            delay_1ms(100U);

            /* check whether the key is pressed */
            if(RESET == gpio_input_bit_get(KEY_B_GPIO_PORT, KEY_B_PIN)) {
                /* toggle LED2 GPIO pin */
                gpio_bit_toggle(LED2_GPIO_PORT, LED2_PIN);
            }
        }
    }
}
