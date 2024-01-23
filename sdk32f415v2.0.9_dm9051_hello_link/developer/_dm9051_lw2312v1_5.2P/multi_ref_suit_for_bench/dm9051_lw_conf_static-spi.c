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

/**
  **************************************************************************
  * @file= #include "dm9051_lw_conf_data.h" 
  **************************************************************************
**/
#define BOARD_SPI_COUNT						1 //1 //4 //3 //1 //2 //1 //3 //1 //2 //1 //x //ETH_COUNT

/* Sanity.
 * 'ETHERNET_COUNT' is defined in "dm9051opts.h" since the application did declare the eth numbers, firstly.
 */
#if (BOARD_SPI_COUNT < ETHERNET_COUNT)
#error "Please make sure that _BOARD_SPI_COUNT(config here) must large equal to _ETHERNET_COUNT"
#endif

/* User_data 1
 */
#if (BOARD_SPI_COUNT == 1)
//
// Single-SPI ethernet f413/f437
//
const static spi_dev_t devconf = {
		"SPI2",
		SPI2,
		CRM_SPI2_PERIPH_CLOCK,
		  //ethernet f413
		  /*
		  "AT32F413 SPI2 ETHERNET, pb13/pb14/pb15/pb12",
		  {GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,    NULL_PINSRC, NULL_MUXSEL},  //ISCK
		  {GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	  NULL_PINSRC, NULL_MUXSEL},	//IMISO
		  {GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	  NULL_PINSRC, NULL_MUXSEL},	//IMOSI
	      {GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, NULL_PINSRC, NULL_MUXSEL}, //(PB12) Test-ISP2 OK
		  */
		  //ethernet f437
		  "AT32F437 SPI2 ETHERNET, pd1/pc2/pd4/pd0",
		  {GPIOD, GPIO_PINS_1, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE1, GPIO_MUX_6},   //ISCK
		  {GPIOC, GPIO_PINS_2, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE2, GPIO_MUX_5},	//IMISO
		  {GPIOD, GPIO_PINS_4, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE4, GPIO_MUX_6},	//IMOSI
	      {GPIOD, GPIO_PINS_0, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  NULL_PINSRC, NULL_MUXSEL}, //(PD0) Test-ISP2 OK
		//intr, rst both empty
		NULL, //&pc7_intr_gpio, //either NULL, or 'gpio_set_t' to enable (e.g. '&pc7_intr_gpio')
		NULL, //&pc7_intr, //NULL, // NULL for disable, or a pointer to 'intr_set_t' to enable (e.g. '&pc7_intr')
		NULL, //&pb8_rst_gpio, //NULL, // NULL for disable, or a pointer to 'gpio_set_t' to enable (e.g. '&pb8_rst_gpio')
		//intr, rst defined as pc7, pb8
		//&pc7_intr_gpio, //either NULL, or 'gpio_set_t' to enable (e.g. '&pc7_intr_gpio')
		//&pc7_intr, //NULL, // NULL for disable, or a pointer to 'intr_set_t' to enable (e.g. '&pc7_intr')
		//&pb8_rst_gpio, //NULL, // NULL for disable, or a pointer to 'gpio_set_t' to enable (e.g. '&pb8_rst_gpio')
};
#elif (BOARD_SPI_COUNT >= 2)

/* User_data 2
 */
const static gpio_set_t pc7_intr_gpio = {
	{GPIOC, GPIO_PINS_7,  CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_INPUT, NULL_PINSRC, NULL_MUXSEL}, //(PC7) INT-pin
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
	{GPIOB, GPIO_PINS_8,  CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, NULL_PINSRC, NULL_MUXSEL}, //(PB8) RST-pin
};

