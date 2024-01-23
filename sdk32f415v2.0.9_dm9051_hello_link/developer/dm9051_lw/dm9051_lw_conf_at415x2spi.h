typedef struct spihead_sel_st {
  char *spi_name;
  spi_type *spi_num;        		//= SPIPORT;
  crm_periph_clock_type spi_crm_clk;	//= SPI_CRM_CLK;
} spihead_t;

typedef struct gpio_sel_st {
	gpio_type *gpport;        		//= PORT;
	uint16_t pin;           		//= PIN;
	crm_periph_clock_type gpio_crm_clk;  //= CRM_CLK;	
	gpio_mode_type gpio_mode;		//= type
	gpio_pins_source_type pinsrc;
	gpio_mux_sel_type muxsel;
} gpio_t;

typedef struct gp_set_st {
	const char *gp_info;	
	const gpio_t gp;
} gp_set_t;

typedef struct extline_st {
	crm_periph_clock_type intr_crm_clk; //CRM_GPIOC_PERIPH_CLOCK,
	uint32_t extline; //= LINE
	IRQn_Type irqn; //= EXINTn_m
} extline_t;

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

typedef struct {
	char *cpu_api_info;	//cpu name is for the purpose to lead the pins, for easy recogition!
	spihead_t spidef;
	gpio_t wire_sck;
	gpio_t wire_mi;
	gpio_t wire_mo;
	gpio_t wire_cs;
} spi_dev_t;

struct extscfg_st { //struct linescfg_st
	const char *irq_enable_inf;	
	extline_t extline;
};

struct modscfg_st {
	const char *scfg_inf;	
	extint_init_t scfg_init;
	struct extscfg_st *extend; //struct linescfg_st *extend; //essential
	gp_set_t *option;
};

#if (BOARD_SPI_COUNT >= 2)
#define board_conf_type "\"dm9051_at32_mf\""
//1.const void *intr_pack = pdn; (without gpio)
//2.const void *intr_pack = pde; (with gpio)
//3.const void *intr_pack = NULL;
const void *intr_pack = NULL;
//1.const gp_set_t *common_rst = NULL;
const gp_set_t *common_rst = NULL; //&pb8_rst_gpio_set;

#define devconf	devconf_XXX
const static spi_dev_t devconf_XXX[BOARD_SPI_COUNT] = {
	{
		//ethernet f413
		//ethernet f413 spi2(crm, gpio)
		"AT32F413 SPI2 ETHERNET, sck/mi/mo/cs pb13/pb14/pb15/pb12",
		{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK},
		{GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,    GPIO_PINSRC_NULL, GPIO_MUX_NULL},  //ISCK
		{GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMISO
		{GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMOSI
		
		{GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PB12) Test-ISP2 OK
		//{GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PB12) Test-ISP2 OK
	},
	{
		//ethernet f413
		//ethernet f413 spi2(crm, gpio)
		
		//"AT32F413 SPI1 ETHERNET, sck/mi/mo/cs pb2/pb0/pc4/pa6",
		"AT32F413 SPI1 ETHERNET, sck/mi/mo/cs pa5/pa6/pa7/pa4",
		
		{"SPI1", SPI1, CRM_SPI1_PERIPH_CLOCK},
		
		//{GPIOB, GPIO_PINS_2, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX, GPIO_PINSRC_NULL, GPIO_MUX_NULL},  //ISCK
		//{GPIOB, GPIO_PINS_0, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMISO
		//{GPIOC, GPIO_PINS_4, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMOSI
		
		//{GPIOA, GPIO_PINS_6, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PB12) Test-ISP2 OK
		
		{GPIOA, GPIO_PINS_5, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX, GPIO_PINSRC_NULL, GPIO_MUX_NULL},  //ISCK
		//{GPIOA, GPIO_PINS_6, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMISO
		{GPIOA, GPIO_PINS_6, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_INPUT,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMISO
		{GPIOA, GPIO_PINS_7, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	//IMOSI
		
		{GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PB12) Test-ISP2 OK
		//{GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PB12) Test-ISP2 OK
	},
	/*
	//ethernet f437
	//ethernet f437 spi2(crm, gpio)
	"AT32F437 SPI2 ETHERNET, sck/mi/mo/cs pd1/pc2/pd4/pd0",
	{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK},
	{GPIOD, GPIO_PINS_1, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE1, GPIO_MUX_6},   //ISCK
	{GPIOC, GPIO_PINS_2, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE2, GPIO_MUX_5},	//IMISO
	{GPIOD, GPIO_PINS_4, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,	  GPIO_PINS_SOURCE4, GPIO_MUX_6},	//IMOSI
	{GPIOD, GPIO_PINS_0, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PD0) Test-ISP2 OK
	*/
};

//[common.macro]
int pin_code;
//#define spi_dev()					&devconf
#define gpio_wire_sck()				devconf[pin_code].wire_sck 
#define gpio_wire_mi()				devconf[pin_code].wire_mi
#define gpio_wire_mo()				devconf[pin_code].wire_mo
#define gpio_cs()					devconf[pin_code].wire_cs
#define spihead()					devconf[pin_code].spidef
#define intr_data()					((struct modscfg_st *)intr_pack) //((intr_param_t *)intr_pack)
#define intr_data_scfg()			(((struct modscfg_st *)intr_pack)->extend) //(((intr_param_t *)intr_pack)->scfg)

#define intr_gpio_data()			((gp_set_t *)intr_data()->option)
#define intr_gpio_exister()			(intr_pack) ? intr_data()->option ? 1 : 0 : 0 //(!intr_pack) ? 0 : (intr_data()->scfg.next_avail == NEXT_AVAILABLE) ? 1 : 0

#define exint_scfg_exister()		(intr_pack)
#define exint_scfg_ptr()			(!intr_pack) ? NULL : intr_data()->extend //(!intr_pack) ? NULL : &(intr_data()->scfg)
#define scfg_info()					(intr_data()->scfg_inf)
#define scfg_crm()					(intr_data()->scfg_init.scfg_clk)
#define scfg_port()					(intr_data()->scfg_init.scfg_port_src)
#define scfg_pin()					(intr_data()->scfg_init.scfg_pin_src)
#define rst_gpio_data()				(common_rst)

#define cpuhead()					devconf[pin_code]
#define cpu_spi_conf_name()			cpuhead().cpu_api_info
#define spi_number()				spihead().spi_num //= spi_no()
#define spi_crm()					spihead().spi_crm_clk
#define spi_conf_name()				spihead().spi_name

#define exint_enable_info()			(intr_data_scfg()->irq_enable_inf)
#define exint_extline()				(intr_data_scfg()->extline.extline)
#define exint_crm()					(intr_data_scfg()->extline.intr_crm_clk)
#define intr_gpio_info()			(intr_gpio_data()->gp_info)
#define intr_gpio_ptr()				((const gpio_t *)(&intr_gpio_data()->gp))

#define rst_gpio_info()				(rst_gpio_data()->gp_info)
#define rst_gpio_ptr()				((const gpio_t *)(&rst_gpio_data()->gp))
#define rst_gpio_exister()			(rst_gpio_data() ? 1 : 0)
//#define rst_gpio_info()			(intr_gpio_data()->gp_info)
//#define rst_gpio_ptr()			&(intr_gpio_data()->gp)

#define mstep_set_index(i)			pin_code = i //empty for 1 eth project
#define mstep_turn_net_index()		//empty for 1 eth project

//-
#define mstep_get_index()			pin_code
//#define _mstep_get_net_index()	pin_code
#endif
