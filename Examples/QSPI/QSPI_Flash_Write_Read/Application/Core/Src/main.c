/*!
    \file    main.c
    \brief   QSPI write and read spi flash

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

#if (defined(GD32G553))

#include <stdio.h>
#include "gd32g5x3.h"
#include "gd32g553q_eval.h"
#include "systick.h"

#define RESET_ENABLE_CMD                    (0x66)
#define RESET_CMD                           (0x99)
#define WRITE_ENABLE_CMD                    (0x06)
#define PAGE_PROG_CMD                       (0x02)
#define READ_CMD                            (0x03)
#define QUAD_FAST_READ_CMD                  (0xEB)
#define SECTOR_ERASE_CMD                    (0x20)
#define READ_STATUS_REG1_CMD                (0x05)
#define READ_STATUS_REG2_CMD                (0x35)
#define WRITE_VOLATILE_CFG_REG_CMD          (0x81)
#define DTR_READ_CMD                        (0xED)
#define ENABLE_QPI_CMD                      (0x38)
#define BUF_SIZE                            (countof(tx_buffer))
#define countof(a)                          (sizeof(a) / sizeof(*(a)))

uint8_t tx_buffer[] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F };
uint8_t rx_buffer[BUF_SIZE];
qspi_command_struct qspi_cmd;
qspi_polling_struct polling_cmd;

void rcu_config(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length);
void reset_buffer(uint8_t *pbuf, uint8_t length);
void qspi_flash_init(void);
void qspi_send_command(uint32_t instruction, uint32_t address, uint32_t dummy_cycles, uint32_t instruction_mode,
                       uint32_t address_mode, uint32_t address_size, uint32_t data_mode, uint32_t trans_rate, uint32_t trans_delay);
void qspi_reset_enable(void);
void qspi_reset(void);
void qspi_write_enable(void);
void qspi_polling_read_status_match_wel(void);
void qspi_polling_read_status_match_wip(void);
void qspi_flash_sector_erase(void);
void qspi_flash_program(void);
void qspi_flash_read(void);
void qspi_flash_read_quad_fast(void);
void qspi_flash_read_ddr(void);
void qspi_memory_map_read_ddr(void);

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
    /* configure COM */
    gd_eval_com_init(EVAL_COM);
    /* configure peripheral clock */
    rcu_config();
    printf("QSPI flash writing...\r\n");
    /* configure QSPI */
    qspi_flash_init();

    /* qspi flash reset enable */
    qspi_reset_enable();
    /* qspi flash reset */
    qspi_reset();
    delay_1ms(1);

    /* qspi flash sector erase */
    qspi_flash_sector_erase();

    /* qspi flash program */
    qspi_flash_program();

    printf("QSPI flash reading...\r\n");

    /* qspi flash read */
    qspi_flash_read();
    if(memory_compare(rx_buffer, tx_buffer, BUF_SIZE)) {
        printf("SPI FLASH WRITE AND 1-LINE READ TEST SUCCESS!\r\n");
    } else {
        printf("SPI FLASH WRITE AND 1-LINE READ TEST ERROR!\r\n");
    }
    /* qspi flash quad fast read */
    reset_buffer(rx_buffer, BUF_SIZE);
    qspi_flash_read_quad_fast();
    if(memory_compare(rx_buffer, tx_buffer, BUF_SIZE)) {
        printf("SPI FLASH WRITE AND 4-LINES READ TEST SUCCESS!\r\n");
    } else {
        printf("SPI FLASH WRITE AND 4-LINES READ TEST ERROR!\r\n");
    }
    /* qspi flash ddr read */
    reset_buffer(rx_buffer, BUF_SIZE);
    qspi_flash_read_ddr();
    if(memory_compare(rx_buffer, tx_buffer, BUF_SIZE)) {
        printf("SPI FLASH WRITE AND 4-LINES DDR MODE READ TEST SUCCESS!\r\n");
    } else {
        printf("SPI FLASH WRITE AND 4-LINES DDR MODE READ TEST ERROR!\r\n");
    }

    /* read data by memory map ddr mode */
    reset_buffer(rx_buffer, BUF_SIZE);
    qspi_memory_map_read_ddr();
    if(memory_compare(rx_buffer, tx_buffer, BUF_SIZE)) {
        printf("SPI FLASH WRITE AND 4-LINES DDR MEMORYMAP MODE READ TEST SUCCESS!\r\n");
    } else {
        printf("SPI FLASH WRITE AND 4-LINES DDR MEMORYMAP MODE READ TEST ERROR!\r\n");
    }
    while(1) {
    }
}

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_QSPI);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}

