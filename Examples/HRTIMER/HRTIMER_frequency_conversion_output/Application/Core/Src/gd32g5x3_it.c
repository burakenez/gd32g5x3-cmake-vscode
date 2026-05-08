/*!
    \file    gd32g5x3_it.c
    \brief   interrupt service routines

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

#include "gd32g5x3_it.h"

#define SRAM_ECC_ERROR_HANDLE(s)    do{}while(1)
#define FLASH_ECC_ERROR_HANDLE(s)   do{}while(1)

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    if(SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_TCMSRAMECCME)) {
        SRAM_ECC_ERROR_HANDLE("TCMSRAM multi-bits non-correction ECC error\r\n");
    }else if(SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_SRAM1ECCME)) {
        SRAM_ECC_ERROR_HANDLE("SRAM1 multi-bits non-correction ECC error\r\n");
    }else if(SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_SRAM0ECCME)) {
        SRAM_ECC_ERROR_HANDLE("SRAM0 multi-bits non-correction ECC error\r\n");
    }else if(SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_FLASHECC)){
        FLASH_ECC_ERROR_HANDLE("FLASH ECC error\r\n");
    }else{
        /* if NMI exception occurs, go to infinite loop */
        /* HXTAL clock monitor NMI error or NMI pin error */
        while(1) {
        }
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles TIMER0 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER0_UP_IRQHandler(void)
{
    static uint8_t odd_flag = 1;
    if(SET == timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_UP)) {
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_UP);

        /* disable the Slave_TIMER0 and Slave_TIMER1 update event */
        HRTIMER_CTL0(HRTIMER0) |= (HRTIMER_CTL0_ST0UPDIS | HRTIMER_CTL0_ST1UPDIS);
        if(odd_flag){
            /* Update the value of PER per control cycle: PER = period */
            HRTIMER_STXCAR(HRTIMER0, HRTIMER_SLAVE_TIMER0) = 34560U;
            HRTIMER_STXCAR(HRTIMER0, HRTIMER_SLAVE_TIMER1) = 34560U;
            /* Update the value of CMP0 per control cycle: CMP0 = period / 2 */
            HRTIMER_STXCMP0V(HRTIMER0, HRTIMER_SLAVE_TIMER0) = 34560U / 2;
            HRTIMER_STXCMP0V(HRTIMER0, HRTIMER_SLAVE_TIMER1) = 34560U / 2;
        }else{
            /* Update the value of PER per control cycle: PER = period */
            HRTIMER_STXCAR(HRTIMER0, HRTIMER_SLAVE_TIMER0) = 46080U;
            HRTIMER_STXCAR(HRTIMER0, HRTIMER_SLAVE_TIMER1) = 46080U;
            /* Update the value of CMP0 per control cycle: CMP0 = period / 2 */
            HRTIMER_STXCMP0V(HRTIMER0, HRTIMER_SLAVE_TIMER0) = 46080U / 2;
            HRTIMER_STXCMP0V(HRTIMER0, HRTIMER_SLAVE_TIMER1) = 46080U / 2;
        }
        /* enable the Slave_TIMER0 and Slave_TIMER1 update event */
        HRTIMER_CTL0(HRTIMER0) &= ~ (HRTIMER_CTL0_ST0UPDIS | HRTIMER_CTL0_ST1UPDIS);

        odd_flag = !odd_flag;
    }
}