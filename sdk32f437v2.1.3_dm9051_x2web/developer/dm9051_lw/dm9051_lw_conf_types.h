/*typedef enum {
	NEXT_NULL       = 0x00,
	NEXT_AVAILABLE  = 0x01
} next_ext_param;

struct scfg_st {
	const char *irq_enable_inf;	
	extline_t extline;
	const char *scfg_inf;	
	extint_init_t scfg_init;
	next_ext_param next_avail;
};

typedef struct {
	struct scfg_st scfg;
} intr_param_t;

typedef struct {
	struct scfg_st scfg;
	const char *gp_info;	//gpio-info
	const gpio_t gp;        //gpio-t
} intr_paramfull_t;*/

#if 0 //[Test]
	/*
	typedef struct spi_pack_st {
		char *cpu_spi_info;	//cpu name is for the purpose to lead the pins, for easy recogition!
		spihead_t spidef;
		gpio_t wire_sck;
		gpio_t wire_mi;
		gpio_t wire_mo;
		gpio_t wire_cs;
	} spi_pack_t;

	#ifndef AT32F437xx
	const spi_pack_t spi2_pack_f413 = {
		//ethernet f413 spi2(crm, gpio)
		"AT32F413 SPI2 ETHERNET, sck/mi/mo/cs pb13/pb14/pb15/pb12",
		{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK},
		{GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,    GPIO_PINSRC_NULL, GPIO_MUX_NULL},  //ISCK
		{GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMISO
		{GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMOSI
		{GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PB12) Test-ISP2 OK
	};
	#define pack_decl	 spi2_pack_f413 
	const spi_pack_t spi2_pack_f437 = {
		//ethernet f437 spi2(crm, gpio)
		"AT32F437 SPI2 ETHERNET, sck/mi/mo/cs pd1/pc2/pd4/pd0",
		{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK},
		{GPIOD, GPIO_PINS_1, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE1, GPIO_MUX_6},   //ISCK
		{GPIOC, GPIO_PINS_2, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE2, GPIO_MUX_5},	//IMISO
		{GPIOD, GPIO_PINS_4, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE4, GPIO_MUX_6},	//IMOSI
		{GPIOD, GPIO_PINS_0, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,  GPIO_MUX_NULL}, //(PD0) Test-ISP2 OK
	};
	const spi_pack_t spi4_pack_f437 = {
		//ethernet f437 spi4(crm, gpio)
		"AT32F437 SPI4 ETHERNET, sck/mi/mo/cs pe2/pe5/pe6/pe4",
		{"SPI4", SPI4, CRM_SPI4_PERIPH_CLOCK},
		{GPIOE, GPIO_PINS_2, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE2, GPIO_MUX_5},   //ISCK
		{GPIOE, GPIO_PINS_5, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE5, GPIO_MUX_5},	//IMISO
		{GPIOE, GPIO_PINS_6, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE6, GPIO_MUX_5},	//IMOSI
		{GPIOE, GPIO_PINS_4, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,  GPIO_MUX_NULL}, //(PE4) Test-ISP4 OK
	};
	const spi_pack_t spi1_pack_f437 = {
		//
		//ethernet f437 spi1(crm, gpio)
		"AT32F437 SPI1 ETHERNET, sck/mi/mo/cs pa5/pa6/pa7/pa15",
		{"SPI1", SPI1, CRM_SPI1_PERIPH_CLOCK},
		{GPIOA, GPIO_PINS_5, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE5, GPIO_MUX_5},   //ISCK
		{GPIOA, GPIO_PINS_6, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE6, GPIO_MUX_5},	//IMISO
		{GPIOA, GPIO_PINS_7, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE7, GPIO_MUX_5},	//IMOSI
		{GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,  GPIO_MUX_NULL}, //(PA15) Test-ISP1 OK
		//
		//ethernet f437 spi1(crm, gpio)
		"AT32F437 SPI1 ETHERNET, sck/mi/mo/cs pe13/pe14/pe15/pe12",
		{"SPI1", SPI1, CRM_SPI1_PERIPH_CLOCK},
		{GPIOE, GPIO_PINS_13, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE13, GPIO_MUX_4},   // o4 x5 x6 ISCK
		{GPIOE, GPIO_PINS_14, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	 GPIO_PINS_SOURCE14, GPIO_MUX_4},	// o4 x5 IMISO
		{GPIOE, GPIO_PINS_15, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	 GPIO_PINS_SOURCE15, GPIO_MUX_4},	// o4 x6 IMOSI
		{GPIOE, GPIO_PINS_12, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,   GPIO_MUX_NULL}, //(PE12) Test-ISP1 TEST
		//
	};
	#endif
	*/