//
// Multi-SPI ethernet f413
//
//
// Multi-SPI ethernet f437
//
const static spi_dev_t devconf[BOARD_SPI_COUNT] = {
	{
		"SPI2",
		SPI2,
		CRM_SPI2_PERIPH_CLOCK,
		  //ethernet f413
		  /*
		  "AT32F413 SPI2 ETHERNET, pb13/pb14/pb15/pb12",
		  {GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,    NULL_PINSRC, NULL_MUXSEL},  //ISCK
		  {GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	  NULL_PINSRC, NULL_MUXSEL},	//IMISO
		  {GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	  NULL_PINSRC, NULL_MUXSEL},	//IMOSI
	      {GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, NULL_PINSRC, NULL_MUXSEL}, //(PB12) Test-ISP2 OK
		  */
		  //ethernet f437
		  "AT32F437 SPI2 ETHERNET, pd1/pc2/pd4/pd0",
		  {GPIOD, GPIO_PINS_1, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE1, GPIO_MUX_6},   //ISCK
		  {GPIOC, GPIO_PINS_2, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE2, GPIO_MUX_5},	//IMISO
		  {GPIOD, GPIO_PINS_4, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE4, GPIO_MUX_6},	//IMOSI
	      {GPIOD, GPIO_PINS_0, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  NULL_PINSRC, NULL_MUXSEL}, //(PD0) Test-ISP2 OK
		//intr, rst both empty
		NULL, //&pc7_intr_gpio, //either NULL, or 'gpio_set_t' to enable (e.g. '&pc7_intr_gpio')
		NULL, //&pc7_intr, //NULL, // NULL for disable, or a pointer to 'intr_set_t' to enable (e.g. '&pc7_intr')
		NULL, //&pb8_rst_gpio, //NULL, // NULL for disable, or a pointer to 'gpio_set_t' to enable (e.g. '&pb8_rst_gpio')
		//intr, rst defined as pc7, pb8
		/*
		&pc7_intr_gpio, //either NULL, or 'gpio_set_t' to enable (e.g. '&pc7_intr_gpio')
		&pc7_intr, //NULL, // NULL for disable, or a pointer to 'intr_set_t' to enable (e.g. '&pc7_intr')
		&pb8_rst_gpio, //NULL, // NULL for disable, or a pointer to 'gpio_set_t' to enable (e.g. '&pb8_rst_gpio')
		*/
	},
	{
		"SPI3",
		SPI3,
		CRM_SPI3_PERIPH_CLOCK,
		  "AT32F437 SPI3 ETHERNET, pc10/pc11/pc12/pb12",
		  {GPIOC, GPIO_PINS_10, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE10, GPIO_MUX_6},   //ISCK
		  {GPIOC, GPIO_PINS_11, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	   GPIO_PINS_SOURCE11, GPIO_MUX_6},	//IMISO
		  {GPIOC, GPIO_PINS_12, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	   GPIO_PINS_SOURCE12, GPIO_MUX_6},	//IMOSI
	      {GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  NULL_PINSRC, NULL_MUXSEL}, //(PD0) Test-ISP2 OK
		NULL, //&pc7_intr_gpio, //either NULL, or 'gpio_set_t' to enable (e.g. '&pc7_intr_gpio')
		NULL, //&pc7_intr, //NULL, // NULL for disable, or a pointer to 'intr_set_t' to enable (e.g. '&pc7_intr')
		NULL, //&pb8_rst_gpio, //NULL, // NULL for disable, or a pointer to 'gpio_set_t' to enable (e.g. '&pb8_rst_gpio')
	},
	{
		"SPI1",
		SPI1,
		CRM_SPI1_PERIPH_CLOCK,
		  "AT32F437 SPI1 ETHERNET, pa5/pa6/pa7/pa15",
		  {GPIOA, GPIO_PINS_5, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE5, GPIO_MUX_5},   //ISCK
		  {GPIOA, GPIO_PINS_6, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE6, GPIO_MUX_5},	//IMISO
		  {GPIOA, GPIO_PINS_7, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE7, GPIO_MUX_5},	//IMOSI
	      {GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  NULL_PINSRC, NULL_MUXSEL}, //(PA15) Test-ISP1 OK
		&pc7_intr_gpio, //either NULL, or 'gpio_set_t' to enable (e.g. '&pc7_intr_gpio')
		&pc7_intr, //NULL, // NULL for disable, or a pointer to 'intr_set_t' to enable (e.g. '&pc7_intr')
		&pb8_rst_gpio, //NULL, // NULL for disable, or a pointer to 'gpio_set_t' to enable (e.g. '&pb8_rst_gpio')
	},
	{
		"SPI4",
		SPI4,
		CRM_SPI4_PERIPH_CLOCK,
		  "AT32F437 SPI4 ETHERNET, pe2/pe5/pe6/pa4",
		  {GPIOE, GPIO_PINS_2, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE2, GPIO_MUX_5},   //ISCK
		  {GPIOE, GPIO_PINS_5, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE5, GPIO_MUX_5},	//IMISO
		  {GPIOE, GPIO_PINS_6, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE6, GPIO_MUX_5},	//IMOSI
	      {GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  NULL_PINSRC, NULL_MUXSEL}, //(PD0) Test-ISP2 OK
		NULL, //&pc7_intr_gpio, //either NULL, or 'gpio_set_t' to enable (e.g. '&pc7_intr_gpio')
		NULL, //&pc7_intr, //NULL, // NULL for disable, or a pointer to 'intr_set_t' to enable (e.g. '&pc7_intr')
		NULL, //&pb8_rst_gpio, //NULL, // NULL for disable, or a pointer to 'gpio_set_t' to enable (e.g. '&pb8_rst_gpio')
	},
};
#endif

