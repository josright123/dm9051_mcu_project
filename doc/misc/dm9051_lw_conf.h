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

typedef enum { 
  CS_EACH = 0,
  CS_LONG,
} csmode_t;

uint8_t dm9051opts_iomode(void); // Driver's laydr2 control, for multi-spi-cards. depend on your definition of 'BOARD_SPI_COUNT'
csmode_t dm9051opts_longcsmode(void);
char *dm9051opts_desccsmode(void);


//#define dm9051opts_iomode()			dm9051opts[pin_code].iomode

//init
int dm9051_board_initialize(void);
void dm9051_board_irq_enable(void);
void dm9051_irqlines_proc(void);

int is_dm9051_board_irq(void);

void cpin_poweron_reset(void);
void cspi_read_regsS(uint8_t reg, u8 *buf, u16 len);
uint8_t cspi_read_reg(uint8_t reg);
void cspi_write_reg(uint8_t reg, uint8_t val);
uint8_t cspi_read_mem2x(void);
void cspi_read_mem(u8 *buf, u16 len);
void cspi_write_mem(u8 *buf, u16 len);
#if 1 //lw_config
#define DM9051_Poweron_Reset	cpin_poweron_reset
#define DM9051_Read_Reg			cspi_read_reg
#define DM9051_Write_Reg		cspi_write_reg
#define DM9051_Read_Mem2X		cspi_read_mem2x
#define DM9051_Read_Mem			cspi_read_mem
#define DM9051_Write_Mem		cspi_write_mem
#endif

/* dm9051 delay times procedures */
void dm_delay_us(uint32_t nus);
void dm_delay_ms(uint16_t nms);

int mstep_get_net_index(void);
void mstep_set_net_index(int i);
void mstep_next_net_index(void);

char *mstep_spi_conf_name(void);
char *mstep_conf_cpu_spi_ethernet(void);
char *mstep_conf_type(void);
int mstep_conf_spi_count(void);

int mstep_dm9051_index(void);

#endif //__DM9051_ENV_H
