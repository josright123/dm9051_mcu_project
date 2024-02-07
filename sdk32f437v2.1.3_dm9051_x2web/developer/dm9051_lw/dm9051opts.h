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

#include "at32f435_437_board.h" //mcu's board
#include "at32f435_437_clock.h" //Also mcu's clock

/*
 * dm9051_declaration_support
 */
#define ETHERNET_COUNT_MAX						4 // Correspond to mcu target board's specification
#define ETHERNET_COUNT							1 //2 //1 //2 //4 //2 //2 //3 //2 //#define get_eth_interfaces() ETH_COUNT

/* Sanity.
 */
#if (ETHERNET_COUNT > ETHERNET_COUNT_MAX)
#error "Please make sure that _ETHERNET_COUNT(config here) must less equal to _ETHERNET_COUNT_MAX"
#endif

#define HELLO_DRIVER_API						1

/*
 * Stop if id not corrext!
 */
#define NON_CHIPID_STOP							1 //0 //0 // stop
#define VER_CHIPID_ONLY							0 //1 //0

/*
 * dm9051_debug_mode selection
 */

#define DM9051_DEBUG_ENABLE						1 //0
#include "dm9051_lw_log.h"

//tobe dm9051_api.c api
//tobe dm9051opts.c internal
//
// [This _HELLO_DRIVER_INTERNAL compiler option, is for a diagnostic purpose while the program code is to use this dm9051_lw driver.]
// [Must be 1, for dm9051_lw driver operating.]
// [Set to 0, for the program code to observ the using APIs of dm9051_lw driver, before add the dm9051_lw driver implement files.]
//
#define HELLO_DRIVER_OPTS_DISPLAY_API			1 //0
#define HELLO_DRIVER_INTERNAL					1 //To support for being called by the program code from outside this dm9051_lw driver.

#if HELLO_DRIVER_INTERNAL

typedef enum {
  DM_FALSE = 0,
  DM_TRUE = !DM_FALSE,
} enable_t;

typedef enum {
  CS_EACH = 0,
  CS_LONG,
} csmode_t;

typedef enum {
  NCR_RST_DEFAULT = 0,
  NCR_FORCE_100MF,
  NCR_AUTO_NEG,
} ncrmode_t;

	#if 0
	/* sub-extern *//* SET and GET */
	/*#define SG_FUNCTION(rtype, field) \
		rtype dm9051opts_get##rtype##field(void); \
		void dm9051opts_set##rtype##field(rtype value)*/

	/* sub *//* SET and GET */
	/*#define DECL_SG_FUNCTION(rtype, field) \
	rtype dm9051opts_get##rtype##field(void) { \
		return dm9051optsex[mstep_get_net_index()].##field; \
	} \
	void dm9051opts_set##rtype##field(rtype value) { \
		dm9051optsex[mstep_get_net_index()].##field = value; \
	}*/
	#endif

#endif //HELLO_DRIVER_INTERNAL

/* [Network Config] */
#define MAC_ADDR_LENGTH                  		(6)
#define ADDR_LENGTH                      		(4)

//------------------

//dm9051opts.c
void GpioDisplay(void);
void dm9051_options_display(void);

#if HELLO_DRIVER_OPTS_DISPLAY_API
void ethcnt_ifdiplay_iomode(void);
void ethcnt_ifdiplay(void);
#endif

//void first_log_clear(int i);
void first_log_init(void);
u8 first_log_get(int i);

//------------------

#define FIELD_SPIDEV(field) \
	devconf[pin_code].field

#define PTR_EXINTD(field) \
	(((struct modscfg_st *)intr_pack)->field)

#define PTR_RSTGPIO(field) \
	((option_rst_common)->field)

//------------------

enum {
	UNIT_TRANS,
	ENUM_TRANS,
};
typedef uint16_t (* trans_t)(void); //typedef void (* trans_t)(void *arg);
uint16_t TRANS_CONN(trans_t trans_func, uint8_t trans_type);

//------------------

#define TO_ADD_CODE_LATER_BACK	0

#endif //__DM9051_OPTS_H
