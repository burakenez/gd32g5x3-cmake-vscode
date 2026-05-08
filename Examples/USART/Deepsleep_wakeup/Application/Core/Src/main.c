/*!
    \file    main.c
    \brief   Deepsleep wakeup

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

#define PLLPSC              (2U - 1U)
#define PLLN                (108U)
#define PLLP                ((2U >> 1U) - 1U)
#define PLLQ                (2U)
#define PLLR                (2U)
#define PLLPSC_REG_OFFSET   0U
#define PLLN_REG_OFFSET     6U
#define PLLP_REG_OFFSET     16U
#define PLLQ_REG_OFFSET     23U
#define PLLR_REG_OFFSET     27U

extern __IO uint8_t counter0;

static void system_clock_reconfig(void);

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
    /* configure led GPIO */
    gd_eval_led_init(LED1);
    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* USART configuration the RCU_USARTSRC_IRC8M as USART clock */
    rcu_usart_clock_config(IDX_USART0, RCU_USARTSRC_IRC8M);
    gd_eval_com_init(EVAL_COM);

    /* USART0 Wakeup EXTI line configuration */
    exti_init(EXTI_28, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    delay_1ms(2000);

    /* use start bit wakeup MCU */
    usart_wakeup_mode_config(EVAL_COM, USART_WUM_STARTB);

    /* enable USART */
    usart_enable(EVAL_COM);

    /* ensure USART is enabled */
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_REA)) {
    }

    /* check USART is not transmitting */
    while(SET == usart_flag_get(EVAL_COM, USART_FLAG_BSY)) {
    }

    usart_wakeup_enable(EVAL_COM);
    /* enable the WUIE interrupt */
    usart_interrupt_enable(EVAL_COM, USART_INT_WU);
    /* turn off selected led */
    gd_eval_led_off(LED1);

    /* enable PMU APB clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enter deep-sleep mode */
    pmu_to_deepsleepmode(PMU_LDO_NORMAL,WFI_CMD);

    /* wait a WUIE interrupt event */
    while(0x00 == counter0);

    /* disable USART peripheral in deepsleep mode */
    usart_wakeup_disable(EVAL_COM);

    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_RBNE));

    usart_data_receive(EVAL_COM);

    usart_receive_config(EVAL_COM, USART_RECEIVE_ENABLE);

    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TC));

    /* disable the USART */
    usart_disable(EVAL_COM);

    /* reconfigure system clock */
    system_clock_reconfig();
    /* configure systick */
    systick_config();

    while(1) {
    }
}

/*!
    \brief      reconfigure system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_reconfig(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        while(1){
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT(7);

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB3 = AHB/1 */
    RCU_CFG0 |= RCU_APB3_CKAHB_DIV1;
    /* APB2 = AHB/1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/1 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;

    /* PLLP = (CK_HXTAL / 2) * 108 / 2 = 216 MHz */
    RCU_PLL &= ~(RCU_PLL_PLLSEL | RCU_PLL_PLLPSC | RCU_PLL_PLLN | RCU_PLL_PLLP | RCU_PLL_PLLQ | RCU_PLL_PLLR);
    RCU_PLL |= (RCU_PLLSRC_HXTAL | (PLLPSC << PLLPSC_REG_OFFSET) | (PLLN << PLLN_REG_OFFSET) | (PLLP << PLLP_REG_OFFSET) | (PLLQ << PLLQ_REG_OFFSET) | (PLLR << PLLR_REG_OFFSET));
    
    /* enable PLLP, PLLQ, PLLR */
    RCU_PLL |= (RCU_PLL_PLLPEN | RCU_PLL_PLLQEN | RCU_PLL_PLLREN);

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLLP != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
}

/*!
    \brief      LED spark
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t time_delay = 0;

    if(0x00 != time_delay) {
        if(time_delay < 5) {
            /* light on */
            gd_eval_led_on(LED1);
        } else {
            /* light off */
            gd_eval_led_off(LED1);
        }

        time_delay--;
    } else {
        time_delay = 100;
    }
}
