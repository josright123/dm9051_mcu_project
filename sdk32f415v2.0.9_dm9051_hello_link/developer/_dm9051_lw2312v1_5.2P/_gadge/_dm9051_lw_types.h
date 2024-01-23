/*
 * dm9051_lw_types.h
 */
 
#if !gpio_mux_sel_type_system_declaration
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

#define NULL_CRMCLK (crm_periph_clock_type)	0
#define NULL_PINSRC (gpio_pins_source_type)	0
#define NULL_MUXSEL (gpio_mux_sel_type)		0

typedef struct gpio_sel_st {
	gpio_type *gpport;        		//= PORT;
	uint16_t pin;           		//= PIN;
	crm_periph_clock_type crm_clk;  //= CRM_CLK;	
	gpio_mode_type gpio_mode;		//= type
	gpio_pins_source_type pinsrc;
	gpio_mux_sel_type mux;
} gpio_t;

typedef struct gpio_set_st {
	const gpio_t wire_descript;
} gpio_set_t;

typedef struct intr_set_st {
	struct {
		crm_periph_clock_type crm_clk; //CRM_GPIOC_PERIPH_CLOCK,
		uint32_t extline; //= LINE
		IRQn_Type irqn; //= EXINTn_m
	} line;
	struct {
		#if scfg_exint_system_ready == 0
		 crm_periph_clock_type scfg_clk;
		 gpio_port_source_type scfg_port_src;
		 gpio_pins_source_type scfg_pin_src;
		#else
		 crm_periph_clock_type scfg_clk;
		 scfg_port_source_type scfg_port_src; //SCFG_PORT_SOURCE_X
		 scfg_pins_source_type scfg_pin_src; //SCFG_PINS_SOURCEX
		#endif
	} scfg;
} intr_set_t;

typedef struct {
  char *spi_name;
  spi_type *spi_num;        		//= SPIPORT;
  crm_periph_clock_type crm_clk;	//= SPI_CRM_CLK;
  gpio_t wire_sck;
  gpio_t wire_mi;
  gpio_t wire_mo;
  gpio_t wire_cs;
  const gpio_set_t *intr_gpio_set;
  const intr_set_t *intr_set;
  const gpio_set_t *rst_gpio_set;
} spi_dev_t;

const gpio_set_t *conf_rstport(void);
