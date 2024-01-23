/**
  **************************************************************************
  * @file     dm9051_at32_mf.h
  * @version  v1.0
  * @date     2023-11-05
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
#ifndef __DM9051_MF_H
#define __DM9051_MF_H

/** @addtogroup cm4_dm9051_at32
  * @{
  */

int pinCode;
#define mstep_set_net_index(i)		pinCode = i
//#if DM9051_AT32_MF
//#endif
//extern int pinCode; //static // = 0 ~ (ETH_COUNT - 1)

#define spi_dev()					&devconf[pinCode]

//#define n_spi_name(cnf, i)		cnf[i].spi_name
//#define n_spi_port(cnf, i)		cnf[i].spi_num

#define spi_conf_name()				devconf[pinCode].spi_name
#define spi_conf_port()				devconf[pinCode].spi_num

//#define spi_name(i)				sconf[i].spi_name
//#define spi_port(i)				sconf[i].bus.spi //spi_type_no(i) sconf[i].bus.spi.spi_num
//#define gpio_cs_base(i)			&sconf[i].cs

#define gpio_wire_sck()				devconf[pinCode].wire_sck 
#define gpio_wire_mi()				devconf[pinCode].wire_mi
#define gpio_wire_mo()				devconf[pinCode].wire_mo
#define gpio_cs()					devconf[pinCode].cs

#define intr_conf_ptr()				((const gpio_set_t *)devconf[pinCode].intr_gpio_set)
#define rst_conf_ptr()				((const gpio_set_t *)devconf[pinCode].rst_gpio_set)
	
#define rst_gpio_ptr()				((const gpio_t *)&devconf[pinCode].rst_gpio_set->pin)
//#define intr_gpio_ptr()			((const gpio_t *)&devconf[pinCode].intr_gpio_set->pin)
#define gpio_set_field_loc(gs)		((const gpio_t *)&gs->pin) //generic.
	
#define exint_conf_ptr()			((const intr_set_t *)devconf[pinCode].intr_set) //&exintd

#define chipid_on_pin_code_log_s	printf("  %s, PinCode %d ...%s... Read Chip ID: %02x\r\n", spi_conf_name(), pinCode, str, id)
#define chipid_on_pin_code_log_err	printf("  %s, Read Chip ID error!\r\n", spi_conf_name())
#define add_net_configure_log_s		printf("ndev TEST (add_net_configure(void).s device %d)\r\n", pinCode)
#define add_net_configure_log_e		printf("ndev TEST (add_net_configure(void).e device %d)\r\n", pinCode)

const static gpio_set_t pc7_intr_gpio = {
	{GPIOC, GPIO_PINS_7,  CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_INPUT, NUL_PINSRC, NUL_MUXSEL}, //(PC7) INT-pin
};

const static intr_set_t pc7_intr = {
	#if scfg_exint_system_ready == 0
	 { CRM_GPIOC_PERIPH_CLOCK, EXINT_LINE_7, EXINT9_5_IRQn}, /* correspond to and with PC7 */
	 { CRM_IOMUX_PERIPH_CLOCK, GPIO_PORT_SOURCE_GPIOC, GPIO_PINS_SOURCE7}, //CRM_IOMUX_PERIPH_CLOCK is temp insread, as artery's  example.
	#else
	 { CRM_GPIOC_PERIPH_CLOCK, EXINT_LINE_7, EXINT9_5_IRQn}, /* correspond to and with PC7 */
	 { CRM_SCFG_PERIPH_CLOCK, SCFG_PORT_SOURCE_GPIOC, SCFG_PINS_SOURCE7},
	#endif
};

const static gpio_set_t pb8_rst_gpio = {
	{GPIOB, GPIO_PINS_8,  CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, NUL_PINSRC, NUL_MUXSEL}, //(PB8) RST-pin
};

const static spi_dev_t devconf[BOARD_SPI_COUNT] = {
	{
		"SPI2",
		SPI2,
		CRM_SPI2_PERIPH_CLOCK,
		  {GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,    NUL_PINSRC, NUL_MUXSEL},
		  {GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	  NUL_PINSRC, NUL_MUXSEL},	//IMISO
		  {GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	  NUL_PINSRC, NUL_MUXSEL},	//IMOSI
	      {GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, NUL_PINSRC, NUL_MUXSEL}, //(PB12) Test-ISP4 OK
		&pc7_intr_gpio, //either NULL, or 'gpio_set_t' to enable (e.g. '&pc7_intr_gpio')
		&pc7_intr, //NULL, /* NULL for disable, or a pointer to 'intr_set_t' to enable (e.g. '&pc7_intr') */
		&pb8_rst_gpio, //NULL, /* NULL for disable, or a pointer to 'gpio_set_t' to enable (e.g. '&pb8_rst_gpio') */
	},
};

/**
  * @}
  */

#if 0
		//[for generic and multiple ndevs support.]
		void set_pin_code(int i);
		void next_pin_code(void);

		void set_pin_code(int i) {
			_pinCode = i;
		}
		void next_pin_code(void)
		{
			_pinCode = 1; //(SPI2)
		}
#endif

#endif //__DM9051_MF_H
