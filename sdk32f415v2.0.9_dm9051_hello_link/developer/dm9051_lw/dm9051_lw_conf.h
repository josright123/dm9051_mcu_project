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

#include "dm9051_lw.h"
#include "dm9051opts.h" //driver attributes
#include "dm9051spiopts.h"

//tobe dm9051opts.c
//
// [This HELLO_DRIVER_INTERNAL compiler option, is for a diagnostic purpose while the program code is to use this dm9051_lw driver.]
// [Must be 1, for dm9051_lw driver operating.]
// [Set to 0, for the program code to observ the using APIs of dm9051_lw driver, before add the dm9051_lw driver implement files.]
//
#if HELLO_DRIVER_INTERNAL //To support for being called by the program code from outside this dm9051_lw driver.

typedef enum { 
  CS_EACH = 0,
  CS_LONG,
} csmode_t;

typedef enum { 
  NCR_RST_DEFAULT = 0,
  NCR_FORCE_100MF,
  NCR_AUTO_NEG,
} ncrmode_t;

//void set_dm9051opts_testplanlog(confirm_state test_log);
//confirm_state get_dm9051opts_testplanlog(void);

//uint8_t dm9051opts_iomode(void); // Driver's laydr2 control, for multi-spi-cards. depend on your definition of '_BOARD_SPI_COUNT'
//char *dm9051opts_desciomode(void);

//csmode_t dm9051opts_csmode(void);
//char *dm9051opts_desccsmode(void);
//ncrmode_t dm9051opts_ncrforcemode(void);
//char *dm9051opts_descncrmode(void);

//uint8_t dm9051opts_promismode(void);
//char *dm9051opts_descpromismode(void);

//-
//confirm_state dm9051opts_confirm_staterxmode_checksum_offload(void); //~is_dm9051opts_rxchecksum_offload(void);
//char *dm9051opts_descrxmode_checksum_offload(void); //~dm9051opts_desc_rxchecksum_offload(void);
//confirm_state dm9051opts_confirm_stateflowcontrolmode(void);
//char *dm9051opts_descflowcontrolmode(void);

SG_FUNCTION(confirm_state, test_plan_include);
SG_FUNCTION(confirm_state, test_plan_log);

IS_FUNCTION(uint8_t, iomode);
IS_FUNCTION(uint8_t, promismode);

IS_FUNCTION(csmode_t, csmode);
IS_FUNCTION(ncrmode_t, ncrmode);

IS_FUNCTION(confirm_state, flowcontrolmode); //confirm_state
IS_FUNCTION(confirm_state, rxmode_checksum_offload); //confirm_state

#define get_testplaninclude			IS_GET_INSTEAD(confirm_state, test_plan_include) //only get is documented export!
#define get_testplanlog				IS_GET_INSTEAD(confirm_state, test_plan_log)
#define set_testplanlog				IS_SET_INSTEAD(confirm_state, test_plan_log)

#define iomode						IS_INSTEAD(uint8_t, iomode)
#define promismode					IS_INSTEAD(uint8_t, promismode)
#define csmode						IS_INSTEAD(csmode_t, csmode) //dm9051opts_csmode_tcsmode
#define ncrmode						IS_INSTEAD(ncrmode_t, ncrmode)
#define isflowcontrolmode			IS_INSTEAD(confirm_state, flowcontrolmode)
#define isrxmode_checksum_offload	IS_INSTEAD(confirm_state, rxmode_checksum_offload)
//#define INSTEAD_IS_FUNCTION(isfunc, rtype, field) \
//	#define isfunc dm9051opts_##rtype##field
//INSTEAD_IS_FUNCTION(isflowcontrolmode, confirm_state, flowcontrolmode)
//INSTEAD_IS_FUNCTION(isrxmode_checksum_offload, confirm_state, rxmode_checksum_offload)
//#define isflowcontrolmode			dm9051opts_confirm_stateflowcontrolmode
//#define isrxmode_checksum_offload	dm9051opts_confirm_staterxmode_checksum_offload

//#define dm9051opts_iomode()			dm9051opts[pin_code].iomode

//int is_dm9051_board_irq(void);

void cpin_poweron_reset(void);
//void cspi_read_regsS(uint8_t reg, u8 *buf, u16 len);
void cspi_read_regs(uint8_t reg, u8 *buf, u16 len, csmode_t csmode);
uint8_t cspi_read_reg(uint8_t reg);
void cspi_write_reg(uint8_t reg, uint8_t val);
uint8_t cspi_read_mem2x(void);
void cspi_read_mem(u8 *buf, u16 len);
void cspi_write_mem(u8 *buf, u16 len);

#endif //HELLO_DRIVER_INTERNAL

//init
#if HELLO_DRIVER_API
int dm9051_board_initialize(void);
#endif
void exint_menable(nvic_priority_group_type priority); //void dm9051_board_irq_enable(void);
void dm9051_irqlines_proc(void);

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

#if HELLO_DRIVER_API
void mstep_set_net_index(int i);
void mstep_next_net_index(void);
char *mstep_spi_conf_name(void);
char *mstep_conf_cpu_spi_ethernet(void);
#endif
int mstep_get_net_index(void);

char *mstep_conf_info(void);
char *mstep_conf_cpu_cs_ethernet(void);
char *mstep_conf_type(void);
//int mstep_conf_spi_count(void);

int mstep_dm9051_index(void);

#endif //__DM9051_ENV_H