#endif
	
#if 0
const static gp_set_t pb8_rst_gpio_set = { //(crm, gpio)
	"AT32 RST PAD, gpio pb8",
	{GPIOB, GPIO_PINS_8,  CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, 	GPIO_PINSRC_NULL, GPIO_MUX_NULL,} //(PB8) RST-pin
};
//.const static common_rst_t common_rst = {
//.	&pb8_rst_gpio_set,
//.};
#endif

#if 0 //[not used this fieldes form which is "scfg_st".]
#if !defined(AT32F437xx)
const static struct extscfg_st pc7_intr = {
		"enable SCFG, extline pc7",
		{ CRM_GPIOC_PERIPH_CLOCK, EXINT_LINE_7, EXINT9_5_IRQn}, // correspond to and with PC7
		"AT32 SCFG, exint pc7",
		{ CRM_IOMUX_PERIPH_CLOCK, GPIO_PORT_SOURCE_GPIOC, GPIO_PINS_SOURCE7},
		//NEXT_NULL,
};
#endif
#endif

/* User_data 2
 */
 
#if 0
gp_set_t gp = {
	"AT32 INT PAD, gpio pc7",
	{GPIOC, GPIO_PINS_7,  CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_INPUT, 	GPIO_PINSRC_NULL, GPIO_MUX_NULL,}, //(PC7) INT-pin
};

struct extscfg_st pe = { //struct linescfg_st 
	"enable SCFG, extline pc7",
	{ CRM_GPIOC_PERIPH_CLOCK, EXINT_LINE_7, EXINT9_5_IRQn}, // correspond to and with PC7
};
struct modscfg_st pdn = {
	"AT32 SCFG, exint pc7",
	{ CRM_IOMUX_PERIPH_CLOCK, GPIO_PORT_SOURCE_GPIOC, GPIO_PINS_SOURCE7},
	&pe, //essential
	NULL,
};
struct modscfg_st pde = {
	"AT32 SCFG, exint pc7",
	{ CRM_IOMUX_PERIPH_CLOCK, GPIO_PORT_SOURCE_GPIOC, GPIO_PINS_SOURCE7},
	&pe, //essential
	&gp,
};
#endif

/*
//.!AT32F437xx
typedef struct ext_gpio_st {
	crm_periph_clock_type scfg_clk;
	gpio_port_source_type scfg_port_src;
	gpio_pins_source_type scfg_pin_src;
} ext_gpio_t;

//.AT32F437xx
typedef struct ext_scfg_st {
	crm_periph_clock_type scfg_clk;
	scfg_port_source_type scfg_port_src; //SCFG_PORT_SOURCE_X
	scfg_pins_source_type scfg_pin_src; //SCFG_PINS_SOURCEX
} ext_scfg_t;
*/	

//typedef struct gpio_set_st {
//	const gpio_t wire_descript;
//} gpio_set_t;

//.typedef struct {
//.  const gp_set_t *rst_gpio; //.  const gpio_set_t *rst_gpio_set;
//.} common_rst_t;
/* Single-SPI interface f413/f437
 */
 