/*!
    \brief      reset buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void reset_buffer(uint8_t *pbuf, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++){
        *pbuf++ = 0;
    }
}

/*!
    \brief      configure the QSPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_init(void)
{
    qspi_init_struct qspi_init_para;

    /* initialize the QSPI command parameter structure */
    qspi_cmd_struct_para_init(&qspi_cmd);
    /* initialize the QSPI read polling parameter structure */
    qspi_polling_struct_para_init(&polling_cmd);

    /* configure QSPI GPIO: DQS/PB7, SCK/PD2, NSS/PD3, IO0/PF8, IO1/PD5, IO2/PD6, IO3/PD7 
    It is strongly recommended to use pins in groups to ensure the maximum communication clock frequency supported by QSPI.
    For details, please refer to the user manual.
    In this example, GPIO configuration is adapted for the test development board.*/
    gpio_af_set(GPIOD, GPIO_AF_10, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ,
                            GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_7);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_7);

    gpio_af_set(GPIOF, GPIO_AF_10, GPIO_PIN_8);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_8);

    /* configure reset GPIO and pull high RESET pin */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_6);
    gpio_bit_reset(GPIOB, GPIO_PIN_6);
    delay_1ms(1);
    gpio_bit_set(GPIOB, GPIO_PIN_6);

    /* initialize the init parameter structure */
    qspi_struct_para_init(&qspi_init_para);
    qspi_init_para.clock_mode     = QSPI_CLOCK_MODE_LOW;
    qspi_init_para.fifo_threshold = 4;
    qspi_init_para.sample_shift1  = QSPI_SAMPLE_SHIFTING_NONE;
    qspi_init_para.cs_high_time   = QSPI_CS_HIGH_TIME_2_CYCLE;
    qspi_init_para.flash_size     = 27;
    qspi_init_para.prescaler      = 8;
    qspi_init(&qspi_init_para);
    /* enable QSPI */
    qspi_enable();
}

/*!
    \brief      send QSPI command
    \param[in]  instruction: QSPI instruction, reference flash commands description
    \param[in]  address: access address, 0-flash size
    \param[in]  dummy_cycles: dummy cycles, 0 - 31
    \param[in]  instruction_mode: instruction mode
      \arg        QSPI_INSTRUCTION_NONE
      \arg        QSPI_INSTRUCTION_1_LINE
      \arg        QSPI_INSTRUCTION_2_LINES
      \arg        QSPI_INSTRUCTION_4_LINES
    \param[in]  address_mode: address mode
      \arg        QSPI_ADDR_NONE
      \arg        QSPI_ADDR_1_LINE
      \arg        QSPI_ADDR_2_LINES
      \arg        QSPI_ADDR_4_LINES
    \param[in]  address_size: address size
      \arg        QSPI_ADDR_8_BITS
      \arg        QSPI_ADDR_16_BITS
      \arg        QSPI_ADDR_24_BITS
      \arg        QSPI_ADDR_32_BITS
    \param[in]  data_mode: data mode
      \arg        QSPI_DATA_NONE
      \arg        QSPI_DATA_1_LINE
      \arg        QSPI_DATA_2_LINES
      \arg        QSPI_DATA_4_LINES
    \param[out] none
    \retval     none
*/
void qspi_send_command(uint32_t instruction, uint32_t address, uint32_t dummy_cycles, uint32_t instruction_mode,
                       uint32_t address_mode, uint32_t address_size, uint32_t data_mode, uint32_t trans_rate, uint32_t trans_delay)
{
    qspi_cmd.instruction      = instruction;
    qspi_cmd.instruction_mode = instruction_mode;
    qspi_cmd.addr             = address;
    qspi_cmd.addr_mode        = address_mode;
    qspi_cmd.addr_size        = address_size;
    qspi_cmd.altebytes        = 0;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = data_mode;
    qspi_cmd.data_length      = 1;
    qspi_cmd.dummycycles      = dummy_cycles;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.trans_rate       = trans_rate;
    qspi_cmd.trans_delay      = trans_delay;
    qspi_command_config(&qspi_cmd);
}

