/**
  **************************************************************************
  * @file     dm9051_lw_conf.c
  * @file     dm9051_at32_conf.c
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
#include "dm9051_lw.h"
#include "dm9051_lw_conf.h"
#include "dm9051_lw_conf_types.h"
//#include "netconf.h"
//#include "testproc/testproc_lw.h"

//#if (_BOARD_SPI_COUNT >= 2)
//#endif //_BOARD_SPI_COUNT

#ifdef AT32F437xx
#include "dm9051_lw_conf_at437x2spi.h" //1.yicheng
#else
#include "dm9051_lw_conf_at437x2spi.h" //or "dm9051_lw_conf_at415x2spi.h"
#endif

void dm9051_irqlines_proc(void)
{
  void ethernetif_line7_proc(void);
  int i;

  for (i = 0; i < ETHERNET_COUNT; i++) { //get_eth_interfaces()
	  if (exint_exister()) {  //[To be enum , e.g. intr_pack[i], if multi-cards]
		  if (exint_flag_get(exint_extline()) != RESET) //if (exint_flag_get(EXINT_LINE_7) != RESET)
		  {
			#if 0
			//ethernetif_line7_proc();
			#endif
			exint_flag_clear(exint_extline()); //exint_flag_clear(EXINT_LINE_7);
		  }
	  }
  }
}

/*********************************
  * @brief  gpio pin configuration
  * @brief  spi configuration
  * @brief  exint configuration
 *********************************/

static void gpio_pin_config(const gpio_t *gpio, gpio_pull_type gppull) //, gpio_mode_type gpmode
{
  gpio_init_type gpio_init_struct;
  crm_periph_clock_enable(gpio->gpio_crm_clk, TRUE); /* enable the gpioa clock */

  /* gp */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_out_type  		= GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_drive_strength	= GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_mode			= gpio->gpio_mode; //gpmode; //GPIO_MODE_INPUT;

  gpio_init_struct.gpio_pull			= gppull; //exint_cfg.gpio_pull; //GPIO_PULL_DOWN; GPIO_PULL_UP; //GPIO_PULL_NONE;
  gpio_init_struct.gpio_pins			= gpio->pin;
  gpio_init(gpio->gpport, &gpio_init_struct);
  
 #ifdef AT32F437xx
  if ((gpio->gpio_mode == GPIO_MODE_MUX) && (gpio->muxsel != GPIO_MUX_NULL))
	gpio_pin_mux_config(gpio->gpport, gpio->pinsrc, gpio->muxsel);
 #endif
}

/**
  * @brief  spi configuration.
  * @param  spi_inf_t* =
  *         struct {
  *         	spi_type *num;        			//= SPIPORT;
  *         	crm_periph_clock_type spi_crm_clk;	//= SPI_CRM_CLK;	
  *         } spi_inf_t;
  * @retval None
  */
static void spi_config(void)
{
  spi_init_type spi_init_struct;
  crm_periph_clock_enable(spi_crm(), TRUE); //crm_spi_clk(_pinCode), CRM_SPI3_PERIPH_CLOCK/CRM_SPI2_PERIPH_CLOCK

  spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  //spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_LSB;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  //spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(spi_number(), &spi_init_struct); //SPI2, _spi_num(_pinCode)
  spi_enable(spi_number(), TRUE); //SPI2, _spi_num(_pinCode)
}

/**
  * @brief  exint pin configuration.
  * @param  struct exint_st* =
  * 	   struct exint_st {
  * 		struct {
  * 			crm_periph_clock_type crm_clk; //CRM_GPIOC_PERIPH_CLOCK,
  * 			uint32_t extline; //= LINE
  * 			IRQn_Type irqn; //= EXINTn_m
  * 		} extline;
  * 		struct {
  * 			crm_periph_clock_type scfg_clk; //CRM_SCFG_PERIPH_CLOCK
  * 			scfg_port_source_type scfg_port_src; //SCFG_PORT_SOURCE_X
  * 			scfg_pins_source_type scfg_pin_src; //SCFG_PINS_SOURCEX
  * 		} scfg_init;
  * 	   }
  *        exint_polarity_config_type
  *        nvic_priority_group_type
  * @e.g.  exint line7 config. configure pc7 in interrupt pin
  * @retval None
  */
static void exint_config(const struct extscfg_st *pexint_set, exint_polarity_config_type polarity) {
  exint_init_type exint_init_struct;
	
  crm_periph_clock_enable(scfg_crm(), TRUE); // CRM_SCFG_PERIPH_CLOCK
  crm_periph_clock_enable(exint_crm(), TRUE); // CRM_GPIOC_PERIPH_CLOCK

 #ifndef AT32F437xx
  gpio_exint_line_config(scfg_port(), scfg_pin()); //SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0
 #else
  scfg_exint_line_config(scfg_port(), scfg_pin()); //SCFG_PORT_SOURCE_GPIOC, SCFG_PINS_SOURCE7
 #endif
	
  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
	
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = pexint_set->extline.extline; //line_no;
  exint_init_struct.line_polarity = polarity; //EXINT_TRIGGER_RISING_EDGE/ EXINT_TRIGGER_FALLING_EDGE
  exint_init(&exint_init_struct);
}

