/*!
    \file    main.c
    \brief   CAN Pretended Networking mode

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
#include "gd32g553q_eval.h"
#include "systick.h"
#include "string.h"

#define  BUFFER_SIZE    8U

/* global variable declarations */
#ifdef GD32G553
can_mailbox_descriptor_struct transmit_message;
#endif /* GD32G553 */
can_mailbox_descriptor_struct receive_message;
FlagStatus can0_match_flag, can0_timeout_flag;

/* function declarations */
void can_gpio_config(void);
void bsp_board_config(void);
void can_config(void);
void can_pn_config(void);
void communication_check(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
#ifdef GD32G553
    uint8_t i = 0;
    FlagStatus can_tx_state = RESET;
#endif /* GD32G553 */

    /* configure systick */
    systick_config();
    /* configure board */
    bsp_board_config();
    /* configure GPIO */
    can_gpio_config();
    /* initialize CAN and filter */
    can_config();

#ifdef GD32G553
    printf("\r\n communication test CAN0 and CAN1, please press KEY_A to start! \r\n");
#else
    printf("\r\n communication test CAN0, please press KEY_A to start! \r\n");
#endif /* GD32G553 */

    can_struct_para_init(CAN_MDSC_STRUCT, &receive_message);
#ifdef GD32G553
    can_struct_para_init(CAN_MDSC_STRUCT, &transmit_message);
    /* fill the data for CAN transmit */
    for(i = 0; i < BUFFER_SIZE; i++){
        transmit_message.data[i] = 0xA0 + i;
    }
    /* initialize transmit message */
    transmit_message.rtr = 0U;
    transmit_message.ide = 0U;
    transmit_message.code = CAN_MB_TX_STATUS_DATA;
    transmit_message.brs = 0U;
    transmit_message.fdf = 0U;
    transmit_message.prio = 0U;
    transmit_message.data_bytes = BUFFER_SIZE;
    transmit_message.id = 0x55U;
#endif /* GD32G553 */
    while(1) {
        /* test whether the KEY_A is pressed */
        if(0U == gd_eval_key_state_get(KEY_A)) {
            delay_1ms(100U);
            if(0U == gd_eval_key_state_get(KEY_A)) {
                /* enter Pretended Networking mode */
                can_operation_mode_enter(CAN0, CAN_PN_MODE);
                printf("\r\nCAN0 enter Pretended Networking mode \r\n");
                /* waiting for the KEY_A up */
                while(0U == gd_eval_key_state_get(KEY_A));
            }
        }
        #ifdef GD32G553
        /* test whether the KEY_B is pressed */
        if(0U == gd_eval_key_state_get(KEY_B)) {
            delay_1ms(100U);
            if(0U == gd_eval_key_state_get(KEY_B)) {
                if((RESET == can_tx_state) || (SET == can_flag_get(CAN1, CAN_FLAG_MB1))) {
                    can_tx_state = SET;
                    can_flag_clear(CAN1, CAN_FLAG_MB1);
                    /* transmit message */
                    can_mailbox_config(CAN1, 1, &transmit_message);

                    printf("\r\nCAN1 transmit data: \r\n");
                    for(i = 0U; i < transmit_message.data_bytes; i++) {
                        printf("%02x ", transmit_message.data[i]);
                    }
                }
                /* waiting for the KEY_B up */
                while(0U == gd_eval_key_state_get(KEY_B));
            }
        }
        #endif /* GD32G553 */
        communication_check();
    }
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN port clock */
    rcu_periph_clock_enable(RCU_GPIOA);
#ifdef GD32G553
    rcu_periph_clock_enable(RCU_GPIOB);
#endif /* GD32G553 */

    /* configure CAN0_TX GPIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_12);
    gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_12);

    /* configure CAN0_RX GPIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_11);
#ifdef GD32G553
    /* configure CAN1_TX GPIO */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_13);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_13);

    /* configure CAN1_RX GPIO */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_12);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_12);
#endif /* GD32G553 */
}

