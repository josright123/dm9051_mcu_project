//
// dm9051_lw_conf_data.h //2.yc
// - spi configuration data
// - mac addresses data
//
#define board_conf_type	"\"dm9051_at32_mf\""

const void *intr_pack = NULL;

const gp_set_t *option_rst_common = NULL;

const spi_dev_t devconf[BOARD_SPI_COUNT] = {
	#ifdef AT32F437xx
	#define devconf_at437_spi2(info, spi_setting_name, cs_setting_name) \
		{ \
			info, \
			{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK}, \
			spi_setting_name, \
			{GPIOD, GPIO_PINS_1, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE1, GPIO_MUX_6},  /* //ISCK */ \
			{GPIOC, GPIO_PINS_2, CRM_GPIOC_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE2, GPIO_MUX_5},	/* //IMISO */ \
			{GPIOD, GPIO_PINS_4, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE4, GPIO_MUX_6},	/* //IMOSI */ \
			cs_setting_name, \
			{GPIOD, GPIO_PINS_0, CRM_GPIOD_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,  GPIO_MUX_NULL}, /* //(PD0) Test-ISP2 OK */ \
		}
	#define devconf_at437_spi4(info, spi_setting_name, cs_setting_name) \
		{ \
			info, \
			{"SPI4", SPI4, CRM_SPI4_PERIPH_CLOCK}, \
			spi_setting_name, \
			{GPIOE, GPIO_PINS_2, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE2, GPIO_MUX_5},   /* //ISCK */ \
			{GPIOE, GPIO_PINS_5, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE5, GPIO_MUX_5},	/* //IMISO */ \
			{GPIOE, GPIO_PINS_6, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE6, GPIO_MUX_5},	/* //IMOSI */ \
			cs_setting_name, \
			{GPIOE, GPIO_PINS_4, CRM_GPIOE_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,  GPIO_MUX_NULL}, /* //(PE4) Test-ISP4 OK */ \
		}
	#define devconf_at437_spi1(info, spi_setting_name, cs_setting_name) \
		{ \
			info, \
			{"SPI1", SPI1, CRM_SPI1_PERIPH_CLOCK}, \
			spi_setting_name, \
			{GPIOA, GPIO_PINS_5, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,     GPIO_PINS_SOURCE5, GPIO_MUX_5},   /* //ISCK */ \
			{GPIOA, GPIO_PINS_6, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE6, GPIO_MUX_5},	/* //IMISO */ \
			{GPIOA, GPIO_PINS_7, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_MUX,	  	GPIO_PINS_SOURCE7, GPIO_MUX_5},	/* //IMOSI */ \
			cs_setting_name, \
			{GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_OUTPUT,  GPIO_PINSRC_NULL,  GPIO_MUX_NULL}, /* //(PA15) Test-ISP1 OK */ \
		}
	devconf_at437_spi2("AT32F437 ETHERNET SPI2", "sck/mi/mo/ pd1/pc2/pd4", "cs/ pd0"),
	devconf_at437_spi2("AT32F437 ETHERNET SPI2", "sck/mi/mo/ pd1/pc2/pd4", "cs/ pd0"), //-
	devconf_at437_spi4("AT32F437 ETHERNET SPI4", "sck/mi/mo/ pe2/pe5/pe6", "cs/ pe4"),
	devconf_at437_spi2("AT32F437 ETHERNET SPI2", "sck/mi/mo/ pd1/pc2/pd4", "cs/ pd0"),
	devconf_at437_spi1("AT32F437 ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa15"),

	#elif defined (AT32F413xx)
	#define devconf_at413_spi2(info, spi_setting_name, cs_setting_name) \
		{ \
			info, \
			{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK}, \
			spi_setting_name, \
			{GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,    GPIO_PINSRC_NULL, GPIO_MUX_NULL},  /* //ISCK */ \
			{GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	/* //IMISO */ \
			{GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	/* //IMOSI */ \
			cs_setting_name, \
			{GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, /* //(PB12) Test-ISP2 OK */ \
			/* {GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, (PB12) Test-ISP2 OK */ \
		}
	#define devconf_at413_spi1(info, spi_setting_name, cs_setting_name, gpport, pin, gpio_crm_clk) \
		{ \
			info, \
			{"SPI1",	SPI1,			CRM_SPI1_PERIPH_CLOCK}, \
			spi_setting_name, \
			{GPIOA,		GPIO_PINS_5, 	CRM_GPIOA_PERIPH_CLOCK, 	GPIO_MODE_MUX, 		GPIO_PINSRC_NULL, GPIO_MUX_NULL},  /* //ISCK */ \
			{GPIOA,		GPIO_PINS_6, 	CRM_GPIOA_PERIPH_CLOCK, 	GPIO_MODE_INPUT,	GPIO_PINSRC_NULL, GPIO_MUX_NULL}, /* //IMISO */ \
			{GPIOA,		GPIO_PINS_7, 	CRM_GPIOA_PERIPH_CLOCK, 	GPIO_MODE_MUX,		GPIO_PINSRC_NULL, GPIO_MUX_NULL}, /* //IMOSI */ \
			cs_setting_name, \
			{gpport,	pin, 			gpio_crm_clk, 				GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, /* //(PA4) Test-ISP2 OK */ \
			/* {GPIOB,	GPIO_PINS_12,	CRM_GPIOB_PERIPH_CLOCK, 	GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, //(PB12) Test-ISP2 OK */ \
			/* {GPIOA,	GPIO_PINS_4,	CRM_GPIOA_PERIPH_CLOCK, 	GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, (PA4) Test-ISP2 OK */ \
		}
	devconf_at413_spi2("AT32F413 ETHERNET SPI2", "sck/mi/mo/ pb13/pb14/pb15", "cs/ pb12"),
	devconf_at413_spi1("AT32F413 ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pB12", GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK),
	devconf_at413_spi1("AT32F413 ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa4", GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK),

	#elif defined (AT32F403Axx)
	#define devconf_at403a_spi2(info, spi_setting_name, cs_setting_name) \
		{ \
			info, \
			{"SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK}, \
			spi_setting_name, \
			{GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,    GPIO_PINSRC_NULL, GPIO_MUX_NULL},  /* //ISCK */ \
			{GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	/* //IMISO */ \
			{GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL},	/* //IMOSI */ \
			cs_setting_name, \
			{GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, /* //(PB12) Test-ISP2 OK */ \
			/*{GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK, 				GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL},*/ /* //(PA15) Test-ISP2 OK */ \
		}
	#define devconf_at403a_spi1(info, spi_setting_name, cs_setting_name, gpport, pin, gpio_crm_clk) \
		{ \
			info, \
			{"SPI1",	SPI1,			CRM_SPI1_PERIPH_CLOCK}, \
			spi_setting_name, \
			{GPIOA,		GPIO_PINS_5, 	CRM_GPIOA_PERIPH_CLOCK, 	GPIO_MODE_MUX, 	GPIO_PINSRC_NULL, GPIO_MUX_NULL},  /* //ISCK */ \
			{GPIOA,		GPIO_PINS_6, 	CRM_GPIOA_PERIPH_CLOCK, 	GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL}, /* //IMISO */ \
			{GPIOA,		GPIO_PINS_7, 	CRM_GPIOA_PERIPH_CLOCK, 	GPIO_MODE_MUX,	GPIO_PINSRC_NULL, GPIO_MUX_NULL}, /* //IMOSI */ \
			cs_setting_name, \
			{gpport,	pin, 			gpio_crm_clk, 				GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}, /* //(PA4) Test-ISP2 OK */ \
		}
	devconf_at403a_spi2("AT32F403A ETHERNET SPI2", "sck/mi/mo/ pb13/pb14/pb15", "cs/ pb12"),
	devconf_at403a_spi1("AT32F403A ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa4", GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK),
	// devconf_at403a_spi1("AT32F403A ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa15", /*GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK*/ GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK),
	#else
	#error "not defined board"
	#endif

	
};

optsex_t dm9051optsex[BOARD_SPI_COUNT] = { //const 
	#define dmopts_normal(iomode, iomode_name) \
		{ \
			/* .set_name */ \
			iomode_name, \
			/* .test_plan_include */ \
			FALSE, \
			/* .test_plan_log */ \
			TRUE, /*FALSE,*/ \
			/* //vs MBNDRY_BYTE, "8-bit",/ MBNDRY_WORD, "16-bit",*/ \
			iomode, iomode_name, \
			/* //vs CS_EACH, "CS_EACH_MODE",/ CS_LONG, "CS_LONG_MODE",*/ \
			CS_EACH, "CS_EACH_MODE", \
			/* //phy27 */ \
			/* 0, */ \
			/* //vs. NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode",/ NCR_FORCE_100MF, "NCR_Force_100MF_mode"/ NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode"*/ \
			NCR_FORCE_100MF, "NCR_Force_100MF_mode", /*NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode",*/ /*NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode",*/ \
			/* //vs. 0, "Normal RX Mode",/ 1, "RX_Promiscuos_mode"*/ \
			0, "Normal RX Mode", \
			/* //vs. FALSE, "Checksum offload disable",/ TRUE, "checksum offload enable",*/ \
			TRUE, "rxmode checksum offload enable", \
			/* //vs. FALSE, "Flow control disable",/ TRUE, "Flow control enable",*/ \
			TRUE, "Flow control enable", \
		}
	#define dmopts_normal_1(iomode, iomode_name) \
		{ \
			/* .set_name */ \
			iomode_name, \
			/* .test_plan_include */ \
			FALSE, \
			/* //.test_plan_log */ \
			FALSE, \
			/* //vs MBNDRY_BYTE, "8-bit",/ MBNDRY_WORD, "16-bit",*/ \
			iomode, iomode_name, \
			/* //vs CS_EACH, "CS_EACH_MODE",/ CS_LONG, "CS_LONG_MODE", */ \
			CS_EACH, "CS_EACH_MODE", /*CS_EACH, "CS_EACH_MODE",*/ \
			/* 0, */ /* //phy27 */ \
			/* //vs. NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode",/ NCR_FORCE_100MF, "NCR_Force_100MF_mode"/ NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode" */ \
			NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode", /*NCR_FORCE_100MF, "NCR_Force_100MF_mode",*/ \
			/* //vs. 0, "Normal RX Mode",/ 1, "RX_Promiscuos_mode" */ \
			0, "Normal RX Mode", \
			/* //vs. FALSE, "Checksum offload disable",/ TRUE, "checksum offload enable", */ \
			FALSE, "Checksum offload disable", \
			/* //vs. FALSE, "Flow control disable",/ TRUE, "Flow control enable", */ \
			TRUE, "Flow control enable", \
		}
	#define dmopts_test1(iomode, iomode_name) \
		{ \
			/* .set_name */ \
			iomode_name, \
			/* .test_plan_include */ \
			FALSE, \
			/* //.test_plan_log */ \
			FALSE, \
			/* //vs MBNDRY_BYTE, "8-bit",/ MBNDRY_WORD, "16-bit",*/ \
			iomode, iomode_name, \
			/* //vs CS_EACH, "CS_EACH_MODE",/ CS_LONG, "CS_LONG_MODE", */ \
			CS_EACH, "CS_EASH_MODE", \
			/* 0, */ /* //phy27 */ \
			/* //vs. NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode",/ NCR_FORCE_100MF, "NCR_Force_100MF_mode"/ NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode" */ \
			NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode", \
			/* //vs. 0, "Normal RX Mode",/ 1, "RX_Promiscuos_mode" */ \
			0, "Normal RX Mode", \
			/* //vs. FALSE, "Checksum offload disable",/ TRUE, "checksum offload enable", */ \
			TRUE, "rxmode checksum offload enable", \
			/* //vs. FALSE, "Flow control disable",/ TRUE, "Flow control enable", */ \
			TRUE, "Flow control enable", \
		}
	dmopts_normal_1(MBNDRY_BYTE, "8-bit"), //(MBNDRY_BYTE, "8-bit mode"),
	dmopts_normal(MBNDRY_BYTE, "8-bit"),
	dmopts_test1(MBNDRY_WORD, "16-bit mode"),
};

//(define is as rather than 'ETHERNET_COUNT', refer to as 'ETHERNET_COUNT' counter)
const uint8_t mac_addresse[BOARD_SPI_COUNT][6] = { \
	{0, 0x60, 0x6e, 0x00, 0x01, 0x26,}, \
	{0, 0x60, 0x6e, 0x00, 0x01, 0x25,}, \
	{0, 0x60, 0x6e, 0x00, 0x01, 0xff,}, \
	{0, 0x60, 0x6e, 0x00, 0x01, 0xff,}, \
	{0, 0x60, 0x6e, 0x00, 0x01, 0xff,}, \
};

//
// 'pin_code' always 0. when ETHERNET_COUNT==1, but _BOARD_SPI_COUNT > 1.
//
int pin_code = 0;

DECL_SG_FUNCTION(confirm_state, test_plan_include)
DECL_SG_FUNCTION(confirm_state, test_plan_log)

DECL_FUNCTION(uint8_t, iomode)
DECL_FUNCTION(uint8_t, promismode)

DECL_FUNCTION(csmode_t, csmode)
DECL_FUNCTION(ncrmode_t, ncrmode)

DECL_FUNCTION(confirm_state, rxmode_checksum_offload)
DECL_FUNCTION(confirm_state, flowcontrolmode)

//[common.macro]
#define info_conf_name()			FIELD_SPIDEV(info)
#define cpu_spi_conf_name()			FIELD_SPIDEV(cpu_spi_info) //devconf[pin_code].cpu_api_info
#define cpu_cs_conf_name()			FIELD_SPIDEV(cpu_cs_info)
#define spihead()					FIELD_SPIDEV(spidef)
#define gpio_wire_sck()				FIELD_SPIDEV(wire_sck)
#define gpio_wire_mi()				FIELD_SPIDEV(wire_mi)
#define gpio_wire_mo()				FIELD_SPIDEV(wire_mo)
#define gpio_cs()					FIELD_SPIDEV(wire_cs)

#define spi_number()				FIELD_SPIDEV(spidef.spi_num) //spihead().spi_num //= spi_no()
#define spi_crm()					FIELD_SPIDEV(spidef.spi_crm_clk) //spihead().spi_crm_clk
#define spi_conf_name()				FIELD_SPIDEV(spidef.spi_name) //spihead().spi_name

#define exint_exister()				((struct modscfg_st *)intr_pack)
#define exint_data()				((struct modscfg_st *)intr_pack)
#define exint_scfg_ptr()			!exint_data() ? NULL : ((struct modscfg_st *)intr_pack)->extend
#define intr_gpio_exister()			!exint_data() ? 0 : !(((struct modscfg_st *)intr_pack)->option) ? 0 : 1
#define scfg_info()					PTR_EXINTD(scfg_inf)
#define scfg_crm()					PTR_EXINTD(scfg_init.scfg_clk)
#define scfg_port()					PTR_EXINTD(scfg_init.scfg_port_src)
#define scfg_pin()					PTR_EXINTD(scfg_init.scfg_pin_src)
#define intr_data_scfg()			PTR_EXINTD(extend)
#define exint_enable_info()			PTR_EXINTD(extend->irq_enable_inf)
#define exint_extline()				PTR_EXINTD(extend->extline.extline)
#define exint_crm()					PTR_EXINTD(extend->extline.intr_crm_clk)
#define intr_gpio_data()			PTR_EXINTD(option)

#define rst_gpio_data()				(option_rst_common)
#define rst_gpio_exister()			(rst_gpio_data() ? 1 : 0)

#define intr_gpio_info()			PTR_EXINTD(option->gp_info)
#define intr_gpio_ptr()				((const gpio_t *)(&PTR_EXINTD(option->gp)))

#define rst_gpio_info()				PTR_RSTGPIO(gp_info)
#define rst_gpio_ptr()				((const gpio_t *)(&PTR_RSTGPIO(gp)))

#define mstep_set_index(i)			pin_code = i //empty for 1 eth project
#define mstep_get_index()			pin_code
#define mstep_turn_net_index()		//empty for 1 eth project

//[common.mac]
#define get_eth_mac()				&mac_addresse[pin_code][0]

//-

void mstep_set_net_index(int i)
{
	mstep_set_index(i); //pinCode = i;
}

int mstep_get_net_index(void)
{
	return mstep_get_index();
}

void mstep_next_net_index(void)
{
	mstep_turn_net_index();
}

char *mstep_spi_conf_name(void)
{
	return spi_conf_name();
}

char *mstep_conf_info(void) {
	return info_conf_name();
}

char *mstep_conf_cpu_spi_ethernet(void) {
	return cpu_spi_conf_name();
}

char *mstep_conf_cpu_cs_ethernet(void) {
	return cpu_cs_conf_name();
}

char *mstep_conf_type(void)
{
	return board_conf_type;
}

const uint8_t *mstep_eth_mac(void)
{
	return get_eth_mac();
}

//-

int is_dm9051_board_irq(void)
{
	const struct extscfg_st *pexint_set = (const struct extscfg_st *) exint_scfg_ptr();
	return pexint_set ? 1 : 0;
}

int intr_gpio_exist(void) {
	return intr_gpio_exister();
}

static int rst_pin_exister(void) {
	return rst_gpio_exister();
}

static int intr_gpio_mptr(void) {
	if (intr_gpio_exist()) {
		printf(": %s :                 intr-pin/ %s\r\n", "config", intr_gpio_info()); //_intr_gpio_exist()->gp_info
		return 1;
	}
	return 0;
}

static int rst_pin_mexist(void) {
	if (rst_pin_exister()) {
		printf(": %s :                 rst-pin/ %s\r\n", "config", rst_gpio_info());
		return 1;
	}
	return 0;
}
