/*
 * testproc_lw.h
 */
#include <stdio.h>
#include "lwip/err.h"

#define bannerline()   printf("\r\n")

#ifndef DM9051_DIAG
#define DM9051_DIAG(x) do {printf x;} while(0)
//#include <stdio.h>
#include <stdlib.h>
#endif

#define DM9051_RX_DBGMSG(expression, message) do { if ((expression)) { \
  DM9051_DIAG(message);}} while(0)

#define flush_path() \
		  dump_rx_hdlr(); \
		  threads_support()
  
#define flush_while_forever()	\
  do { \
	  /*FILE __stdout;*/ \
	  printf("while(1);\r\n"); \
	  /*fflush(NULL);*/ /*fflush(__stdout);*/ /*fflush(NULL);*/ /*flush();*/ \
	  while(1) { \
		  /*delay_ms(1);*/ \
		  flush_path(); \
	  } \
  } while(0)

/* [DM9051 device mac address] */
#if (ETHERNET_COUNT == 1) 
static const uint8_t MACaddr[6] = {0x00, 0x60, 0x6e, 0x00, 0x01, 0x25};
#elif (ETHERNET_COUNT >= 2) 
static const uint8_t addr_meanless[ETHERNET_COUNT][6] = {
	{0x00, 0x60, 0x6e, 0x00, 0x01, 0x25},
	{0x00, 0x60, 0x6e, 0x00, 0x01, 0x26},
}; //no exactly need.
#endif

//for 'util'
void banner(char *str);
void banner_arg(char *str, uint8_t value);

//.
extern int test_line7_enter_check_setup; //.
	 
//enum {
//  NET_LINK_DOWN,
//  NET_LINK_UP
//};
	
//for 'main'

#define MQTT_CLIENT_LINK_TMR_MS						200

/*void _netlink_spring_init(void);
*/
/** Function prototype for ... Register such a function
 * using _netlink_spring_init().
 *
 * @param net, argument to pass to the function - operation call by phy_link_timer()
 */
/*typedef struct net_s net_t;
typedef void (*net_link_cb_t)(net_t *net, void *arg, uint8_t status);

struct net_s
{
  // ** link state
  uint8_t link_state;
  // ** net callback
  void *net_arg;
  net_link_cb_t cbf; //=net_cb;
};*/

void tasks_base_init(void);
void tasks_mach_reinit(void);

void testmode_real(void);
void testing_loop(void);
void proc_test_plan(void);
void proc_test_plan_alldone(void);
void proc_alldone_reset(void);
void proc_testing(void);

void lwip_app_init(void);
void proc_runs(void);

void testproc_board_initialize(void);
uint16_t testproc_drv_initialize(void);
//void _linkup_cb(net_t *net, void *arg, uint8_t status);
//void _linkup_cb(net_t *net, void *arg, uint8_t status);
int in_time_link_up_found(void);
void testproc_run(void);
void testproc_runs(void);
//for 'main'
void linkchangeup_sending(void);

//for RX
//.err_t ethernetif_inp(struct netif *netif);

//.void dump_rx_hdlr(void);
//.err_t lwip_rx_hdlr(void);

//for 'logic apply'

#if 0
//.int check_get(void);
//.int check_get_check_done(void);
//.void check_decr_to_done(void);
//.void check_set_done(void);
//.void check_set_new(void); //logic applied start. 
#define check_get()				1
#define check_get_check_done()	0
#define	check_decr_to_done()
#define	check_set_done()
#define	check_set_new()
#endif

#if 0
//int get_testing_rx_count(void);
//void rxrp_dump_print_init_show(void);
//int tp_all_done(void);
//void display_state(void);
#endif

//util
//.void bannerline(void);
//void _show_rxwp(void); //from 'moni_show_rxwp'
void moni_show_rxrp(void);
void txlen_show_rxwp(uint16_t len);

void check_from_to(int c);
void check_to(void);

void tp_rotate_state(void);
void tpp_set_linkup_stage(void);
int tpp_get_link_state(void);

void tpp_set_linkdown_stage(void);
int tpp_recognized_stage(void);

void tpp_refine_linkup_progress(void);
void tpp_refine_linkdown_progress(void);

uint8_t  flow_control_test_config_init(void);
