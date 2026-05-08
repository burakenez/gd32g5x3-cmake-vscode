/*!
    \file    main.c
    \brief   WWDGT demo

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

#define SEL_IRC8M           0x00U
#define SEL_HXTAL           0x01U
#define SEL_PLLP            0x03U

#define PLLPSC_REG_OFFSET   0U
#define PLLN_REG_OFFSET     6U
#define PLLP_REG_OFFSET     16U
#define PLLQ_REG_OFFSET     23U
#define PLLR_REG_OFFSET     27U

#define PLLPSC              (2U - 1U)
#define PLLN                (108U)
#define PLLP                ((2U >> 1U) - 1U)
#define PLLQ                (2U)
#define PLLR                (2U)

/* reconfigure the system clock to 216M by PLL which selects HXTAL as its clock source*/
void system_clock_216m_hxtal_recongfig(void);

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

    /* configure LED1 and LED2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* turn off LED1 and LED2 */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);

    /* reconfigure the system clock to 216M by PLL which selects HXTAL as its clock source*/
    system_clock_216m_hxtal_recongfig();

    /* delay */
    delay_1ms(150);

    /* check if the system has resumed from WWDGT reset */
    if(RESET != rcu_flag_get(RCU_FLAG_WWDGTRST)) {
        /* WWDGTRST flag set */
        gd_eval_led_on(LED1);
        /* clear the WWDGTRST flag */
        rcu_all_reset_flag_clear();

        while(1);
    }

    /* enable WWDGT clock */
    rcu_periph_clock_enable(RCU_WWDGT);
    /*
     *  set WWDGT clock = (PCLK1 (54MHz)/4096)/4 = 3295.6Hz (~3034.35 us)
     *  set counter value to 127
     *  set window value to 80
     *  refresh window is: ~3034.35 * (127-80)= 14.42ms < refresh window < ~75.85 * (127-63) =19.4ms.
     */
    wwdgt_config(127, 80, WWDGT_CFG_PSC_DIV4);
    wwdgt_enable();

    while(1) {
        /* toggle LED2 */
        gd_eval_led_toggle(LED2);
        /* insert 18 ms delay */
        delay_1ms(18);
        /* update WWDGT counter */
        wwdgt_counter_update(127);
    }

}

/*!
    \brief      reconfigure the system clock to 216M by PLL which selects HXTAL as its clock source
    \param[in]  none
    \param[out] none
    \retval     none
*/
void system_clock_216m_hxtal_recongfig(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    } while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)) {
        while(1) {
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
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    /* PLLP = (CK_HXTAL / 2) * 108 / 2 = 216 MHz */
    RCU_PLL &= ~(RCU_PLL_PLLSEL | RCU_PLL_PLLPSC | RCU_PLL_PLLN | RCU_PLL_PLLP | RCU_PLL_PLLQ | RCU_PLL_PLLR);
    RCU_PLL |= (RCU_PLLSRC_HXTAL | (PLLPSC << PLLPSC_REG_OFFSET) | (PLLN << PLLN_REG_OFFSET) | (PLLP << PLLP_REG_OFFSET) | (PLLQ << PLLQ_REG_OFFSET) |
                (PLLR << PLLR_REG_OFFSET));

    /* enable PLLP, PLLQ, PLLR */
    RCU_PLL |= (RCU_PLL_PLLPEN | RCU_PLL_PLLQEN | RCU_PLL_PLLREN);

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLLP != (RCU_CFG0 & RCU_CFG0_SCSS)) {
    }
}
