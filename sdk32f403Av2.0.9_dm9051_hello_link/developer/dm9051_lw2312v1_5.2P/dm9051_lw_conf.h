/**
 **************************************************************************
 * @file     dm9051_lw_conf.h
 * @version  v1.0
 * @date     2023-04-28
 * @brief    header file of dm9051 environment config program.
 **************************************************************************
 *                       Copyright notice & Disclaimer
 *
 * The software Board Support Package (BSP) that is made available to
 * download from Artery official website is the copyrighted work of Artery.
 * Artery authorizes customers to use, copy, and distribute the BSP
 * software and its related documentation for the purpose of design and
 * development in conjunction with Artery microcontrollers. Use of the
 * software is governed by this copyright notice and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
 * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
 * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
 * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
 * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 *
 **************************************************************************
 */
#ifndef __DM9051_ENV_H
#define __DM9051_ENV_H

#include "dm9051opts.h" //driver attributes

// init
void dm9051_board_initialize(void);
void dm9051_board_irq_enable(void);
void dm9051_irqlines_proc(void);

void cpin_poweron_reset(void);
uint8_t cspi_read_reg(uint8_t reg);
void cspi_read_regnx(uint8_t reg, uint8_t length, uint8_t *buf);
void cspi_write_reg(uint8_t reg, uint8_t val);
uint8_t cspi_read_mem2x(void);
void cspi_read_mem(u8 *buf, u16 len);
void cspi_write_mem(u8 *buf, u16 len);
#if 1 // lw_config
#define DM9051_Poweron_Reset cpin_poweron_reset
#define DM9051_Read_Reg cspi_read_reg
#define DM9051_Read_Regnx cspi_read_regnx
#define DM9051_Write_Reg cspi_write_reg
#define DM9051_Read_Mem2X cspi_read_mem2x
#define DM9051_Read_Mem cspi_read_mem
#define DM9051_Write_Mem cspi_write_mem
#endif

/* dm9051 delay times procedures */
void dm_delay_us(uint32_t nus);
void dm_delay_ms(uint16_t nms);

void mstep_set_net_index(int i);
void mstep_next_net_index(void);

char *mstep_spi_conf_name(void);
char *mstep_conf_cpu_spi_ethernet(void);
char *mstep_conf_type(void);
int mstep_conf_spi_count(void);

#endif //__DM9051_ENV_H
