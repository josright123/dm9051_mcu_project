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
#include "dm9051spiopts.h"
#include "dm9051_lw.h"

//tobe dm9051opts.c
//
// [This _HELLO_DRIVER_INTERNAL compiler option, is for a diagnostic purpose while the program code is to use this dm9051_lw driver.]
// [Must be 1, for dm9051_lw driver operating.]
// [Set to 0, for the program code to observ the using APIs of dm9051_lw driver, before add the dm9051_lw driver implement files.]
//
#if HELLO_DRIVER_INTERNAL //To support for being called by the program code from outside this dm9051_lw driver.

//void set_dm9051opts_testplanlog(enable_t test_log);
//enable_t get_dm9051opts_testplanlog(void);

//uint8_t dm9051opts_iomode(void); // Driver's laydr2 control, for multi-spi-cards. depend on your definition of '_BOARD_SPI_COUNT'
//char *dm9051opts_desciomode(void);

//csmode_t dm9051opts_csmode(void);
//char *dm9051opts_desccsmode(void);
//ncrmode_t dm9051opts_ncrforcemode(void);
//char *dm9051opts_descncrmode(void);

//uint8_t dm9051opts_promismode(void);
//char *dm9051opts_descpromismode(void);

//-

//#define OPTS_FUNC_EXT
//#undef OPTS_FUNC_EXT
	#if 0
	/*#define DM_FUNC(rtype, field) \
		dm9051opts_##rtype##field()

	#define DM_MACRO(rtype, field) \
		rtype dm9051opts_##rtype##field(void); \
		char *dm9051opts_desc##field(void);*/
	#endif

#define DM_TYPE		0
#include "dm_types.h"

/*
 * Below access to OPTs data:
 */
//#define IS_GET_INSTEAD(rtype, field)	dm9051opts_get##rtype##field /* definition for extern short-call-name */
//#define IS_SET_INSTEAD(rtype, field)	dm9051opts_set##rtype##field
//#define get_testplaninclude			IS_GET_INSTEAD(enable_t, test_plan_include) //only get is documented export!
//#define set_testplanlog				IS_SET_INSTEAD(enable_t, test_plan_log)
//#define get_testplanlog				IS_GET_INSTEAD(enable_t, test_plan_log)=
#define get_testplanlog(name)			DM_FUNC(enable_t, name)
#define OPT_U8(name)					DM_FUNC(uint8_t, name) //appcall
#define OPT_CS(name)					DM_FUNC(csmode_t, name) //appcall
#define OPT_NCR(name)					DM_FUNC(ncrmode_t, name) //appcall
#define OPT_CONFIRM(name)				DM_FUNC(enable_t, name) //appcall (define isonlybytemode(), or OPT_CONFIRM(onlybytemode))

//#define u8iomode					OPTS_DATA(uint8_t, iomode)
//#define u8promismode				OPTS_DATA(uint8_t, promismode)
//#define enum_csmode				OPTS_DATA(csmode_t, csmode) //dm9051opts_csmode_tcsmode
//#define enum_ncrmode				OPTS_DATA(ncrmode_t, ncrmode)
//#define isrxtype_test				!OPTS_DATA(enable_t, rxtypemode)
//#define isrxtype_run				OPTS_DATA(enable_t, rxtypemode)
//#define isflowcontrolmode			OPTS_DATA(enable_t, flowcontrolmode)
//#define isrxmode_checksum_offload	OPTS_DATA(enable_t, rxmode_checksum_offload)

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
void dm9051_boards_initialize(int n);
//void ethernet_interfaces_initialize(void);
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
const uint8_t *mstep_eth_mac(void);
const uint8_t *mstep_eth_ip(void);
const uint8_t *mstep_eth_gw(void);
const uint8_t *mstep_eth_mask(void);

int mstep_dm9051_index(void);

#endif //__DM9051_ENV_H