/* Varibles_and_access
 */
#if (BOARD_SPI_COUNT == 1)
#define board_conf_type "\"dm9051_at32_cf\"" //Setting

#define spi_dev()					&devconf
#define spi_conf_name()				devconf.spi_name
#define spi_conf_port()				devconf.spi_num
#define cpu_spi_conf_name()			devconf.cpu_api_info
#define gpio_wire_sck()				devconf.wire_sck 
#define gpio_wire_mi()				devconf.wire_mi
#define gpio_wire_mo()				devconf.wire_mo
#define gpio_cs()					devconf.wire_cs
#define intr_conf_ptr()				((const gpio_set_t *)devconf.intr_gpio_set)
#define rst_conf_ptr()				((const gpio_set_t *)devconf.rst_gpio_set)
#define rst_gpio_ptr()				((const gpio_t *)&devconf.rst_gpio_set->wire_descript)
#define gpio_set_field_loc(gs)		((const gpio_t *)&gs->wire_descript) //generic.
#define exint_conf_ptr()			((const intr_set_t *)devconf.intr_set) //&exintd
#define mstep_set_index(i)
#define mstep_turn_net_index()		//void mstep_next_net_index(void); //#define mstep_next_net_index()

#elif (BOARD_SPI_COUNT >= 2)
#define board_conf_type "\"dm9051_at32_mf\"" //Settings

#define spi_dev()					&devconf[pinCode]
#define spi_conf_name()				devconf[pinCode].spi_name
#define spi_conf_port()				devconf[pinCode].spi_num
#define cpu_spi_conf_name()			devconf[pinCode].cpu_api_info
#define gpio_wire_sck()				devconf[pinCode].wire_sck 
#define gpio_wire_mi()				devconf[pinCode].wire_mi
#define gpio_wire_mo()				devconf[pinCode].wire_mo
#define gpio_cs()					devconf[pinCode].wire_cs
#define intr_conf_ptr()				((const gpio_set_t *)devconf[pinCode].intr_gpio_set)
#define rst_conf_ptr()				((const gpio_set_t *)devconf[pinCode].rst_gpio_set)
#define rst_gpio_ptr()				((const gpio_t *)&devconf[pinCode].rst_gpio_set->wire_descript)
#define gpio_set_field_loc(gs)		((const gpio_t *)&gs->wire_descript) //generic.
#define exint_conf_ptr()			((const intr_set_t *)devconf[pinCode].intr_set) //&exintd