/*!
    \brief      QSPI reset enable
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_reset_enable(void)
{
    /* write enable */
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction      = RESET_ENABLE_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command_config(&qspi_cmd);
}
/*!
    \brief      QSPI reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_reset(void)
{
    /* reset */
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction      = RESET_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command_config(&qspi_cmd);
}

/*!
    \brief      QSPI write
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_write_enable(void)
{
    /* write enable */
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction      = WRITE_ENABLE_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command_config(&qspi_cmd);
}

/*!
    \brief      poll the status of the write enable latch(wel) flag in the flash's status register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_polling_read_status_match_wel(void)
{
    polling_cmd.match            = 0x02;
    polling_cmd.mask             = 0x02;
    polling_cmd.match_mode       = QSPI_MATCH_MODE_AND;
    polling_cmd.statusbytes_size = 1;
    polling_cmd.interval         = 0x10;
    polling_cmd.polling_stop     = QSPI_POLLING_STOP_ENABLE;
    qspi_cmd.instruction         = READ_STATUS_REG1_CMD;
    qspi_cmd.data_mode           = QSPI_DATA_1_LINE;

    qspi_polling_config(&qspi_cmd, &polling_cmd);

    /* wait for the BUSY flag to be reset */
    while(RESET != qspi_flag_get(QSPI_FLAG_BUSY)) {
    }
    /* clear the RPMF flag */
    qspi_flag_clear(QSPI_FLAG_RPMF);
}

/*!
    \brief      poll the status of the write in progress(wip) flag in the flash's status register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_polling_read_status_match_wip(void)
{
    qspi_cmd.instruction         = READ_STATUS_REG1_CMD;
    qspi_cmd.addr_mode           = QSPI_ADDR_NONE;
    qspi_cmd.data_mode           = QSPI_DATA_1_LINE;
    polling_cmd.match            = 0x00;
    polling_cmd.mask             = 0x01;
    polling_cmd.match_mode       = QSPI_MATCH_MODE_AND;
    polling_cmd.statusbytes_size = 1;
    polling_cmd.interval         = 0x10;
    polling_cmd.polling_stop     = QSPI_POLLING_STOP_ENABLE;

    qspi_polling_config(&qspi_cmd, &polling_cmd);

    /* wait for the BUSY flag to be reset */
    while(RESET != qspi_flag_get(QSPI_FLAG_BUSY)) {
    }
    /* clear the RPMF flag */
    qspi_flag_clear(QSPI_FLAG_RPMF);
}

/*!
    \brief      qspi flash sector erase
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_sector_erase(void)
{
    /* QSPI write enable */
    qspi_write_enable();
    /* configure read polling mode to wait for write enabling */
    qspi_polling_read_status_match_wel();
    /* erase sector 0-0x1000 */
    qspi_send_command(SECTOR_ERASE_CMD, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDR_1_LINE, QSPI_ADDR_24_BITS, QSPI_DATA_NONE,
                      0, 0);
    qspi_polling_read_status_match_wip();
}

/*!
    \brief      qspi flash program
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_program(void)
{
    /* QSPI write enable */
    qspi_write_enable();
    /* configure read polling mode to wait for write enabling */
    qspi_polling_read_status_match_wel();
    qspi_send_command(PAGE_PROG_CMD, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDR_1_LINE, QSPI_ADDR_24_BITS, QSPI_DATA_1_LINE,
                      0, 0);
    qspi_data_length_config(BUF_SIZE);
    qspi_data_transmit(tx_buffer);
    /* wait for the data transmit completed */
    while(RESET == qspi_flag_get(QSPI_FLAG_TC)) {
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);
    qspi_polling_read_status_match_wip();
}

