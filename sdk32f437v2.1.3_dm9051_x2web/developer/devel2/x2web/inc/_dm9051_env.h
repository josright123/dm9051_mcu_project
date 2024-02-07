/**
  **************************************************************************
  * @file     dm9051_env.h
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

#ifdef __cplusplus
 extern "C" {
#endif

#include "at32f435_437.h" //"at32f413.h" //#include "at32f415.h"
#include "netif.h"
#include "dm9051_lw.h"
	 
#include "dm9051_at32_pins.h"

//#define VER_ENV_TEST							0 //1

#define ETH_COUNT								4 //3 //2
#define BOARD_SPI_COUNT							4 //ETH_COUNT // (ETH_COUNT - 1)

const static spi_interface_t sconf[BOARD_SPI_COUNT] = {
#if BOARD_SPI_COUNT >= 1
	{
		"SPI1",
		{
		 {SPI1, CRM_SPI1_PERIPH_CLOCK},
		 {
		  {GPIOA, GPIO_PINS_5,  CRM_GPIOA_PERIPH_CLOCK, GPIO_PINS_SOURCE5,  GPIO_MUX_5},
		  {GPIOA, GPIO_PINS_6,  CRM_GPIOA_PERIPH_CLOCK, GPIO_PINS_SOURCE6,  GPIO_MUX_5},	//IMISO
		  {GPIOA, GPIO_PINS_7,  CRM_GPIOA_PERIPH_CLOCK, GPIO_PINS_SOURCE7,  GPIO_MUX_5},	//IMOSI
		 },
	     {GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK, NUL_PINSRC, NUL_MUXSEL}, //(PA15) api1's cs
		 
		 {0, 0,  NULL_CRMCLK, NUL_PINSRC, NUL_MUXSEL}, //(NULL) INT-pin
		 //{GPIOC, GPIO_PINS_7,  CRM_GPIOC_PERIPH_CLOCK, NUL_PINSRC, NUL_MUXSEL}, //(PC7) INT-pin
		 
		 {0,     0,            NULL_CRMCLK,            NUL_PINSRC, NUL_MUXSEL}, //(NULL) RST-pin
		},
	},
#endif
#if BOARD_SPI_COUNT >= 2
	{
		"SPI2",
		{
		 {SPI2, CRM_SPI2_PERIPH_CLOCK},
		 {
		  {GPIOD, GPIO_PINS_1, CRM_GPIOD_PERIPH_CLOCK, GPIO_PINS_SOURCE1, GPIO_MUX_6},
		  {GPIOC, GPIO_PINS_2, CRM_GPIOC_PERIPH_CLOCK, GPIO_PINS_SOURCE2, GPIO_MUX_5},	//IMISO
		  {GPIOD, GPIO_PINS_4, CRM_GPIOD_PERIPH_CLOCK, GPIO_PINS_SOURCE4, GPIO_MUX_6},	//IMOSI
	     },
		 {GPIOD, GPIO_PINS_0, CRM_GPIOD_PERIPH_CLOCK, NUL_PINSRC, NUL_MUXSEL}, //(PD0) spi2's cs
		 
		 {GPIOC, GPIO_PINS_7,  CRM_GPIOC_PERIPH_CLOCK, NUL_PINSRC, NUL_MUXSEL}, //(PC7) INT-pin
		 //{0, 0,  NULL_CRMCLK, NUL_PINSRC, NUL_MUXSEL}, //(NULL) INT-pin
		 
		 {0, 0,  NULL_CRMCLK, NUL_PINSRC, NUL_MUXSEL}, //(NULL) RST-pin
		},
		//"SPI2",
	    //{
	  	 //{SPI2, CRM_SPI2_PERIPH_CLOCK},
	  	 //{
	  	 // {GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK},
	  	 // {GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK},	//IMISO
	  	 // {GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK},	//IMOSI
	  	 //},
	  	 //{GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK},
	    //},
	},
#endif
#if BOARD_SPI_COUNT >= 3
	{
		"SPI3",
		{
		 {SPI3, CRM_SPI3_PERIPH_CLOCK},
		 {
		  {GPIOC, GPIO_PINS_10, CRM_GPIOC_PERIPH_CLOCK, GPIO_PINS_SOURCE10, GPIO_MUX_6},
		  {GPIOC, GPIO_PINS_11, CRM_GPIOC_PERIPH_CLOCK, GPIO_PINS_SOURCE11, GPIO_MUX_6},	//IMISO
		  {GPIOC, GPIO_PINS_12, CRM_GPIOC_PERIPH_CLOCK, GPIO_PINS_SOURCE12, GPIO_MUX_6},	//IMOSI
	     },
	     {GPIOB, GPIO_PINS_12,  CRM_GPIOB_PERIPH_CLOCK, NUL_PINSRC, NUL_MUXSEL}, //(PB12) Test-ISP4 OK
		 {0, 0,  NULL_CRMCLK, NUL_PINSRC, NUL_MUXSEL}, //(NULL) INT-pin
		 {0, 0,  NULL_CRMCLK, NUL_PINSRC, NUL_MUXSEL}, //(NULL) RST-pin
		},
	},
#endif
#if BOARD_SPI_COUNT >= 4
	{ 
		"SPI4",
		{
		 {SPI4, CRM_SPI4_PERIPH_CLOCK},
		 {
			 {GPIOE, GPIO_PINS_2,   CRM_GPIOE_PERIPH_CLOCK, GPIO_PINS_SOURCE2,   GPIO_MUX_5},
			 {GPIOE, GPIO_PINS_5,   CRM_GPIOE_PERIPH_CLOCK, GPIO_PINS_SOURCE5,   GPIO_MUX_5},	//IMISO
			 {GPIOE, GPIO_PINS_6,   CRM_GPIOE_PERIPH_CLOCK, GPIO_PINS_SOURCE6,   GPIO_MUX_5},	//IMOSI

		  //{GPIOB, GPIO_PINS_13,  CRM_GPIOB_PERIPH_CLOCK, GPIO_PINS_SOURCE13,  GPIO_MUX_6},
		  //{GPIOE, GPIO_PINS_2,   CRM_GPIOE_PERIPH_CLOCK, GPIO_PINS_SOURCE2,   GPIO_MUX_5},
		  
		  //{GPIOB, GPIO_PINS_8,   CRM_GPIOB_PERIPH_CLOCK, GPIO_PINS_SOURCE8,   GPIO_MUX_6},	//IMISO
		  //{GPIOC, GPIO_PINS_0,   CRM_GPIOC_PERIPH_CLOCK, GPIO_PINS_SOURCE0,   GPIO_MUX_6},
		  
		  //{GPIOA, GPIO_PINS_1,   CRM_GPIOA_PERIPH_CLOCK, GPIO_PINS_SOURCE1,   GPIO_MUX_5},	//IMOSI
		 },
		 {GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK, NUL_PINSRC, NUL_MUXSEL}, //(PA4) spi3's cs
		 {0, 0,  NULL_CRMCLK, NUL_PINSRC, NUL_MUXSEL}, //(NULL) INT-pin
		 {0, 0,  NULL_CRMCLK, NUL_PINSRC, NUL_MUXSEL}, //(NULL) RST-pin
		},
	},
#endif
};

const static struct exint_st exintd = {
	{ CRM_GPIOC_PERIPH_CLOCK, EXINT_LINE_7, EXINT9_5_IRQn}, /* PC7 */
	{ CRM_SCFG_PERIPH_CLOCK, SCFG_PORT_SOURCE_GPIOC, SCFG_PINS_SOURCE7},
};

