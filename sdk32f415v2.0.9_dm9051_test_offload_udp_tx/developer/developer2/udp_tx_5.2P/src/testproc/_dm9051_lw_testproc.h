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
uint16_t testproc_drv_initialize(void);
int in_time_link_up_found(void);
void testproc_net_test(void);
void testproc_run(void);
void send_udp_main(void);
void linkchangeup_sending(void);

extern int test_line7_enter_check_setup; //.
void self_test_tx_and_rx(void);

void line7_proc(void);

//logic apply
void check_set_new(void); //logic applied start. 

int check_get(void);
void check_decr_to_done(void);

void check_set_done(void);
int check_get_check_done(void);