static void exint_enable(const struct extscfg_st *pexint_set, nvic_priority_group_type priority) {
  nvic_priority_group_config(priority); //NVIC_PRIORITY_GROUP_0
  nvic_irq_enable(pexint_set->extline.irqn, 1, 0); //nvic_irq_enable(EXINT9_5_IRQn, 1, 0); //i.e.= //_misc
}

//[finally enable
void exint_menable(nvic_priority_group_type priority)
{
	const struct extscfg_st *pexint_set = (const struct extscfg_st *) exint_scfg_ptr();
	if (pexint_set) {
		printf(": %s :                 exint-enable/ %s\r\n", "config", exint_enable_info()); //pexint_set
		exint_enable(pexint_set, priority);
	}
}

//-caller
void exint_mconfig(exint_polarity_config_type polarity)
{
	const struct extscfg_st *pexint_set = (const struct extscfg_st *) exint_scfg_ptr();
	if (pexint_set) {
		printf(": %s :                 exint_config/ %s\r\n", "config", scfg_info());
		printf("................................ dm9051 exint_init(_exint_conf_ptr())\r\n");
		exint_config(pexint_set, polarity);
	}
}

static void config_exint(gpio_pull_type gppull, exint_polarity_config_type polarity)
{
  if (intr_gpio_mptr()) {
	  printf("................................ dm9051 gpio_pin_config(for intr)\r\n");
	  gpio_pin_config(intr_gpio_ptr(), gppull);
  }

  exint_mconfig(polarity);
}

/*********************************
 * dm9051 delay times procedures 
 *********************************/

#define	board_printf(format, args...) //int board_printf(const char *format, args...) { return 0; }

void dm_delay_us(uint32_t nus) {
	void delay_us(uint32_t nus);
	board_printf("test %d ,because rxb %02x (is %d times)\r\n", rstccc, rxbyteee, timesss);
	delay_us(nus);
}
void dm_delay_ms(uint16_t nms) {
	void delay_ms(uint16_t nms);
	delay_ms(nms);
}

// -

//void dm9051_board_irq_enable(void) //void net_irq_enable(void)
//{
//  exint_menable(NVIC_PRIORITY_GROUP_0); //if (exint_conf_mptr()) _exint_enable(_exint_conf_ptr(), NVIC_PRIORITY_GROUP_0);
//}

/**
  * @brief  gpio configuration.
  * @brief  spi configuration.
  * @brief  exint configuration.
  */
static void spi_add(void) //=== pins_config(); //total_eth_count++;
{
  // When if [AT32F413/415], when SPI1 use pb3/pb4/pb5 for sck/mi/mo
  if (spi_number() == SPI1) {
	  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	  gpio_pin_remap_config(SWJTAG_GMUX_010, TRUE);
	  
	  //.
	  //gpio_pin_remap_config(SPI1_MUX_01, TRUE);
	  //.
	  //.dma_channel_enable(DMA1_CHANNEL4, TRUE);
  }
  
//.  printf(": spi_add :                %s\r\n", mstep_spi_conf_name());
  gpio_pin_config(&gpio_wire_sck(), GPIO_PULL_NONE); //,GPIO_MODE_MUX
  gpio_pin_config(&gpio_wire_mi(), GPIO_PULL_NONE); //,GPIO_MODE_MUX
  gpio_pin_config(&gpio_wire_mo(), GPIO_PULL_NONE); //,GPIO_MODE_MUX //GPIO_PULL_UP; //test ffff
  spi_config(); //(spi_port_ptr(_pinCode));
  gpio_pin_config(&gpio_cs(), GPIO_PULL_NONE); //,GPIO_MODE_OUTPUT
}

void rst_add(void)
{
  if (rst_pin_mexist())
	gpio_pin_config(rst_gpio_ptr(), GPIO_PULL_UP); //=(rst_gpio_ptr(_pinCode), GPIO_PULL_UP); //,GPIO_MODE_OUTPUT
}

void exint_add(void)
{
  config_exint(GPIO_PULL_UP, EXINT_TRIGGER_FALLING_EDGE); //
}

int dm9051_board_initialize(void)
{
  int i;
  //.bannerline_log();
  //.GpioDisplay();
	
  //.bannerline_log();
  for (i = 0; i < ETHERNET_COUNT; i++) { //get_eth_interfaces()
	mstep_set_net_index(i);
	
	spi_add();
	rst_add();
	exint_add();
  }

  cpin_poweron_reset();
  return ETHERNET_COUNT;
}