/*#define board_conf_type "\"dm9051_at32_cf\""
const void *intr_pack = NULL;
const gp_set_t *common_rst = NULL; //&_pb8_rst_gpio_set;

#define devconf	devconf_XXX
const static spi_dev_t devconf_XXX = {
#if !defined(AT32F437xx)
	//ethernet f413
	//ethernet f413 spi2(crm, gpio)
	"AT32F413 SPI2 ETHERNET, sck/mi/mo/cs pb13/pb14/pb15/pb12",
	{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK},
	{GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,    GPIO_PINSRC_NULL, GPIO_MUX_NULL},  //ISCK
	{GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMISO
	{GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMOSI
	{GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PB12) Test-ISP2 OK
#elif defined(AT32F437xx)
	//ethernet f437 spi2(crm, gpio)
	//"AT32F437 SPI2 ETHERNET, sck/mi/mo/cs pd1/pc2/pd4/pd0",
	//{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK},
	//{GPIOD, GPIO_PINS_1, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE1, GPIO_MUX_6},   //ISCK
	//{GPIOC, GPIO_PINS_2, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE2, GPIO_MUX_5},	//IMISO
	//{GPIOD, GPIO_PINS_4, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE4, GPIO_MUX_6},	//IMOSI
	//{GPIOD, GPIO_PINS_0, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,  GPIO_MUX_NULL}, //(PD0) Test-ISP2 OK
	//ethernet f437 spi4(crm, gpio)
	"AT32F437 SPI4 ETHERNET, sck/mi/mo/cs pe2/pe5/pe6/pe4",
	{"SPI4", SPI4, CRM_SPI4_PERIPH_CLOCK},
	{GPIOE, GPIO_PINS_2, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE2, GPIO_MUX_5}, //ISCK
	{GPIOE, GPIO_PINS_5, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE5, GPIO_MUX_5},	//IMISO
	{GPIOE, GPIO_PINS_6, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE6, GPIO_MUX_5},	//IMOSI
	{GPIOE, GPIO_PINS_4, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,  GPIO_MUX_NULL}, //(PE4) Test-ISP4 OK
#endif
};

const optsex_t _dm9051optsex = {
	DM_FALSE,
	MBNDRY_WORD, "MBNDRY_WORD",
	CS_EACH, "CS_EACH_MODE",
	0, //phy27
	0, "NCR Normal Mode", //vs. 1, "NCR_Test_mode"
	0, "Normal RX Mode", //vs. 1, "Promiscous_RX_mode"
};*/

#if HELLO_DRIVER_INTERNAL

/* Sanity3.
 */
#ifndef AT32F437xx
/**
  * @purpose make sure to have 'gpio_mux_sel_type'
  * @brief  gpio muxing function selection type (ARTRRY start to support found in AT32F437)
  * @brief  define to declare while if system header files not defined.
  */
typedef enum
{
  GPIO_MUX_0                             	= 0x00, /*!< gpio muxing function selection 0 */
  GPIO_MUX_1                             	= 0x01, /*!< gpio muxing function selection 1 */
  GPIO_MUX_2                             	= 0x02, /*!< gpio muxing function selection 2 */
  GPIO_MUX_3                             	= 0x03, /*!< gpio muxing function selection 3 */
  GPIO_MUX_4                             	= 0x04, /*!< gpio muxing function selection 4 */
  GPIO_MUX_5                             	= 0x05, /*!< gpio muxing function selection 5 */
  GPIO_MUX_6                             	= 0x06, /*!< gpio muxing function selection 6 */
  GPIO_MUX_7                             	= 0x07, /*!< gpio muxing function selection 7 */
  GPIO_MUX_8                             	= 0x08, /*!< gpio muxing function selection 8 */
  GPIO_MUX_9                             	= 0x09, /*!< gpio muxing function selection 9 */
  GPIO_MUX_10                            	= 0x0A, /*!< gpio muxing function selection 10 */
  GPIO_MUX_11                            	= 0x0B, /*!< gpio muxing function selection 11 */
  GPIO_MUX_12                            	= 0x0C, /*!< gpio muxing function selection 12 */
  GPIO_MUX_13                            	= 0x0D, /*!< gpio muxing function selection 13 */
  GPIO_MUX_14                            	= 0x0E, /*!< gpio muxing function selection 14 */
  GPIO_MUX_15                            	= 0x0F  /*!< gpio muxing function selection 15 */
} gpio_mux_sel_type;
#endif

