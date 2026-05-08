/*!
    \file    main.c
    \brief   ADC0 ADC1 ADC2 routine parallel demo

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
#include <stdio.h>
#include "gd32g553q_eval.h"

uint16_t adc_value[6];

/* configure the different system clocks */
void rcu_config(void);
/* configure the GPIO peripheral */
void gpio_config(void);
/* configure the DMA peripheral */
void dma_config(void);
/* configure the ADC peripheral */
void adc_config(void);
/* configure the timer peripheral */
void timer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* system clocks configuration */
    rcu_config();
    /* systick configuration */
    systick_config();
    /* GPIO configuration */
    gpio_config();
    /* configure EVAL_COM */
    gd_eval_com_init(EVAL_COM);
    /* TIMER configuration */
    timer_config();
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();

    while(1) {
        delay_1ms(1000);
        printf("\n\r ADC0: PC0, adc_value[0] = %04X \n\r",adc_value[0]);
        printf("\n\r ADC1: PC1, adc_value[1] = %04X \n\r",adc_value[1]);
        printf("\n\r ADC2: PB0, adc_value[2] = %04X \n\r",adc_value[2]);
        printf("\n\r ADC0: PC1, adc_value[3] = %04X \n\r",adc_value[3]);
        printf("\n\r ADC1: PC0, adc_value[4] = %04X \n\r",adc_value[4]);
        printf("\n\r ADC2: PB1, adc_value[5] = %04X \n\r",adc_value[5]);
        printf("\n\r ******************* \n\r");
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC1);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC2);
    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_1);
    gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_1);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_init_struct;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA single data mode */
    dma_init_struct.request = DMA_REQUEST_ADC0;
    dma_init_struct.periph_addr = (uint32_t)(&ADC_SYNCDATA);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_addr = (uint32_t)(&adc_value);
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.number = 6;
    dma_init_struct.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_init_struct);

    /* enable DMA circulation mode */
    dma_circulation_enable(DMA0, DMA_CH0);
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* reset ADC */
    adc_deinit(ADC0);
    adc_deinit(ADC1);
    adc_deinit(ADC2);
    /* ADC clock config */
    adc_clock_config(ADC0, ADC_CLK_SYNC_HCLK_DIV8);
    /* ADC scan mode enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC1, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC2, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
    adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 2);
    adc_channel_length_config(ADC1, ADC_ROUTINE_CHANNEL, 2);
    adc_channel_length_config(ADC2, ADC_ROUTINE_CHANNEL, 2);

    /* ADC routine channel config */
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_5, 24);
    adc_routine_channel_config(ADC0, 1, ADC_CHANNEL_6, 24);
    adc_routine_channel_config(ADC1, 0, ADC_CHANNEL_6, 24);
    adc_routine_channel_config(ADC1, 1, ADC_CHANNEL_5, 24);
    adc_routine_channel_config(ADC2, 0, ADC_CHANNEL_11, 24);
    adc_routine_channel_config(ADC2, 1, ADC_CHANNEL_0, 24);

    /* ADC trigger config */
    trigsel_init(TRIGSEL_OUTPUT_ADC0_ROUTRG, TRIGSEL_INPUT_TIMER1_CH1);
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_RISING);
    adc_external_trigger_config(ADC1,ADC_ROUTINE_CHANNEL,EXTERNAL_TRIGGER_DISABLE);
    adc_external_trigger_config(ADC2,ADC_ROUTINE_CHANNEL,EXTERNAL_TRIGGER_DISABLE);

    /* configure the ADC sync mode */
    adc_sync_mode_config(ADC_TRIPLE_ROUTINE_PARALLEL);
    adc_sync_dma_config(ADC_SYNC_DMA_MODE0);
    adc_sync_dma_request_after_last_enable();
    /* enable ADC interface */
    adc_enable(ADC0);
    adc_enable(ADC1);
    adc_enable(ADC2);
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC calibration mode config */
    adc_calibration_mode_config(ADC0, ADC_CALIBRATION_OFFSET_MISMATCH);
    adc_calibration_mode_config(ADC1, ADC_CALIBRATION_OFFSET_MISMATCH);
    adc_calibration_mode_config(ADC2, ADC_CALIBRATION_OFFSET_MISMATCH);
    /* ADC calibration number config */
    adc_calibration_number(ADC0, ADC_CALIBRATION_NUM1);
    adc_calibration_number(ADC1, ADC_CALIBRATION_NUM1);
    adc_calibration_number(ADC2, ADC_CALIBRATION_NUM1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    adc_calibration_enable(ADC1);
    adc_calibration_enable(ADC2);
}

/*!
    \brief      configure the timer peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_struct_para_init(&timer_initpara);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 19999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* CH1 configuration in PWM mode0 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 3999);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* enable TIMER1 */
    timer_enable(TIMER1);
}
