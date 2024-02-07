/**
  **************************************************************************
  * @file     dm9051_env.c ,dm9051_config.c ,at32_emac.c
  * @version  v1.0
  * @date     2023-04-28, 2023-10-17, 2023-10-24
  * @brief    dm9051 config program
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
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "dm9051_env.h" //("dm9051f_netconf.h")(contain "dm9051_lw.h")
#include "developer_conf.h" //#include "main.h" //#include "developer_conf.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/ip4_frag.h"
#include "lwip/etharp.h"
#include "lwip/igmp.h"
#include <string.h>

static int pinCode; // = 0 ~ (ETH_COUNT - 1)
static int total_eth_count = 0;

/*
#if 1
#define spi_type_no(i)				sconf[i].bus.spi.spi_num
#define gpio_cs(i)					sconf[i].bus.cs

spi_type *spi_no(void) // offer to "dm9051_at32_transfer.c"
{
	return spi_type_no(pinCode);
}
gpio_type *gpio_gpport(void) // offer to "dm9051_at32_transfer.c"
{
	return gpio_cs(pinCode).gpport;
}
uint16_t gpio_pin(void) // offer to "dm9051_at32_transfer.c"
{
	return gpio_cs(pinCode).pin;
}
#endif

void set_pin_code(int i) {
	pinCode = i;
}
void next_pin_code(void)
{
}

void on_pin_code(char *str, uint16_t id) {
	printf("  %s, PinCode %d ...%s... Read Chip ID: %02x\r\n", get_spi_name(pinCode), pinCode, str, id);
}

#define spi_name(i)					sconf[i].spi_name

char *get_spi_name(int i) { //extern
	return spi_name(i);
}*/

//#define crm_spi_clk(i)			sconf[i].bus.spi.crm_clk

#define crm_equ(i, j) \
	(sconf[pinCode].bus.wire[i].crm_clk == sconf[pinCode].bus.wire[j].crm_clk)

#define crm_gp_clk(i) \
	sconf[pinCode].bus.wire[i].crm_clk

#define gp_equal(is, im) \
	(sconf[pinCode].bus.wire[is].gpport == sconf[pinCode].bus.wire[im].gpport)
#define gp_port(i) \
	sconf[pinCode].bus.wire[i].gpport
#define gp_pin(i) \
	sconf[pinCode].bus.wire[i].pin

#define gpio_spi_wire_mux(iwr) \
	if (sconf[pinCode].bus.wire[iwr].mux != NUL_MUXSEL) \
		gpio_pin_mux_config(sconf[pinCode].bus.wire[iwr].gpport, sconf[pinCode].bus.wire[iwr].pinsrc, \
							sconf[pinCode].bus.wire[iwr].mux)

/**
  * @brief  pd.01 gpio configuration. GPIOD, PD0
  */
	
//void gpio_cs_pin_config(void)
//{
//  gpio_init_type gpio_init_struct;
//  crm_periph_clock_enable(crm_cs_clk(), TRUE); /* enable the gpioa clock */

//  gpio_default_para_init(&gpio_init_struct);
//  gpio_init_struct.gpio_out_type  		= GPIO_OUTPUT_PUSH_PULL;
//  gpio_init_struct.gpio_drive_strength	= GPIO_DRIVE_STRENGTH_STRONGER;
//  gpio_init_struct.gpio_mode			= GPIO_MODE_OUTPUT;

  /* spi_cs */
//  gpio_init_struct.gpio_pull			= GPIO_PULL_NONE;
//  gpio_init_struct.gpio_pins			= cs_pin(); //sconf[pinCode].bus.cs.pin; // GPIO_PINS_12/GPIO_PINS_0
//  gpio_init(cs_port(), &gpio_init_struct); // GPIOB/GPIOD
//}

//void exint_scfg_config(void)=
  //gpio_intr_pin_config();
  //_exint_pin_config(EXINT_TRIGGER_FALLING_EDGE, NVIC_PRIORITY_GROUP_0); //(EXINT_TRIGGER_FALLING_EDGE);

//.static void dm9051_pins_configuration(void){
//.}



/**
  * @brief  enable /emac clock and /gpio clock
  * @retval success or error
  *
  * Called by main() / "main.c"
  *
  */
//error_status dm9051_env_init(void)
//{
  //.dm9051_pins_configuration_all();
  
  //#if VER_ENV_TEST
  //dm9051_init_dual(lwip_get_mac_addresse1());
  //#endif
  //return SUCCESS;
//}
