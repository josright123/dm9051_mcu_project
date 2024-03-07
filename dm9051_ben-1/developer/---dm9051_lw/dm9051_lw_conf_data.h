//
// dm9051_lw_conf_data.h //2.yc
// - spi configuration data
// - mac addresses data
//
#define board_conf_type	"\"dm9051_at32_mf\""

const void *intr_pack = NULL;

const gp_set_t *option_rst_common = NULL;

#define SPI_PINSTD(spiname,spinum,crm_clk,iom)	{spiname,spinum,crm_clk, iom}
#define GPIO_PINNORM(gpport,pin,crm_clk)			{gpport,pin,crm_clk, GPIO_MODE_MUX, GPIO_PINSRC_NULL, GPIO_MUX_NULL}
#define GPIO_PINMUX(gpport,pin,crm_clk,pinsrc,mux)	{gpport,pin,crm_clk, GPIO_MODE_MUX, pinsrc,mux}
#define GPIO_PINOUT(gpport,pin,crm_clk)		{gpport,pin,crm_clk, GPIO_MODE_OUTPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}
	
const spi_dev_t devconf[BOARD_SPI_COUNT] = {
	#ifdef AT32F437xx
		//AT32F437xx
		#define devconf_at437_spi2(info, spi_setting_name, cs_setting_name) \
			{ \
				info, \
				SPI_PINSTD("SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK, IO_MUX_NULL), \
				spi_setting_name, \
				GPIO_PINMUX(GPIOD, GPIO_PINS_1, CRM_GPIOD_PERIPH_CLOCK, GPIO_PINS_SOURCE1, GPIO_MUX_6),  /* //SCK */ \
				GPIO_PINMUX(GPIOC, GPIO_PINS_2, CRM_GPIOC_PERIPH_CLOCK, GPIO_PINS_SOURCE2, GPIO_MUX_5),	/* //MISO */ \
				GPIO_PINMUX(GPIOD, GPIO_PINS_4, CRM_GPIOD_PERIPH_CLOCK, GPIO_PINS_SOURCE4, GPIO_MUX_6),	/* //MOSI */ \
				cs_setting_name, \
				GPIO_PINOUT(GPIOD, GPIO_PINS_0, CRM_GPIOD_PERIPH_CLOCK), /* //(PD0) */ \
			}
		#define devconf_at437_spi4(info, spi_setting_name, cs_setting_name) \
			{ \
				info, \
				SPI_PINSTD("SPI4", SPI4, CRM_SPI4_PERIPH_CLOCK, IO_MUX_NULL), \
				spi_setting_name, \
				GPIO_PINMUX(GPIOE, GPIO_PINS_2, CRM_GPIOE_PERIPH_CLOCK, GPIO_PINS_SOURCE2, GPIO_MUX_5), /* //SCK */ \
				GPIO_PINMUX(GPIOE, GPIO_PINS_5, CRM_GPIOE_PERIPH_CLOCK, GPIO_PINS_SOURCE5, GPIO_MUX_5),	/* //MISO */ \
				GPIO_PINMUX(GPIOE, GPIO_PINS_6, CRM_GPIOE_PERIPH_CLOCK, GPIO_PINS_SOURCE6, GPIO_MUX_5),	/* //MOSI */ \
				cs_setting_name, \
				GPIO_PINOUT(GPIOE, GPIO_PINS_4, CRM_GPIOE_PERIPH_CLOCK), /* //(PE4) */ \
			}
		#define devconf_at437_spi1(info, spi_setting_name, cs_setting_name) \
			{ \
				info, \
				SPI_PINSTD("SPI1", SPI1, CRM_SPI1_PERIPH_CLOCK, IO_MUX_NULL), \
				spi_setting_name, \
				GPIO_PINMUX(GPIOA, GPIO_PINS_5, CRM_GPIOA_PERIPH_CLOCK, GPIO_PINS_SOURCE5, GPIO_MUX_5), /* //SCK */ \
				GPIO_PINMUX(GPIOA, GPIO_PINS_6, CRM_GPIOA_PERIPH_CLOCK, GPIO_PINS_SOURCE6, GPIO_MUX_5),	/* //MISO */ \
				GPIO_PINMUX(GPIOA, GPIO_PINS_7, CRM_GPIOA_PERIPH_CLOCK, GPIO_PINS_SOURCE7, GPIO_MUX_5),	/* //MOSI */ \
				cs_setting_name, \
				GPIO_PINOUT(GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK), /* //(PA15) */ \
			}
		devconf_at437_spi2("AT32F437 ETHERNET SPI2", "sck/mi/mo/ pd1/pc2/pd4", "cs/ pd0"),
		devconf_at437_spi4("AT32F437 ETHERNET SPI4", "sck/mi/mo/ pe2/pe5/pe6", "cs/ pe4"),
		devconf_at437_spi2("AT32F437 ETHERNET SPI2", "sck/mi/mo/ pd1/pc2/pd4", "cs/ pd0"), //-
		devconf_at437_spi2("AT32F437 ETHERNET SPI2", "sck/mi/mo/ pd1/pc2/pd4", "cs/ pd0"),
		devconf_at437_spi1("AT32F437 ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa15"),
	#elif defined (AT32F413xx) || defined (AT32F415xx)
		//(AT32F413/415)
		#define GPIO_PININ(gpport,pin,crm_clk)		{gpport,pin,crm_clk, GPIO_MODE_INPUT, GPIO_PINSRC_NULL, GPIO_MUX_NULL}
		#define devconf_at413_spi2_0(info, spi_setting_name, cs_setting_name) \
			{ \
				info, \
				SPI_PINSTD("SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK, IO_MUX_NULL), \
				spi_setting_name, \
				GPIO_PINNORM(GPIOB, GPIO_PINS_13, CRM_GPIOB_PERIPH_CLOCK), /* //SCK */ \
				GPIO_PINNORM(GPIOB, GPIO_PINS_14, CRM_GPIOB_PERIPH_CLOCK), /* //MISO */ \
				GPIO_PINNORM(GPIOB, GPIO_PINS_15, CRM_GPIOB_PERIPH_CLOCK), /* //MOSI */ \
				cs_setting_name, \
				GPIO_PINOUT(GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK), /* //(PB12) */ \
				/* GPIO_PINOUT(GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK), (PB12) */ \
			}
		#define devconf_at413_spi1_0(info, spi_setting_name, cs_setting_name, gp_port, gp_pin, gp_crm_clk) \
			{ \
				info, \
				SPI_PINSTD("SPI1", SPI1, CRM_SPI1_PERIPH_CLOCK, IO_MUX_NULL), \
				spi_setting_name, \
				GPIO_PINNORM(GPIOB,		GPIO_PINS_3, 	CRM_GPIOB_PERIPH_CLOCK), /* //SCK */ \
				GPIO_PININ(GPIOB,		GPIO_PINS_4, 	CRM_GPIOB_PERIPH_CLOCK), /* //MISO */ \
				GPIO_PINNORM(GPIOB,		GPIO_PINS_5, 	CRM_GPIOB_PERIPH_CLOCK), /* //MOSI */ \
				cs_setting_name, \
				GPIO_PINOUT(gp_port,	gp_pin, 		gp_crm_clk), /* //(PA4) */ \
				/* GPIO_PINOUT(GPIOB,	GPIO_PINS_12,	CRM_GPIOB_PERIPH_CLOCK), (PB12) */ \
				/* GPIO_PINOUT(GPIOA,	GPIO_PINS_4,	CRM_GPIOA_PERIPH_CLOCK), (PA4) */ \
			}

		//AT32F413/415
		#define devconf_at413_spi2(info, spi_setting_name, cs_setting_name) { \
				info, \
				SPI_PINSTD("SPI2", SPI2, CRM_SPI2_PERIPH_CLOCK, IO_MUX_NULL), \
				spi_setting_name, \
				GPIO_PINNORM(GPIOB,		GPIO_PINS_13,	CRM_GPIOB_PERIPH_CLOCK),  /* //SCK */ \
				GPIO_PINNORM(GPIOB,		GPIO_PINS_14,	CRM_GPIOB_PERIPH_CLOCK),	/* //MISO */ \
				GPIO_PINNORM(GPIOB,		GPIO_PINS_15,	CRM_GPIOB_PERIPH_CLOCK),	/* //MOSI */ \
				cs_setting_name, \
				GPIO_PINOUT(GPIOB,	GPIO_PINS_12,	CRM_GPIOB_PERIPH_CLOCK), /* //(PB12) */ \
			}
		#define devconf_at413_spi1a(info, spi_setting_name, cs_setting_name, gp_port, gp_pin, gp_crm_clk, iom) { \
				info, \
				SPI_PINSTD("SPI1", SPI1, CRM_SPI1_PERIPH_CLOCK, iom), \
				spi_setting_name, \
				GPIO_PINNORM(GPIOA,		GPIO_PINS_5,	CRM_GPIOA_PERIPH_CLOCK),  /* //SCK */ \
				GPIO_PININ(GPIOA,		GPIO_PINS_6,	CRM_GPIOA_PERIPH_CLOCK), /* //MISO */ \
				GPIO_PINNORM(GPIOA,		GPIO_PINS_7,	CRM_GPIOA_PERIPH_CLOCK), /* //MOSI */ \
				cs_setting_name, \
				GPIO_PINOUT(gp_port,	gp_pin,			gp_crm_clk), /* //(PA4) */ \
			}
		#define devconf_at413_spi1b(info, spi_setting_name, cs_setting_name, gp_port, gp_pin, gp_crm_clk, iom) { \
				info, \
				SPI_PINSTD("SPI1", SPI1, CRM_SPI1_PERIPH_CLOCK, iom), \
				spi_setting_name, \
				GPIO_PINNORM(GPIOB,		GPIO_PINS_3,	CRM_GPIOB_PERIPH_CLOCK),  /* //SCK */ \
				GPIO_PININ(GPIOB,		GPIO_PINS_4,	CRM_GPIOB_PERIPH_CLOCK), /* //MISO */ \
				GPIO_PINNORM(GPIOB,		GPIO_PINS_5,	CRM_GPIOB_PERIPH_CLOCK), /* //MOSI */ \
				cs_setting_name, \
				GPIO_PINOUT(gp_port,	gp_pin,			gp_crm_clk), /* //(PA4) */ \
			}
		devconf_at413_spi2("AT32F413 ETHERNET SPI2", "sck/mi/mo/ pb13/pb14/pb15", "cs/ pb12"),
		devconf_at413_spi1a("AT32F413 ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa15",
			GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK, IO_CRM_CLOCK),
		devconf_at413_spi1b("AT32F413 ETHERNET SPI1", "sck/mi/mo/ pb3/pb4/pb5", "cs/ pa15",
			GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK, IO_CRM_CLOCK | IO_MUX_PINREMAP), /* NEW-ADDED */

		devconf_at413_spi1a("AT32F413 ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa4",
			GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK, IO_MUX_NULL),
	#elif defined (AT32F403Axx) || defined (AT32F403xx) || defined (AT32F407xx)
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
		devconf_at403a_spi1("AT32F403A ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa4", GPIOA, GPIO_PINS_4, CRM_GPIOA_PERIPH_CLOCK),
		devconf_at403a_spi2("AT32F403A ETHERNET SPI2", "sck/mi/mo/ pb13/pb14/pb15", "cs/ pb12"),	
		/*!< pa15 must jtag-dp disabled and sw-dp enabled */
		// devconf_at403a_spi1("AT32F403A ETHERNET SPI1", "sck/mi/mo/ pa5/pa6/pa7", "cs/ pa15", /*GPIOB, GPIO_PINS_12, CRM_GPIOB_PERIPH_CLOCK*/ GPIOA, GPIO_PINS_15, CRM_GPIOA_PERIPH_CLOCK),
	#else
		#error "not defined board"
	#endif
};

optsex_t dm9051optsex[BOARD_SPI_COUNT] = { //const 
	#define dmopts_normal(iomode, iomode_name) \
		{ \
			/* .set_name */ \
			/*iomode_name,*/ \
			/* .test_plan_include */ \
			/*DM_FALSE,*/ \
			\
			/* .test_plan_log */ \
			DM_TRUE, "some test log", /*DM_FALSE,*/ \
			/* //vs MBNDRY_BYTE, "8-bit",/ MBNDRY_WORD, "16-bit",*/ \
			iomode, iomode_name, \
			/* //vs CS_EACH, "CS_EACH_MODE",/ CS_LONG, "CS_LONG_MODE",*/ \
			CS_EACH, "CS_EACH_MODE", \
			/* //vs. NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode",/ NCR_FORCE_100MF, "NCR_Force_100MF_mode"/ NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode"*/ \
			NCR_FORCE_100MF, "NCR_Force_100MF_mode", /*NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode",*/ /*NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode",*/ \
			/* //vs. 0, "RX_CTRL Normal Mode",/ 1, "RX_CTRL Promiscuos mode" */ \
			0, "RX_CTRL Normal Mode", \
			/* //vs. DM_TRUE, "Run RX mode",/ DM_FALSE, "Test RX Mode"*/ \
			DM_FALSE, "Test RX Mode", \
			/* //vs. DM_FALSE, "Checksum offload disable",/ DM_TRUE, "checksum offload enable",*/ \
			DM_TRUE, "rxmode checksum offload enable", \
			/* //vs. DM_FALSE, "Flow control disable",/ DM_TRUE, "Flow control enable",*/ \
			DM_TRUE, "Flow control enable", \
			/* //vs. DM_FALSE, "Device support 8/16 bit modes",/ DM_TRUE, "Device is only 8 bit mode",*/ \
			DM_FALSE, "Device support 8/16 bit modes", \
			/* //vs 0~255, "the delay for x2ms times in the hdlr",*/ \
			150, "The delay for x2ms times in the hdlr", \
			/* //vs. DM_FALSE, "No config set recv",/ DM_TRUE, "Hdlr without configure recv",*/ \
			DM_TRUE, "Hdlr configure recv", \
			/* //vs. DM_TRUE, "Davicom tx endbit",/ DM_FALSE, "No tx endbit",*/ \
			DM_FALSE, "No tx endbit", \
			/* //vs. DM_TRUE, "Generic core rst",/ DM_FALSE, "Traditional core rst",*/ \
			DM_TRUE, "Long_delay core rst", \
		}
	#define dmopts_normaldefault(iomode, iomode_name) \
		{ \
			/* .set_name */ \
			/*iomode_name,*/ \
			/* .test_plan_include */ \
			/*DM_FALSE,*/ \
			\
			/* //.test_plan_log */ \
			DM_FALSE, "some test log", \
			/* //vs MBNDRY_BYTE, "8-bit",/ MBNDRY_WORD, "16-bit",*/ \
			iomode, iomode_name, \
			/* //vs CS_EACH, "CS_EACH_MODE",/ CS_LONG, "CS_LONG_MODE", */ \
			CS_EACH, "CS_EACH_MODE", /*CS_EACH, "CS_EACH_MODE",*/ \
			/* //vs. NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode",/ NCR_FORCE_100MF, "NCR_Force_100MF_mode"/ NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode" */ \
			NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode", /*NCR_FORCE_100MF, "NCR_Force_100MF_mode",*/ \
			/* //vs. 0, "RX_CTRL Normal Mode",/ 1, "RX_CTRL Promiscuos mode" */ \
			0, "RX_CTRL Normal Mode", /*0, "RX_CTRL Normal Mode",*/ \
			/* //vs. DM_TRUE, "Run RX mode",/ DM_FALSE, "Test RX Mode"*/ \
			DM_TRUE, "Run RX mode", /*DM_FALSE, "Test RX Mode",*/ \
			/* //vs. DM_FALSE, "Checksum offload disable",/ DM_TRUE, "checksum offload enable", */ \
			DM_FALSE, "Checksum offload disable", \
			/* //vs. DM_FALSE, "Flow control disable",/ DM_TRUE, "Flow control enable", */ \
			DM_FALSE, "Flow control disable", \
			/* //vs. DM_FALSE, "Device support 8/16 bit modes",/ DM_TRUE, "Device is only 8 bit mode",*/ \
			DM_TRUE, "Device is only 8 bit mode", \
			/* //vs 0~255, "the delay for x2ms times in the hdlr",*/ \
			150, "The delay for x2ms times in the hdlr", \
			/* //vs. DM_FALSE, "No config set recv",/ DM_TRUE, "Hdlr without configure recv",*/ \
			DM_TRUE, "Hdlr configure recv", \
			/* //vs. DM_TRUE, "Davicom tx endbit",/ DM_FALSE, "No tx endbit",*/ \
			DM_FALSE, "No tx endbit", \
			/* //vs. DM_TRUE, "Generic core rst",/ DM_FALSE, "Traditional core rst",*/ \
			DM_TRUE, "Long_delay core rst", \
		}
	#define dmopts_test1(iomode, iomode_name) \
		{ \
			/* .set_name */ \
			/*iomode_name,*/ \
			/* .test_plan_include */ \
			/*DM_FALSE,*/ \
			\
			/* //.test_plan_log */ \
			DM_FALSE, "some test log", \
			/* //vs MBNDRY_BYTE, "8-bit",/ MBNDRY_WORD, "16-bit",*/ \
			iomode, iomode_name, \
			/* //vs CS_EACH, "CS_EACH_MODE",/ CS_LONG, "CS_LONG_MODE", */ \
			CS_EACH, "CS_EASH_MODE", \
			/* //vs. NCR_RST_DEFAULT, "NCR PwrOnRst-Default Mode",/ NCR_FORCE_100MF, "NCR_Force_100MF_mode"/ NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode" */ \
			NCR_AUTO_NEG, "NCR_Auto_Negotiation_mode", \
			/* //vs. 0, "RX_CTRL Normal Mode",/ 1, "RX_CTRL Promiscuos mode" */ \
			0, "RX_CTRL Normal Mode", \
			/* //vs. DM_TRUE, "Run RX mode",/ DM_FALSE, "Test RX Mode"*/ \
			DM_FALSE, "Test RX Mode", \
			/* //vs. DM_FALSE, "Checksum offload disable",/ DM_TRUE, "checksum offload enable", */ \
			DM_TRUE, "rxmode checksum offload enable", \
			/* //vs. DM_FALSE, "Flow control disable",/ DM_TRUE, "Flow control enable", */ \
			DM_TRUE, "Flow control enable", \
			/* //vs. DM_FALSE, "Device support 8/16 bit modes",/ DM_TRUE, "Device is only 8 bit mode",*/ \
			DM_FALSE, "Device support 8/16 bit modes", \
			/* //vs 0~255, "the delay for x2ms times in the hdlr",*/ \
			150, "The delay for x2ms times in the hdlr", \
			/* //vs. DM_FALSE, "No config set recv",/ DM_TRUE, "Hdlr without configure recv",*/ \
			DM_TRUE, "Hdlr configure recv", \
			/* //vs. DM_TRUE, "Davicom tx endbit",/ DM_FALSE, "No tx endbit",*/ \
			DM_FALSE, "No tx endbit", \
			/* //vs. DM_TRUE, "Generic core rst",/ DM_FALSE, "Traditional core rst",*/ \
			DM_TRUE, "Long_delay core rst", \
		}
	dmopts_normaldefault(MBNDRY_BYTE, "8-bit"), //CH390 can not use (1, "RX_CTRL Promiscuos mode")
	dmopts_normaldefault(MBNDRY_BYTE, "8-bit"),
	dmopts_normaldefault(MBNDRY_WORD, "16-bit mode"),
	dmopts_normal(MBNDRY_BYTE, "8-bit"),
	dmopts_test1(MBNDRY_WORD, "16-bit mode"),
};

//(define is as rather than '_ETHERNET_COUNT', refer to as '_ETHERNET_COUNT' counter)
const uint8_t mac_addresse[BOARD_SPI_COUNT][MAC_ADDR_LENGTH] = { \
	{0, 0x60, 0x6e, 0x00, 0x01, 0x17,}, \
	{0, 0x60, 0x6e, 0x00, 0x01, 0x26,}, \
	{0, 0x60, 0x6e, 0x00, 0x01, 0x25,}, \
	{0, 0x60, 0x6e, 0x00, 0x01, 0xff,}, \
	{0, 0x60, 0x6e, 0x00, 0x01, 0xff,}, \
};
const uint8_t local_ipaddr[BOARD_SPI_COUNT][ADDR_LENGTH]   	= { \
	{192, 168, 6, 17}, \
	{192, 168, 6, 26}, \
	{192, 168, 6, 25}, \
};
const uint8_t local_gwaddr[BOARD_SPI_COUNT][ADDR_LENGTH]   	= { \
	{192, 168, 6, 1}, \
	{192, 168, 6, 1}, \
	{192, 168, 6, 1}, \
};
const uint8_t local_maskaddr[BOARD_SPI_COUNT][ADDR_LENGTH] 	= { \
	{255, 255, 255, 0}, \
	{255, 255, 255, 0}, \
	{255, 255, 255, 0}, \
};

//
// 'pin_code' always 0. when _ETHERNET_COUNT==1, but _BOARD_SPI_COUNT > 1.
//
int pin_code = 0;

/*IS_DECL_FUNCTION(uint8_t, iomode)
IS_DECL_FUNCTION(uint8_t, promismode)
IS_DECL_FUNCTION(csmode_t, csmode)
IS_DECL_FUNCTION(ncrmode_t, ncrmode)
IS_DECL_FUNCTION(enable_t, rxtypemode)
IS_DECL_FUNCTION(enable_t, rxmode_checksum_offload)
IS_DECL_FUNCTION(enable_t, flowcontrolmode)
IS_DECL_FUNCTION(enable_t, onlybytemode);
IS_DECL_FUNCTION(uint8_t, hdir_x2ms)
IS_DECL_FUNCTION(enable_t, hdlr_confrecv)
IS_DECL_FUNCTION(enable_t, tx_endbit)
IS_DECL_FUNCTION(enable_t, generic_core_rst)
*/
//.DECL_SG_FUNCTION(enable_t, test_plan_include)

//#define OPTS_FUNC_IMPL
//#undef OPTS_FUNC_IMPL
	#if 0
	/*#define DM_MACRO2(rtype, field) \
		rtype dm9051opts_##rtype##field(void) { \
			return dm9051optsex[mstep_get_net_index()].##field; \
		} \
		char *dm9051opts_desc##field(void) { \
			return dm9051optsex[mstep_get_net_index()].desc##field##; \
		}*/
	#endif

#define DM_TYPE		2
#include "dm_types.h"

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
#define spi_iomux()					FIELD_SPIDEV(spidef.iomux)

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
#define get_eth_ip()				&local_ipaddr[pin_code][0]
#define get_eth_gw()				&local_gwaddr[pin_code][0]
#define get_eth_mask()				&local_maskaddr[pin_code][0]

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

const uint8_t *mstep_eth_ip(void) {
	return get_eth_ip();
}
const uint8_t *mstep_eth_gw(void) {
	return get_eth_gw();
}
const uint8_t *mstep_eth_mask(void) {
	return get_eth_mask();
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