//typedef enum {
//} iomux_t;
#define  IO_MUX_NULL                    	((uint16_t)0x0000) /*!< subordinate  */
#define  IO_MUX_PINREMAP                 	((uint16_t)0x0001) /*!< subordinate mode a (such as f413)*/
#define  IO_MUX_GPIOMUX                 	((uint16_t)0x0002) /*!< subordinate mode b (such as f437) */
#define  IO_CRM_CLOCK                 		((uint16_t)0x0100) /*!< subordinate mode c (such as f413 clock) */

typedef struct spihead_sel_st {
  char *spi_name;
  spi_type *spi_num;        		//= SPIPORT;
  crm_periph_clock_type spi_crm_clk;	//= SPI_CRM_CLK;
  uint16_t iomux;
} spihead_t;

typedef struct gpio_sel_st {
	gpio_type *gpport;        		//= PORT;
	uint16_t pin;           		//= PIN;
	crm_periph_clock_type gpio_crm_clk;  //= CRM_CLK;	
	gpio_mode_type gpio_mode;		//= type
	gpio_pins_source_type pinsrc;
	gpio_mux_sel_type muxsel;
} gpio_t;

typedef struct {
	char *info;
	spihead_t spidef;
	char *cpu_spi_info;	//cpu name is for the purpose to lead the pins, for easy recogition!
	gpio_t wire_sck;
	gpio_t wire_mi;
	gpio_t wire_mo;
	char *cpu_cs_info;
	gpio_t wire_cs;
} spi_dev_t;

typedef struct extint_init_st {
	crm_periph_clock_type scfg_clk;
#if !defined(AT32F437xx)
	gpio_port_source_type scfg_port_src;
	gpio_pins_source_type scfg_pin_src;
#elif defined(AT32F437xx)
	scfg_port_source_type scfg_port_src; //SCFG_PORT_SOURCE_X
	scfg_pins_source_type scfg_pin_src; //SCFG_PINS_SOURCEX
#endif
} extint_init_t;

typedef struct extline_st {
	crm_periph_clock_type intr_crm_clk; //CRM_GPIOC_PERIPH_CLOCK,
	uint32_t extline; //= LINE
	IRQn_Type irqn; //= EXINTn_m
} extline_t;

struct extscfg_st { //struct linescfg_st
	const char *irq_enable_inf;	
	extline_t extline;
};

typedef struct gp_set_st {
	const char *gp_info;	
	const gpio_t gp;
} gp_set_t;

struct modscfg_st {
	const char *scfg_inf;	
	struct extint_init_st scfg_init; //extint_init_t
	struct extscfg_st *extend; //struct linescfg_st *extend; //essential
	struct gp_set_st *option; //gp_set_t
};

// -

typedef struct {
	//.uint8_t mac_addresse[MAC_ADDR_LENGTH];
	//.uint8_t phy27;
	//.char *set_name;
	//.enable_t test_plan_include; //whether run test_plan fnc!
	
//#define OPTS_FUNC_DATA
//#undef OPTS_FUNC_DATA
	#if 0
	/*#define DM_MACRO(rtype,field) \
		rtype field; \
		char *desc##field;*/
	#endif

#undef DM_TYPE
#define DM_TYPE		1
#include "dm_types.h"
	
} optsex_t;

/* Sanity2.
 */
#ifndef AT32F437xx
//#define scfg_exint_system_ready			0
//#define gpio_mux_sel_type_system_declaration	0
#else
//#define scfg_exint_system_ready			1
//#define gpio_mux_sel_type_system_declaration	1
#endif

/* Sanity5. def
 */
//#define NULL_CRMCLK (crm_periph_clock_type)	0
//#define NULL_PINSRC (gpio_pins_source_type)	0
//#define NULL_MUXSEL (gpio_mux_sel_type)		0

#define GPIO_PINSRC_NULL (gpio_pins_source_type) 0
#define GPIO_MUX_NULL    (gpio_mux_sel_type)	 0

#endif //HELLO_DRIVER_INTERNAL