/*!
    \brief      QSPI read flash using 1-line mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_read(void)
{
    /* 1 line read without DQS */
    qspi_cmd.instruction      = READ_CMD;
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.addr             = 0;
    qspi_cmd.addr_mode        = QSPI_ADDR_1_LINE;
    qspi_cmd.addr_size        = QSPI_ADDR_24_BITS;
    qspi_cmd.altebytes        = 0;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = QSPI_DATA_1_LINE;
    qspi_cmd.data_length      = 1;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command_config(&qspi_cmd);
    qspi_data_length_config(BUF_SIZE);

    qspi_data_receive(rx_buffer);
    /* wait for the data receive completed */
    while(RESET == qspi_flag_get(QSPI_FLAG_TC)) {
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);
    /* wait for the BUSY flag to be reset */
    while(SET == qspi_flag_get(QSPI_FLAG_BUSY)) {
    }
}

/*!
    \brief      QSPI read flash using 4-lines mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_read_quad_fast(void)
{
    qspi_cmd.instruction      = QUAD_FAST_READ_CMD;
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.addr             = 0;
    qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
    qspi_cmd.addr_size        = QSPI_ADDR_24_BITS;
    qspi_cmd.altebytes        = 0xFE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_4_LINES;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    qspi_cmd.data_length      = 1;
    qspi_cmd.dummycycles      = 14;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.trans_rate       = 0;
    qspi_cmd.trans_delay      = 0;
    qspi_command_config(&qspi_cmd);
    qspi_data_length_config(BUF_SIZE);

    qspi_data_receive(rx_buffer);

    /* wait for the data receive completed */
    while(RESET == qspi_flag_get(QSPI_FLAG_TC)) {
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);
    /* wait for the BUSY flag to be reset */
    while(SET == qspi_flag_get(QSPI_FLAG_BUSY)) {
    }
}

/*!
    \brief      QSPI read flash using 4-lines mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_read_ddr(void)
{
    uint8_t volatile_reg_data[] = {0xE7};

    /* enter QPI mode */
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction      = ENABLE_QPI_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command_config(&qspi_cmd);
    /* wait for the BUSY flag to be reset */
    while(SET == qspi_flag_get(QSPI_FLAG_BUSY)) {
    }

    /* configure flash in Quad DTR with DQS */
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
    qspi_cmd.instruction      = WRITE_VOLATILE_CFG_REG_CMD;
    qspi_cmd.addr             = 0;
    qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
    qspi_cmd.addr_size        = QSPI_ADDR_24_BITS;
    qspi_cmd.altebytes        = 0;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    qspi_cmd.data_length      = 1;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command_config(&qspi_cmd);

    qspi_data_length_config(1);
    qspi_data_transmit(volatile_reg_data);
    /* wait for the BUSY flag to be reset */
    while(SET == qspi_flag_get(QSPI_FLAG_BUSY)) {
    }

    qspi_cmd.instruction      = DTR_READ_CMD;
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
    qspi_cmd.addr             = 0;
    qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
    qspi_cmd.addr_size        = QSPI_ADDR_24_BITS;
    qspi_cmd.altebytes        = 0xFE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_4_LINES;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    qspi_cmd.dummycycles      = 15;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.trans_rate       = QSPI_DDR_MODE;
    qspi_cmd.trans_delay      = 0;
    qspi_command_config(&qspi_cmd);
    qspi_data_length_config(BUF_SIZE);

    qspi_data_receive(rx_buffer);
    /* wait for the data receive completed */
    while(RESET == qspi_flag_get(QSPI_FLAG_TC)) {
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);
}

/*!
    \brief      read spi flash by memory map mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_memory_map_read_ddr(void)
{
    qspi_cmd.instruction      = DTR_READ_CMD;
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
    qspi_cmd.addr             = 0x0;
    qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
    qspi_cmd.addr_size        = QSPI_ADDR_24_BITS;
    qspi_cmd.altebytes        = 0xFE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_4_LINES;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    qspi_cmd.dummycycles      = 15;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.trans_rate       = QSPI_DDR_MODE;
    qspi_cmd.trans_delay      = 0;

    qspi_memorymapped_config(&qspi_cmd, 0, QSPI_TMOUT_DISABLE);

    for(uint32_t i = 0; i < BUF_SIZE; i++) {
        rx_buffer[i] = *(uint8_t *)(0x90000000 + i);
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
    
}

#endif /* (defined(GD32G553)) */