/*!
    \brief      configure BSP
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_board_config(void)
{
    /* configure USART */
    gd_eval_com_init(EVAL_COM);
    /* clock enable */
    rcu_periph_clock_enable(RCU_PMU);

    /* configure KEY_A key */
    gd_eval_key_init(KEY_A, KEY_MODE_GPIO);
    /* configure KEY_B key */
    gd_eval_key_init(KEY_B, KEY_MODE_GPIO);

    /* configure LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
}

/*!
    \brief      initialize CAN function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_config(void)
{
    can_parameter_struct can_parameter;
#ifdef GD32G553
    /* initialize CAN parameters structure */
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);

    /* select CK_APB2 as CAN's clock source */
    rcu_can_clock_config(IDX_CAN0, RCU_CANSRC_APB2);
    rcu_can_clock_config(IDX_CAN1, RCU_CANSRC_APB2);
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    /* initialize CAN register */
    can_deinit(CAN0);
    can_deinit(CAN1);

    /* configure CAN parameters */
    can_parameter.internal_counter_source = CAN_TIMER_SOURCE_BIT_CLOCK;
    can_parameter.self_reception = DISABLE;
    can_parameter.mb_tx_order = CAN_TX_HIGH_PRIORITY_MB_FIRST;
    can_parameter.mb_tx_abort_enable = ENABLE;
    can_parameter.local_priority_enable = DISABLE;
    can_parameter.mb_rx_ide_rtr_type = CAN_IDE_RTR_FILTERED;
    can_parameter.mb_remote_frame = CAN_STORE_REMOTE_REQUEST_FRAME;
    can_parameter.rx_private_filter_queue_enable = DISABLE;
    can_parameter.edge_filter_enable = DISABLE;
    can_parameter.protocol_exception_enable = DISABLE;
    can_parameter.rx_filter_order = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;
    can_parameter.memory_size = CAN_MEMSIZE_32_UNIT;
    /* filter configuration */
    can_parameter.mb_public_filter = 0U;
    /* baud rate 1Mbps */
    can_parameter.resync_jump_width = 1U;
    can_parameter.prop_time_segment = 2U;
    can_parameter.time_segment_1 = 4U;
    can_parameter.time_segment_2 = 1U;
    can_parameter.prescaler = 27U;

    /* initialize CAN */
    can_init(CAN0, &can_parameter);
    can_init(CAN1, &can_parameter);

    /* configure Pretended Networking mode */
    can_pn_config();

    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_WKUP_IRQn, 0U, 0U);

    /* EXTI configuration */
    exti_deinit();
    exti_init(EXTI_25, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    can_flag_clear(CAN0, CAN_FLAG_WAKEUP_MATCH);
    can_flag_clear(CAN0, CAN_FLAG_WAKEUP_TIMEOUT);
    exti_interrupt_flag_clear(EXTI_25);
    exti_interrupt_enable(EXTI_25);

    /* enable CAN WAKEUP interrupt */
    can_interrupt_enable(CAN0, CAN_INT_WAKEUP_MATCH);
    can_interrupt_enable(CAN0, CAN_INT_WAKEUP_TIMEOUT);

    can_operation_mode_enter(CAN1, CAN_NORMAL_MODE);
#else
    /* initialize CAN parameters structure */
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);

    /* select CK_APB2 as CAN's clock source */
    rcu_can_clock_config(IDX_CAN0, RCU_CANSRC_APB2);
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    /* initialize CAN register */
    can_deinit(CAN0);

    /* configure CAN parameters */
    can_parameter.internal_counter_source = CAN_TIMER_SOURCE_BIT_CLOCK;
    can_parameter.self_reception = DISABLE;
    can_parameter.mb_tx_order = CAN_TX_HIGH_PRIORITY_MB_FIRST;
    can_parameter.mb_tx_abort_enable = ENABLE;
    can_parameter.local_priority_enable = DISABLE;
    can_parameter.mb_rx_ide_rtr_type = CAN_IDE_RTR_FILTERED;
    can_parameter.mb_remote_frame = CAN_STORE_REMOTE_REQUEST_FRAME;
    can_parameter.rx_private_filter_queue_enable = DISABLE;
    can_parameter.edge_filter_enable = DISABLE;
    can_parameter.protocol_exception_enable = DISABLE;
    can_parameter.rx_filter_order = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;
    can_parameter.memory_size = CAN_MEMSIZE_32_UNIT;
    /* filter configuration */
    can_parameter.mb_public_filter = 0U;
    /* baud rate 1Mbps */
    can_parameter.resync_jump_width = 1U;
    can_parameter.prop_time_segment = 2U;
    can_parameter.time_segment_1 = 4U;
    can_parameter.time_segment_2 = 1U;
    can_parameter.prescaler = 27U;

    /* initialize CAN */
    can_init(CAN0, &can_parameter);

    /* configure Pretended Networking mode */
    can_pn_config();

    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_WKUP_IRQn, 0U, 0U);

    /* EXTI configuration */
    exti_deinit();
    exti_init(EXTI_25, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    can_flag_clear(CAN0, CAN_FLAG_WAKEUP_MATCH);
    can_flag_clear(CAN0, CAN_FLAG_WAKEUP_TIMEOUT);
    exti_interrupt_flag_clear(EXTI_25);
    exti_interrupt_enable(EXTI_25);

    /* enable CAN WAKEUP interrupt */
    can_interrupt_enable(CAN0, CAN_INT_WAKEUP_MATCH);
    can_interrupt_enable(CAN0, CAN_INT_WAKEUP_TIMEOUT);
#endif /* GD32G553 */
}

