/*
 * dm9051_lw_testproc.h
 */

/* [Network test partner ip address] */
#define DECLARATION_PARTNER_IPADDR  		"192.168.6.50"

/* [DM9051 device mac address] */
#if (ETHERNET_COUNT == 1) 
static const uint8_t MACaddr[6] = {0x00, 0x60, 0x6e, 0x00, 0x01, 0x25};
#endif

//for 'main'
void testproc_board_initialize(void);
void testproc_drv_initialize(void);
void testproc_packets_test(void);
