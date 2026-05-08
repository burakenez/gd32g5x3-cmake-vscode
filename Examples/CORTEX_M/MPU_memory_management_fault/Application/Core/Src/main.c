/*!
    \file    main.c
    \brief   MPU's functional attributes example

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
#include "main.h"
#include "gd32g553q_eval.h"

#define SP_PROCESS                  0x02   /* Process stack */
#define SP_MAIN                     0x00   /* Main stack */
#define THREAD_MODE_PRIVILEGED      0x00   /* Thread mode has privileged access */
#define THREAD_MODE_UNPRIVILEGED    0x01   /* Thread mode has unprivileged access */

__IO uint32_t value = 0U;
__IO uint32_t sram_handler_flag = 0U;
__IO uint32_t flash_handler_flag = 0U;

static void mpu_config(void);
/*!
    \brief      configure the MPU attributes
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mpu_config(void)
{
    mpu_region_init_struct region_init_struct;
    mpu_attribute_init_struct attribute_init_struct;

    mpu_region_struct_para_init(&region_init_struct);
    mpu_attribute_struct_para_init(&attribute_init_struct);

    /* disable the MPU */
    mpu_disable();

    /* configure the MPU attributes for SRAM */
    region_init_struct.region_number         = MPU_REGION_NUMBER0;
    region_init_struct.region_base_address   = 0x20002000U;
    region_init_struct.region_limit_address  = 0x20002000U;
    region_init_struct.access_permission     = MPU_REGION_PRIVILEGED_RO;
    region_init_struct.shareability          = MPU_ACCESS_NOT_SHAREABLE;
    region_init_struct.instruction_exec      = MPU_INSTRUCTION_EXEC_PERMIT;
    region_init_struct.attribute_index       = MPU_ATTRIBUTE_NUMBER4;
    mpu_region_config(&region_init_struct);

    attribute_init_struct.attribute_number   = MPU_ATTRIBUTE_NUMBER4;
    attribute_init_struct.memory_type        = MPU_MEMORY_NORMAL;
    attribute_init_struct.outer_attributes   = MPU_NORMAL_OUTER_WT_TRAN_RW_ALLOC;
    attribute_init_struct.inner_attributes   = MPU_NORMAL_INNER_WT_TRAN_RW_ALLOC;
    mpu_attribute_config(&attribute_init_struct);
    mpu_region_enable();
    
    /* configure the MPU attributes for SRAM */
    region_init_struct.region_number         = MPU_REGION_NUMBER1;
    region_init_struct.region_base_address   = 0x20000000U;
    region_init_struct.region_limit_address  = 0x20001000U;
    region_init_struct.access_permission     = MPU_REGION_ALL_RW;
    region_init_struct.shareability          = MPU_ACCESS_NOT_SHAREABLE;
    region_init_struct.instruction_exec      = MPU_INSTRUCTION_EXEC_PERMIT;
    region_init_struct.attribute_index       = MPU_ATTRIBUTE_NUMBER4;
    mpu_region_config(&region_init_struct);

    attribute_init_struct.attribute_number   = MPU_ATTRIBUTE_NUMBER4;
    attribute_init_struct.memory_type        = MPU_MEMORY_NORMAL;
    attribute_init_struct.outer_attributes   = MPU_NORMAL_OUTER_WT_TRAN_RW_ALLOC;
    attribute_init_struct.inner_attributes   = MPU_NORMAL_INNER_WT_TRAN_RW_ALLOC;
    mpu_attribute_config(&attribute_init_struct);
    mpu_region_enable();

    /* configure the MPU attributes for FLASH */
    region_init_struct.region_number         = MPU_REGION_NUMBER2;
    region_init_struct.region_base_address   = 0x08000000U;
    region_init_struct.region_limit_address  = 0x08001FE0U;
    region_init_struct.access_permission     = MPU_REGION_ALL_RW;
    region_init_struct.shareability          = MPU_ACCESS_NOT_SHAREABLE;
    region_init_struct.instruction_exec      = MPU_INSTRUCTION_EXEC_PERMIT;
    region_init_struct.attribute_index       = MPU_ATTRIBUTE_NUMBER1;
    mpu_region_config(&region_init_struct);

    attribute_init_struct.attribute_number   = MPU_ATTRIBUTE_NUMBER1;
    attribute_init_struct.memory_type        = MPU_MEMORY_NORMAL;
    attribute_init_struct.outer_attributes   = MPU_NORMAL_OUTER_WT_TRAN_RW_ALLOC;
    attribute_init_struct.inner_attributes   = MPU_NORMAL_INNER_WT_TRAN_RW_ALLOC;
    mpu_attribute_config(&attribute_init_struct);
    mpu_region_enable();
    
    /* configure the MPU attributes for FLASH */
    region_init_struct.region_number         = MPU_REGION_NUMBER3;
    region_init_struct.region_base_address   = 0x08002000U;
    region_init_struct.region_limit_address  = 0x08002000U;
    region_init_struct.access_permission     = MPU_REGION_PRIVILEGED_RW;
    region_init_struct.shareability          = MPU_ACCESS_NOT_SHAREABLE;
    region_init_struct.instruction_exec      = MPU_INSTRUCTION_EXEC_PERMIT;
    region_init_struct.attribute_index       = MPU_ATTRIBUTE_NUMBER7;
    mpu_region_config(&region_init_struct);

    attribute_init_struct.attribute_number   = MPU_ATTRIBUTE_NUMBER7;
    attribute_init_struct.memory_type        = MPU_MEMORY_NORMAL;
    attribute_init_struct.outer_attributes   = MPU_NORMAL_OUTER_WT_TRAN_RW_ALLOC;
    attribute_init_struct.inner_attributes   = MPU_NORMAL_INNER_WT_TRAN_RW_ALLOC;
    mpu_attribute_config(&attribute_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    mpu_enable(MPU_MODE_PRIV_DEFAULT);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure MPU */
    mpu_config();
    /* configure systick */
    systick_config();
    /* initilize the LEDs, USART and key */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_com_init(EVAL_COM);
    gd_eval_key_init(KEY_A, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_B, KEY_MODE_GPIO);
    while(1) {
        if(RESET == gd_eval_key_state_get(KEY_A)) {
            delay_1ms(50U);
            if(RESET == gd_eval_key_state_get(KEY_A)) {
                /* turn on LED1 */
                gd_eval_led_on(LED1);
                sram_handler_flag = 1U;
                /* A memory management fault occurred */
                *(uint32_t *)0x20002000U = 0x55555555U;
            }
            while(RESET == gd_eval_key_state_get(KEY_A)) {
            }
        }
        if(RESET == gd_eval_key_state_get(KEY_B)) {
            delay_1ms(50U);
            if(RESET == gd_eval_key_state_get(KEY_B)) {
                /* turn on LED2 */
                gd_eval_led_on(LED2);
                flash_handler_flag = 1U;
                /* Switch to unprivileged mode */
                __set_CONTROL(THREAD_MODE_UNPRIVILEGED);
                /* Execute ISB instruction to flush pipeline as recommended by Arm */
                __ISB();
                /* A memory management fault occurred */
                value = *(uint32_t *)0x08002000U;
            }
            while(RESET == gd_eval_key_state_get(KEY_B)) {
            }
        }
    }
}
