/*!
    \file    main.c
    \brief   logic AND function use ADC_CONV and GPIO as input

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

#if (defined(GD32G553))

void rcu_config(void);
void gpio_config(void);
void cla0_input_gpio_config();
void cla0_output_gpio_config(void);
void cla0_config(void);
void adc_config(void);

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
    /*configure ADC */
    adc_config();
    /* enable TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* select  to trigger CLA_IN11 */
    trigsel_init(TRIGSEL_OUTPUT_CLA_IN11, TRIGSEL_INPUT_ADC0_CONV);
    /* lock trigger register */
    trigsel_register_lock_set(TRIGSEL_OUTPUT_CLA_IN11);
    /* configure cla0 input GPIO */
    cla0_input_gpio_config();
    /* configure cla0 output GPIO */
    cla0_output_gpio_config();
    /* reset CLA */
    cla_deinit();
    /* CLA0 configuration */
    cla0_config();

    while(1){
         /* delay 1ms */
         delay_1ms(1U);
         /* ADC software trigger enable */
         adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
         /* wait the end of conversion flag */
         while(!adc_flag_get(ADC0, ADC_FLAG_EOC));
         /* clear the end of conversion flag */
         adc_flag_clear(ADC0, ADC_FLAG_EOC);
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
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* configure ADC clock */
    rcu_adc_clock_config(IDX_ADC0, RCU_ADCSRC_CKSYS);
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
    /* configure PB6 as input */
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
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
    /* configure GPIO pins of CLA0 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ,GPIO_PIN_2);
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
    /* select ADC_CONV as input of SIGS1 */
    cla_sigs_input_config(CLA0, SIGS1, CLA0SIGS1_TRIGSEL_CLA_IN11);
    /* IN0 & IN1 */
    cla_lcu_control_config(CLA0, 0xC0);
    /* select LCU result as CLA output */
    cla_output_config(CLA0, LCU_RESULT);
    /* enable CLA0 output */
    cla_output_enable(CLA0);
    /* enable CLA0 */
    cla_enable(CLA0);
}

/*!
    \brief      configure ADC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* reset ADC */
    adc_deinit(ADC0);
    /* ADC clock configuration */
    adc_clock_config(ADC0, ADC_CLK_SYNC_HCLK_DIV16);
    /* ADC scan mode enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment configuration */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length configuration */
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);
    /* ADC routine channel configuration */
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_0, 0);
    /* ADC trigger configuration */
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
    /* enable ADC interface */
    adc_enable(ADC0);
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC calibration mode configuration */
    adc_calibration_mode_config(ADC0, ADC_CALIBRATION_OFFSET_MISMATCH);
    /* ADC calibration number configuration */
    adc_calibration_number(ADC0, ADC_CALIBRATION_NUM1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
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
