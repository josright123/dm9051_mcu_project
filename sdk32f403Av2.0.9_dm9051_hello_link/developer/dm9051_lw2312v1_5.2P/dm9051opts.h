#ifndef __DM9051_OPTS_H
#define __DM9051_OPTS_H

#include "stdint.h"
#include <string.h>

/*
 * at32_cm4_device_support
 */

//#include "at32f413_board.h"
//#include "at32f413_board.h"
//#include "at32f415_board.h"
//#include "at32f415_board.h"
//#include "at32f435_437_board.h"

//#include "at32f415_board.h" //mcu's board 
//#include "at32f415_clock.h" //Also mcu's clock 


#include "at32f403a_407_board.h" //mcu's board 
#include "at32f403a_407_clock.h" //Also mcu's clock 


/*
 * dm9051_declaration_support
 */
#define ETHERNET_COUNT_MAX					4   // Correspond to mcu target board's specification
#define ETHERNET_COUNT							1   //2 //4 //2 //2 //3 //2 //#define get_eth_interfaces() ETH_COUNT

/*
 * Stop if id not corrext!
 */
#define NON_CHIPID_STOP							0 //0 // stop

/*
 * dm9051_debug_mode selection
 */

#define DM9051_DEBUG_ENABLE						0 //1
//#define DM9051_DEBUG_ENABLE					1
#include "dm9051_lw_log.h"
	
/* Put here, instead. Instead of "dm9051_lw_log.h", some generic called in the other implementation place.
 */
#if DM9051_DEBUG_ENABLE == 1
#endif
//.void dm9051_log_dump0(const char *prefix_str, size_t tlen, const void *buf, size_t len);
//.void dm9051_txlog_monitor_tx_all(int hdspc, const uint8_t *buffer, size_t len);
//.void dm9051_txlog_disp(uint8_t *buffer, int len);
//.void dm9051_rxlog_arp(void *payload, uint16_t tot_len, uint16_t len);
//.void dm9051_rxlog_ack(void *payload, uint16_t tot_len, char *result_str);

#if DM9051_DEBUG_ENABLE == 0
//.#define dm9051_log_dump0(prefix_str, tlen, buf, len)
#define dm9051_txlog_monitor_tx_all( hdspc,   buffer,  len)
#define dm9051_rxlog_monitor_rx_all( hdspc,   buffer,  len)
#define dm9051_txlog_disp(buffer,  len)
#define dm9051_rxlog_arp( payload,  tot_len,  len)
#define dm9051_rxlog_ack( payload,  tot_len,  result_str)
#endif

/* Sanity.
 */
#if (ETHERNET_COUNT > ETHERNET_COUNT_MAX)
#error "Please make sure that _ETHERNET_COUNT(config here) must less equal to _ETHERNET_COUNT_MAX"
#endif

/* Sanity2.
 */
#ifndef AT32F437xx
//#define scfg_exint_system_ready			0
//#define gpio_mux_sel_type_system_declaration	0
#else
//#define scfg_exint_system_ready			1
//#define gpio_mux_sel_type_system_declaration	1
#endif

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

/* Sanity5. def
 */
//#define NULL_CRMCLK (crm_periph_clock_type)	0
//#define NULL_PINSRC (gpio_pins_source_type)	0
//#define NULL_MUXSEL (gpio_mux_sel_type)		0

#define GPIO_PINSRC_NULL (gpio_pins_source_type) 0
#define GPIO_MUX_NULL    (gpio_mux_sel_type)	 0

#endif //__DM9051_OPTS_H