int pinCode;
#define mstep_set_index(i) \
	pinCode = i
#define mstep_turn_net_index() \
	pinCode++; \
	if (pinCode >= ETHERNET_COUNT) \
		pinCode = 0
#endif

/**
  * @brief  gpio pin configuration.
  * @param  gpio_t* =
  *         struct {
  * 	        gpio_type *gpport;        		// GPIOA/GPIOB/GPIOC
  * 	        uint16_t pin;           		// GPIO_PINS_0/GPIO_PINS_1/GPIO_PINS_2
  * 	        crm_periph_clock_type crm_clk;  // CRM_GPIOA_PERIPH_CLOCK/CRM_GPIOB_PERIPH_CLOCK
  * 	        gpio_pins_source_type pinsrc;   // GPIO_PINS_SOURCE0/GPIO_PINS_SOURCE1/GPIO_PINS_SOURCE2
  * 	        gpio_mux_sel_type mux;          // GPIO_MUX_0/GPIO_MUX_1/GPIO_MUX_2
  *         } gpio_t;
  *         gpio_mode_type = gpmode;            // GPIO_MODE_INPUT/GPIO_MODE_OUTPUT/GPIO_MODE_MUX
  *         gpio_pull_type = gppull;            // GPIO_PULL_UP/GPIO_PULL_DOWN/GPIO_PULL_NONE
  * @retval None
  */
static void gpio_pin_config(const gpio_t *gpio, gpio_pull_type gppull) //, gpio_mode_type gpmode
{
  gpio_init_type gpio_init_struct;
  if (gpio->gpport) {
	  crm_periph_clock_enable(gpio->crm_clk, TRUE); /* enable the gpioa clock */

	  gpio_default_para_init(&gpio_init_struct);
	  gpio_init_struct.gpio_out_type  		= GPIO_OUTPUT_PUSH_PULL;
	  gpio_init_struct.gpio_drive_strength	= GPIO_DRIVE_STRENGTH_STRONGER;
	  gpio_init_struct.gpio_mode			= gpio->gpio_mode; //gpmode; //GPIO_MODE_INPUT;

	  /* gp */
	  gpio_init_struct.gpio_pull			= gppull; //exint_cfg.gpio_pull; //GPIO_PULL_DOWN; GPIO_PULL_UP; //GPIO_PULL_NONE;
	  gpio_init_struct.gpio_pins			= gpio->pin;
	  gpio_init(gpio->gpport, &gpio_init_struct);
	  
	 #if gpio_mux_sel_type_system_declaration
	  if (gpio->mux != NULL_MUXSEL)
		gpio_pin_mux_config(gpio->gpport, gpio->pinsrc, gpio->mux);
	 #endif
  }
}

/**
  * @brief  spi configuration.
  * @param  spi_inf_t* =
  *         struct {
  *         	spi_type *num;        			//= SPIPORT;
  *         	crm_periph_clock_type crm_clk;	//= SPI_CRM_CLK;	
  *         } spi_inf_t;
  * @retval None
  */
static void spi_config(const spi_dev_t *pdev)
{
  spi_init_type spi_init_struct;
  crm_periph_clock_enable(pdev->crm_clk, TRUE); //crm_spi_clk(_pinCode), CRM_SPI3_PERIPH_CLOCK/CRM_SPI2_PERIPH_CLOCK

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
  spi_init(pdev->spi_num, &spi_init_struct); //SPI2, _spi_num(_pinCode)
  spi_enable(pdev->spi_num, TRUE); //SPI2, _spi_num(_pinCode)
}

