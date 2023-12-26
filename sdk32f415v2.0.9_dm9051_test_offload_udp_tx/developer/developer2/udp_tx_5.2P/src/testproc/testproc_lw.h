/*
 * testproc_lw.h
 */
#include "lwip/err.h"

#ifndef DM9051_DIAG
#define DM9051_DIAG(x) do {printf x;} while(0)
#include <stdio.h>
#include <stdlib.h>
#endif

#define DM9051_RX_DBGMSG(expression, message) do { if ((expression)) { \
  DM9051_DIAG(message);}} while(0)

/* [DM9051 device mac address] */
#if (ETHERNET_COUNT == 1) 
static const uint8_t MACaddr[6] = {0x00, 0x60, 0x6e, 0x00, 0x01, 0x25};
#endif

extern int test_line7_enter_check_setup; //.

//for 'util'
void banner(char *str);
void banner_arg(char *str, uint8_t value);

//.
	 
enum {
  NET_LINK_DOWN,
  NET_LINK_UP
};
void netlink_init(void);
	
//for 'main'

#define MQTT_CLIENT_LINK_TMR_MS						200
/** Function prototype for ... Register such a function
 * using netlink_init().
 *
 * @param net, argument to pass to the function - operation call by phy_link_timer()
 */
typedef struct net_s net_t;
typedef void (*net_link_cb_t)(net_t *net, void *arg, uint8_t status);

struct net_s
{
  /** link state */
  uint8_t link_state;
  /** net callback */
  void *net_arg;
  net_link_cb_t cbf; //=net_cb;
};


void testproc_board_initialize(void);
uint16_t testproc_drv_initialize(void);
void linkup_cb(net_t *net, void *arg, uint8_t status);
int in_time_link_up_found(void);
void testproc_run(void);
//for 'main'
void linkchangeup_sending(void);

//for RX
//.err_t ethernetif_inp(struct netif *netif);
void dump_rx_hdlr(void);
err_t lwip_rx_hdlr(void);

//for 'logic apply'
void check_set_new(void); //logic applied start. 

int check_get(void);
void check_decr_to_done(void);

void check_set_done(void);
int check_get_check_done(void);

//util
void bannerline(void);
int get_testing_rx_count(void);
