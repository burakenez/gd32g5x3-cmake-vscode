/*!
    \file    main.c
    \brief   LPTIMER decoder mode example

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

#if (defined(GD32G553))

/* configure the GPIO ports */
void gpio_config(void);
/* configure the NVIC */
void nvic_config(void);
/* configure the LPTIMER peripheral */
void lptimer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure led GPIO */
    gd_eval_led_init(LED1);
    /* turn off selected led */
    gd_eval_led_off(LED1);
    /* configure the GPIO ports */
    gpio_config();
    /* configure the NVIC */
    nvic_config();
    /* configure the LPTIMER peripheral */
    lptimer_config();

    while(1) {
    }
}

/*!
    \brief      configure the NVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(LPTIMER_IRQn, 0U, 0U);
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* LPTIMER GPIO RCU */
    rcu_periph_clock_enable(RCU_GPIOC);

    /* LPTIMER GPIO */
    /*configure PC1(LPTIMER_O) as alternate function*/
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_1);
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_1);

    /*configure PC0(LPTIMER_IN0) as alternate function*/
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_0);
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_0);

    /*configure PC2(LPTIMER_IN1) as alternate function*/
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_2);
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_2);
}

/*!
    \brief      configure the LPTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lptimer_config(void)
{
    /* ----------------------------------------------------------------------------
    LPTIMER Configuration:
    LPTIMER is used in decoder mode0, and it will count with two quadrature signals
    which inputs of LPTIMER_IN0(PC0) and LPTIMER_IN1(PC2).

    LPTIMER count with internal clock IRC32K, the prescaler is 1, the period is 100.
    LPTIMER configuration: generate 1 PWM signal with the duty cycle:
    LPTIMERCLK = IRC32K / 1 = 32KHz
    LPTIMER_O duty cycle = (50/ 100)* 100  = 50%
    ---------------------------------------------------------------------------- */
    /* LPTIMER configuration */
    lptimer_parameter_struct lptimer_structure;
    /* LPTIMER clock */
    rcu_periph_clock_enable(RCU_LPTIMER);
    rcu_osci_on(RCU_IRC32K);
    rcu_osci_stab_wait(RCU_IRC32K);
    rcu_lptimer_clock_config(RCU_LPTIMERSRC_IRC32K);

    /* deinit a LPTIMER */
    lptimer_deinit();

    /* initialize LPTIMER init parameter struct */
    lptimer_struct_para_init(&lptimer_structure);
    /* LPTIMER configuration */
    lptimer_structure.clocksource      = LPTIMER_INTERNALCLK;
    lptimer_structure.prescaler        = LPTIMER_PSC_1;
    lptimer_structure.extclockpolarity = LPTIMER_EXTERNALCLK_BOTH;
    lptimer_structure.extclockfilter   = LPTIMER_EXTERNALCLK_FILTEROFF;
    lptimer_structure.triggermode      = LPTIMER_TRIGGER_SOFTWARE;
    lptimer_structure.extriggersource  = LPTIMER_EXTRIGGER_GPIO;
    lptimer_structure.extriggerfilter  = LPTIMER_TRIGGER_FILTEROFF;
    lptimer_structure.outputpolarity   = LPTIMER_OUTPUT_INVERTED;
    lptimer_structure.outputmode       = LPTIMER_OUTPUT_PWMORSINGLE;
    lptimer_structure.countersource    = LPTIMER_COUNTER_EXTERNAL;
    lptimer_init(&lptimer_structure);

    lptimer_register_shadow_disable();
    lptimer_timeout_disable();
    lptimer_inputremap(LPTIMER_INPUT0_GPIO, LPTIMER_INPUT1_GPIO);

    lptimer_interrupt_flag_clear(LPTIMER_INT_FLAG_CARM);
    lptimer_interrupt_enable(LPTIMER_INT_CARM);

    lptimer_decodemode0_enable();
    lptimer_countinue_start(99U, 50U);

    /* wait software trigger start LPTIMER counter */
    while(lptimer_counter_read() == 0) {
    }
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