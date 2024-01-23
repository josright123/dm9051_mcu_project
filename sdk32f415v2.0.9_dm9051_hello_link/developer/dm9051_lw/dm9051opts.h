#ifndef __DM9051_OPTS_H
#define __DM9051_OPTS_H

#include "stdint.h"
#include <string.h>
#if 0 //[make commented, because no lwip. so no lwip-testmode && lwip-testmode-real]
#include "lwip/opt.h" //We want some compiler option define, defined in "lwipopts.h" such as 'LWIP_TESTMODE' && 'LWIP_TESTMODE_REAL'
					//when finish the test-plan version, remove this #include is expected. for a code style independently.
#endif

/*
 * at32_cm4_device_support
 */

//#include "at32f413_board.h"
//#include "at32f413_board.h"
//#include "at32f415_board.h"
//#include "at32f415_board.h"
//#include "at32f435_437_board.h"

#include "at32f415_board.h" //mcu's board 
#include "at32f415_clock.h" //Also mcu's clock 

/*
 * dm9051_declaration_support
 */
#define ETHERNET_COUNT_MAX						4 // Correspond to mcu target board's specification
#define ETHERNET_COUNT							1 //1 //2 //1 //2 //4 //2 //2 //3 //2 //#define get_eth_interfaces() ETH_COUNT

/* Sanity.
 */
#if (ETHERNET_COUNT > ETHERNET_COUNT_MAX)
#error "Please make sure that _ETHERNET_COUNT(config here) must less equal to _ETHERNET_COUNT_MAX"
#endif

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
#define HELLO_DRIVER_OPTS_DISPLAY_API			1 //0
#define HELLO_DRIVER_INTERNAL					1 //To support for being called by the program code from outside this dm9051_lw driver.

//
// [This HELLO_DRIVER_INTERNAL compiler option, is for a diagnostic purpose while the program code is to use this dm9051_lw driver.]
// [Must be 1, for dm9051_lw driver operating.]
// [Set to 0, for the program code to observ the using APIs of dm9051_lw driver, before add the dm9051_lw driver implement files.]
//
#if HELLO_DRIVER_INTERNAL

/* extern */
#define EXTERN_FUNCTION(rtype, field) \
	rtype dm9051opts_##rtype##field(void); \
	char *dm9051opts_desc##field(void)
#define IS_FUNCTION(rtype, field) \
	EXTERN_FUNCTION(rtype, field)

/* definition for extern short-call-name */
#define IS_INSTEAD(rtype, field)	dm9051opts_##rtype##field

/* declaration */
#define DECL_FUNCTION(rtype, field) \
rtype dm9051opts_##rtype##field(void) { \
	return dm9051optsex[mstep_get_net_index()].##field; \
}									\
char *dm9051opts_desc##field(void) { \
	return dm9051optsex[mstep_get_net_index()].desc##field##; \
}

/* sub-extern */
#define EXTERN_SG_FUNCTION(rtype, field) /* SET and HET */ \
	rtype dm9051opts_get##rtype##field(void); \
	void dm9051opts_set##rtype##field(rtype value)
#define SG_FUNCTION(rtype, field) \
	EXTERN_SG_FUNCTION(rtype, field)
/* sub */
#define DECL_SG_FUNCTION(rtype, field) \
rtype dm9051opts_get##rtype##field(void) { \
	return dm9051optsex[mstep_get_net_index()].##field; \
} \
void dm9051opts_set##rtype##field(rtype value) { \
	dm9051optsex[mstep_get_net_index()].##field = value; \
}
/* definition for extern short-call-name */
#define IS_GET_INSTEAD(rtype, field)	dm9051opts_get##rtype##field
#define IS_SET_INSTEAD(rtype, field)	dm9051opts_set##rtype##field
#endif //HELLO_DRIVER_INTERNAL

//------------------

//dm9051opts.c
#if HELLO_DRIVER_OPTS_DISPLAY_API
void GpioDisplay(void);
void ethcnt_ifdiplay_iomode(void);
void ethcnt_ifdiplay_chipmac(void);
void ethcnt_ifdiplay(void);
#endif

void first_log_init(void);
u8 first_log_get(int i);
//void first_log_clear(int i);

//------------------

#define FIELD_SPIDEV(field) \
	devconf[pin_code].field

#define PTR_EXINTD(field) \
	(((struct modscfg_st *)intr_pack)->field)

#define PTR_RSTGPIO(field) \
	((option_rst_common)->field)

#endif //__DM9051_OPTS_H