//.
//.const static gpio_t int_pin[] = {
//.	GPIOC, GPIO_PINS_7,  CRM_GPIOC_PERIPH_CLOCK, NUL_PINSRC, NUL_MUXSEL, //(PC7)INT-pin
//.	GPIOA, GPIO_PINS_0,  CRM_GPIOA_PERIPH_CLOCK, NUL_PINSRC, NUL_MUXSEL, //(PC7)INT-pin
//.};
//.
//.const ststic struct exint_st exint_cfg[] = {
//.	{CRM_GPIOC_PERIPH_CLOCK, EXINT_LINE_7, EXINT9_5_IRQn}, // PC7
//	{CRM_SCFG_PERIPH_CLOCK, SCFG_PORT_SOURCE_GPIOC, SCFG_PINS_SOURCE7},
//.	{CRM_GPIOA_PERIPH_CLOCK, EXINT_LINE_0, EXINT0_IRQn}, // PA0
//  {CRM_SCFG_PERIPH_CLOCK, SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0},
//.};
//.

/** @addtogroup 415_dm9051_env
  * @{
  */
	 

char *get_spi_name(int i);
void gpio_spi_clk_mux(gpio_mux_sel_type gpio_mux); //ISCK
void gpio_spi_mi_mux(gpio_mux_sel_type gpio_mux); //IMISO
void gpio_spi_mo_mux(gpio_mux_sel_type gpio_mux); //IMOSI

/*
 * A function _dm9051_env_init(void)
 */
//error_status dm9051_env_init(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif //__DM9051_ENV_H