/**
  * @brief  exint pin configuration.
  * @param  struct exint_st* =
  * 	   struct exint_st {
  * 		struct {
  * 			crm_periph_clock_type crm_clk; //CRM_GPIOC_PERIPH_CLOCK,
  * 			uint32_t extline; //= LINE
  * 			IRQn_Type irqn; //= EXINTn_m
  * 		} line;
  * 		struct {
  * 			crm_periph_clock_type scfg_clk; //CRM_SCFG_PERIPH_CLOCK
  * 			scfg_port_source_type scfg_port_src; //SCFG_PORT_SOURCE_X
  * 			scfg_pins_source_type scfg_pin_src; //SCFG_PINS_SOURCEX
  * 		} scfg;
  * 	   }
  *        exint_polarity_config_type
  *        nvic_priority_group_type
  * @e.g.  exint line7 config. configure pc7 in interrupt pin
  * @retval None
  */
static void exint_config(const intr_set_t *pexint_set, exint_polarity_config_type polarity, nvic_priority_group_type priority) //(uint32_t line_no, exint_polarity_config_type pol)
{
  printf(": %s :                 exint-irq, pexint_set\r\n", pexint_set ? "config" : "null");
  if (pexint_set) { //_devconf[_pinCode].intr_set
	  exint_init_type exint_init_struct;
		
	  crm_periph_clock_enable(pexint_set->scfg.scfg_clk, TRUE); // CRM_SCFG_PERIPH_CLOCK
	  crm_periph_clock_enable(pexint_set->line.crm_clk, TRUE); // CRM_GPIOC_PERIPH_CLOCK

	 #if scfg_exint_system_ready == 0
	  gpio_exint_line_config(pexint_set->scfg.scfg_port_src, pexint_set->scfg.scfg_pin_src); //SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0
	 #else
	  scfg_exint_line_config(pexint_set->scfg.scfg_port_src, pexint_set->scfg.scfg_pin_src); //SCFG_PORT_SOURCE_GPIOC, SCFG_PINS_SOURCE7
	 #endif
		
	  exint_default_para_init(&exint_init_struct);
	  exint_init_struct.line_enable = TRUE;
		
	  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
	  exint_init_struct.line_select = pexint_set->line.extline; //line_no;
	  exint_init_struct.line_polarity = polarity; //EXINT_TRIGGER_RISING_EDGE/ EXINT_TRIGGER_FALLING_EDGE
	  exint_init(&exint_init_struct);
		
	  nvic_priority_group_config(priority); //NVIC_PRIORITY_GROUP_0
	  nvic_irq_enable(pexint_set->line.irqn, 1, 0); //EXINT9_5_IRQn
  }
}

void rst_intf_conf(const gpio_set_t *rst_gpio_set)
{
  printf(": %s :                 rst-pin, rst_gpio_set\r\n", rst_gpio_set ? "config" : "null");
  if (rst_gpio_set) //=_conf_rstport()
	gpio_pin_config(gpio_set_field_loc(rst_gpio_set), GPIO_PULL_UP); //=(rst_gpio_ptr(_pinCode), GPIO_PULL_UP); //,GPIO_MODE_OUTPUT
}

static void exint_inft_config(const gpio_set_t *intr_gpio_set, const intr_set_t *pexint_set)
{
  printf(": %s :                 intr-pin, intr_gpio_set\r\n", intr_gpio_set ? "config" : "null");
  if (intr_gpio_set) //=(intr_conf_ptr(_pinCode)) 
	  gpio_pin_config(gpio_set_field_loc(intr_gpio_set),  GPIO_PULL_UP); //=(intr_gpio_ptr(_pinCode),  GPIO_PULL_UP); //,GPIO_MODE_INPUT //exint
  
  exint_config(pexint_set, EXINT_TRIGGER_FALLING_EDGE, NVIC_PRIORITY_GROUP_0);
}