/*!
    \brief      initialize CAN Pretended Networking function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_pn_config(void)
{
    can_pn_mode_config_struct can_pn_parameter;
    can_pn_mode_filter_struct filter_parameter[2];

    /* initialize CAN Pretended Networking */
    can_struct_para_init(CAN_PN_MODE_INIT_STRUCT, &can_pn_parameter);
    /* initialize CAN Pretended Networking parameters */
    can_pn_parameter.timeout_int = ENABLE;
    can_pn_parameter.match_int = ENABLE;
    can_pn_parameter.num_matches = 1U;
    can_pn_parameter.match_timeout = 0xFFFFU;
    can_pn_parameter.frame_filter = CAN_PN_FRAME_FILTERING_ID;
    can_pn_parameter.id_filter = CAN_PN_ID_FILTERING_EXACT;

    /* initialize CAN Pretended Networking mode */
    can_pn_mode_config(CAN0, &can_pn_parameter);

    /* initialize CAN Pretended Networking filter */
    can_struct_para_init(CAN_PN_MODE_FILTER_STRUCT, &filter_parameter[0]);
    can_struct_para_init(CAN_PN_MODE_FILTER_STRUCT, &filter_parameter[1]);
    /* initialize CAN Pretended Networking filter parameters */
    filter_parameter[0].rtr = RESET;
    filter_parameter[0].ide = RESET;
    filter_parameter[0].id = 0x55 | CAN_STANDARD;
    filter_parameter[1].rtr = RESET;
    filter_parameter[1].ide = RESET;
    filter_parameter[1].id = 0x1FF | CAN_STANDARD;

    /* initialize CAN Pretended Networking filter */
    can_pn_mode_filter_config(CAN0, &filter_parameter[0], &filter_parameter[1]);
}

/*!
    \brief      check received data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void communication_check(void)
{
    uint8_t i = 0U;
    /* CAN0 wakeup match event */
    if(SET == can0_match_flag) {
        can0_match_flag = RESET;
        can_pn_mode_data_read(CAN0, 0U, &receive_message);
        printf("\r\nCAN0 woke up with match event \r\n");
        printf("\r\nCAN0 receive data: \r\n");
        for(i = 0U; i < receive_message.data_bytes; i++) {
            printf("%02x ", receive_message.data[i]);
        }
        gd_eval_led_toggle(LED1);
        can_pn_mode_exit(CAN0);
    }

    /* CAN0 wakeup timeout */
    if(SET == can0_timeout_flag) {
        printf("\r\nCAN0 woke up with timeout event \r\n");
        can0_timeout_flag = RESET;
        gd_eval_led_toggle(LED2);
        can_pn_mode_exit(CAN0);
    }
}