// -
/*********************************
 * dm9051 spi interface accessing 
 *********************************/

static void spi_cs_lo(void) {
	gpio_bits_reset(gpio_cs().gpport, gpio_cs().pin); //cs.gpport->clr = cs.pin;
}
static void spi_cs_hi(void) {
	gpio_bits_set(gpio_cs().gpport, gpio_cs().pin); //cs.gpport->scr = cs.pin;
}

static uint8_t spi_exc_data(uint8_t byte) {
    while(spi_i2s_flag_get(spi_number(), SPI_I2S_TDBE_FLAG) == RESET);	//while(spi_i2s_flag_get(SPI2, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(spi_number(), byte);							//spi_i2s_data_transmit(SPI2, byte); //spi2 tx	
    while(spi_i2s_flag_get(spi_number(), SPI_I2S_RDBF_FLAG) == RESET);	//while(spi_i2s_flag_get(SPI2, SPI_I2S_RDBF_FLAG) == RESET);
    return (uint8_t) spi_i2s_data_receive(spi_number());				//return (uint8_t) spi_i2s_data_receive(SPI2); //spi2 rx (rx_pad)
}

static void rst_pin_pulse(void) {
	gpio_bits_reset(rst_gpio_ptr()->gpport, rst_gpio_ptr()->pin); //rstpin_lo();
	dm_delay_ms(1);
	gpio_bits_set(rst_gpio_ptr()->gpport, rst_gpio_ptr()->pin); //rstpin_hi();
}

// -
/*********************************
 * functions for driver's ops 
 *********************************/

#define dm9051if_rstb_pulse() rst_pin_pulse() //.dm9051_if->rstb_pulse()
#define dm9051if_cs_lo() spi_cs_lo()
#define dm9051if_cs_hi() spi_cs_hi()
#define dm9051_spi_command_write(rd) spi_exc_data(rd)
#define dm9051_spi_dummy_read() spi_exc_data(0)

void cspi_read_regs(uint8_t reg, u8 *buf, u16 len, csmode_t csmode)
{
	int i;
	if (csmode == CS_LONG) {
	  dm9051if_cs_lo();
	  for(i=0; i < len; i++, reg++) {
		dm9051_spi_command_write(reg | OPC_REG_R);
		buf[i] = dm9051_spi_dummy_read();
	  }
	  dm9051if_cs_hi();
	}
	else { //CS_EACH
	  for(i=0; i < len; i++, reg++) {
		//printf("cspi_read_reg(reg) ..\r\n");
		buf[i] = cspi_read_reg(reg);
	  }
	}
}

void cpin_poweron_reset(void)
{
	if (rst_pin_exister())
		dm9051if_rstb_pulse();
}

uint8_t cspi_read_reg(uint8_t reg) //static (todo)
{
	uint8_t val;
	dm9051if_cs_lo();
	dm9051_spi_command_write(reg | OPC_REG_R);
	val = dm9051_spi_dummy_read();
	dm9051if_cs_hi();
	return val;
}
void cspi_write_reg(uint8_t reg, uint8_t val)
{
	dm9051if_cs_lo();
	dm9051_spi_command_write(reg | OPC_REG_W);
	dm9051_spi_command_write(val);
	dm9051if_cs_hi();
}
uint8_t cspi_read_mem2x(void)
{
	uint8_t rxb;
	dm9051if_cs_lo();
	dm9051_spi_command_write(DM9051_MRCMDX | OPC_REG_R);
	rxb = dm9051_spi_dummy_read();
	rxb = dm9051_spi_dummy_read();
	dm9051if_cs_hi();
	return rxb;
}
void cspi_read_mem(u8 *buf, u16 len)
{
	int i;
	dm9051if_cs_lo();
	dm9051_spi_command_write(DM9051_MRCMD | OPC_REG_R);
	if (iomode() == MBNDRY_WORD) //dm9051opts_iomode(), MBNDRY_DEFAULT == MBNDRY_WORD
	if (len & 1)
		len++;
	for(i=0; i<len; i++)
		buf[i] = dm9051_spi_dummy_read();
	dm9051if_cs_hi();
}
void cspi_write_mem(u8 *buf, u16 len)
{
	int i;
	dm9051if_cs_lo();
	dm9051_spi_command_write(DM9051_MWCMD | OPC_REG_W);
	if (iomode() == MBNDRY_WORD) //dm9051opts_iomode(), MBNDRY_DEFAULT == MBNDRY_WORD
	if (len & 1)
		len++;
	for(i=0; i<len; i++)
		dm9051_spi_command_write(buf[i]);
	dm9051if_cs_hi();
}