/*void spi_intf_config(void)
{
//=spi_pins_config(gpio_wires(_pinCode));
  gpio_pin_config(&gpio_wire_sck(_pinCode), GPIO_PULL_NONE); //,GPIO_MODE_MUX
  gpio_pin_config(&gpio_wire_mi(_pinCode), GPIO_PULL_NONE); //,GPIO_MODE_MUX
  gpio_pin_config(&gpio_wire_mo(_pinCode), GPIO_PULL_NONE); //,GPIO_MODE_MUX //GPIO_PULL_UP; //test ffff
//=gpio_cs_pin_config();
  gpio_pin_config(&gpio_cs(_pinCode), GPIO_PULL_NONE); //,GPIO_MODE_OUTPUT

  spi_config(spi_dev(_pinCode)); //(spi_port_ptr(_pinCode));
}*/

void add_net_configure_log_s(void)
{
	#if (_BOARD_SPI_COUNT >= 2)
	//if (mstep_conf_spi_count() >= 2) {
	printf(": _net_add_configure(void) for device %d\r\n", pinCode);
	//}
	#endif
}
void add_net_configure_log_e(void)
{
}

/**
  * @brief  gpio configuration.
  * @brief  spi configuration.
  * @brief  exint configuration.
  */
void net_add_configure(void) //=== pins_config(); //total_eth_count++;
{
//==spi_intf_config();
//=spi_pins_config(gpio_wires(_pinCode));
  gpio_pin_config(&gpio_wire_sck(), GPIO_PULL_NONE); //,GPIO_MODE_MUX
  gpio_pin_config(&gpio_wire_mi(), GPIO_PULL_NONE); //,GPIO_MODE_MUX
  gpio_pin_config(&gpio_wire_mo(), GPIO_PULL_NONE); //,GPIO_MODE_MUX //GPIO_PULL_UP; //test ffff
//=gpio_cs_pin_config();
  spi_config(spi_dev()); //(spi_port_ptr(_pinCode));
  gpio_pin_config(&gpio_cs(), GPIO_PULL_NONE); //,GPIO_MODE_OUTPUT
	
  exint_inft_config(intr_conf_ptr(), exint_conf_ptr());
	
  rst_intf_conf(conf_rstport()); //(=_rst_conf_ptr());
}

void dm9051_board_initialize(void)
{
  int i;
  for (i = 0; i < ETHERNET_COUNT; i++) { //get_eth_interfaces()
	mstep_set_net_index(i); //_pinCode = i;
	add_net_configure_log_s();
	net_add_configure(); //=== pins_config()
	add_net_configure_log_e();
  }
  mstep_set_net_index(0); //_pinCode = 0; //[No need, only because if multi-devs, there is a rotate access-design.]
}

// -

void mstep_set_net_index(int i)
{
	mstep_set_index(i); //pinCode = i;
}

void mstep_next_net_index(void)
{
	mstep_turn_net_index();
}

char *mstep_spi_conf_name(void)
{
	return spi_conf_name();
}

char *mstep_conf_cpu_spi_ethernet(void)
{
	return cpu_spi_conf_name();
}

char *mstep_conf_type(void)
{
	return board_conf_type;
}

int mstep_conf_spi_count(void)
{
	return BOARD_SPI_COUNT;
}

// -

spi_type *spi_no(void)
{
	return spi_conf_port(); //return spi_port(_pinCode).spi_num; //return spi_type_no(_pinCode);
}

gpio_type *gpio_csport(void)
{
	return gpio_cs().gpport;
}

uint16_t gpio_cspin(void)
{
	return gpio_cs().pin; //output
}

const gpio_set_t *conf_rstport(void)
{
	return rst_conf_ptr();
}

gpio_type *gpio_rstport(void)
{
	return rst_gpio_ptr()->gpport;
	//=
	//const gpio_set_t *rst_gpio_set = _rst_conf_ptr(_pinCode); if (rst_gpio_set) ...
	//return gpio_set_field_loc(rst_gpio_set)->gpport;
}

uint16_t gpio_rstpin(void)
{
	return rst_gpio_ptr()->pin; //output
	//=
	//const gpio_set_t *rst_gpio_set = _rst_conf_ptr(_pinCode); if (rst_gpio_set) ...
	//return gpio_set_field_loc(rst_gpio_set)->pin;
}
