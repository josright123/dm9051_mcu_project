#include "dm9051_lw_conf.h"
#include "dm9051_lw.h"
#include "lwip/apps/httpd.h"

#include "developer_conf.h"
#include "netconf.h"
#include "testproc/testproc_lw.h"
#include "main_malloc.h"

#include "lwip/sys.h"
#include "../netifo.h"

void tasks_base_init(void)
{
  int i;
  mqtt_client_init_log();
	
  for (i = 0; i < ETHERNET_COUNT; i++) {

	  //.tcpip_stack_init();
	  //.dm9051_tmr_init(); //delay_ms(30);
	  //.mqtt_client_process_init();

	  //printf("[i= %d INIT-NOW] working for pin_code: %d\r\n", i, i);
	  mstep_set_net_index(i);
	  if (linkhandler_type() == TYPE_TIMEROUTS)
		  netlink_spring_init();
  }

  //. mqtt_client_init_log(i);
  
  reset_change_timeouts(); //SHARED. //make below _log has no print-out!
  onchange_timeouts_log("main-init"); //SHARED.
}

void tasks_mach_reinit(void) //net->cbf = cb;
{
  int i;
  for (i = 0; i < ETHERNET_COUNT; i++) {
	  mstep_set_net_index(i);
	  if (linkhandler_type() == TYPE_TIMEROUTS) {
		  netlink_reinit();
	  }
  }
}

void lwip_app_init(void)
{
  httpd_init(); //CAN BE SHARED. AS FOR BOTH xnetif[] ?
  
  //debug_dynamic_page();
  //.printf("httpd_init() finish\r\n"); 
  bannerline();
}

void banner_fcr(void)
{
	uint8_t on_fcr;
	on_fcr = cspi_read_reg(DM9051_FCR);
	banner_arg("View current FCR %02x -- dumping ...", on_fcr);
}

void banner_fcr_ingress(void)
{
	uint8_t on_fcr = cspi_read_reg(DM9051_FCR);
	char buff[50];

	sprintf(buff, "View[%d] current FCR %02x -- ingressing ...", mstep_get_net_index(), on_fcr);
	banner(buff);
}

//int tp_flowcontrol_test(void)
//{
//	static int tp_flow_stage = 0;
//	
//	tp_flow_stage++;
//	if (tp_flow_stage % 2)
//		return 1;
//	
//	return 0;
//}

//
// banner.c
//

void banner(char *str)
{
  int n =  strlen(str);
  n = (n >= 65) ? 0 : (65 - n) >> 1;
//"----------------------------------------------------------------"
  printf("\r\n");
  printf("----------------------------------------------------------------\r\n");
  while (n--) printf(" ");
  printf("%s\r\n", str);
  printf("----------------------------------------------------------------\r\n");
  //printf("\r\n");
}

#define ARG_U8	0
#define ARG_U16	1

void banner_arg1(int argtype, char *str, void *value)
{
//----------------------------------------------------------------
  char buffer[160];
  uint8_t *p8 = value;
  uint16_t *p16 = value;
  int n;
  
  //strcpy(buffer, str);
  //if (argtype == ARG_U8)
	//printf(buffer, *p8);
  //if (argtype == ARG_U16)
	//printf(buffer, *p16);
  //printf("\r\n");

  if (argtype == ARG_U8)
	  sprintf(buffer, str, *p8);
  else if (argtype == ARG_U16)
	  sprintf(buffer, str, *p16);
  else
	  sprintf(buffer, str, *p16);
  
  n = strlen(buffer);
  n = (n >= 65) ? 0 : (65 - n) >> 1;
  
  printf("\r\n");
  printf("----------------------------------------------------------------\r\n");
  while (n--) printf(" ");
  printf("%s\r\n", buffer);
  printf("----------------------------------------------------------------\r\n");
  printf("\r\n");
}

void banner_arg(char *str, uint8_t value)
{
  //uint8_t val = value;
  banner_arg1(ARG_U8, str, &value);
}

//
// netconf.c
//

#define DUMP_FREQ_MS 25 //250 //1500 note: more fast is better, and it can continuous input packets recyclely.
static uint32_t local_time;

static uint16_t moni_rxrp = 0;

void rxrp_dump_print_init_show(void) { //init show
	uint16_t rxrp;
	//printf("%s: Update rxrp - - - debug (txc %d rxc %d) (checkrxbcount %d)\r\n",
	//	"init", testing_tx_count, testing_rx_count, check_get()); //" _sending_done_flag %d", sending_done_flag
	
	//=moni_show_rxrp(); //~_moni_read_rxrp();
	rxrp = cspi_read_reg(DM9051_MRRL);
	rxrp |= cspi_read_reg(DM9051_MRRH) << 8;
	moni_rxrp = rxrp;
	printf("<%d>.%4x ", mstep_get_net_index(), rxrp);
	
	local_time = sys_now() + DUMP_FREQ_MS; //since 'fcr_stage_flag' true
}
static int rxrp_read_print_diff_show(void) { //diff show
	//=return moni_read_rxrp();
	uint16_t rxrp;
	rxrp = cspi_read_reg(DM9051_MRRL);
	rxrp |= cspi_read_reg(DM9051_MRRH) << 8;
	if (rxrp != moni_rxrp) {
		moni_rxrp = rxrp;
		printf("<%d> %4x ", mstep_get_net_index(), moni_rxrp);
		return 1;
	}
	return 0;
}

void dumping_start(void);

/*static*/
static void show_rxwp(void) { //.init
	uint16_t rxwp;
	rxwp = cspi_read_reg(DM9051_RWPAL);
	rxwp |= cspi_read_reg(DM9051_RWPAH) << 8;
	printf(" %4x", rxwp);
}

static void init_show_rxwp(void) { //init show
	//printf("%s: Update rxwp - - - debug (txc %d rxc %d) (checkrxbcount %d)\r\n",
		//"init", testing_tx_count, testing_rx_count, check_get()); //"sending_done_flag %d", sending_done_flag
	show_rxwp(); //_moni_read_rxwp("init");
}
static void start_show_rxwp(void) {
	printf(".this starting, rxwp ="); //%04x\r\n");
	show_rxwp();
}
/*extern*/
void txlen_show_rxwp(uint16_t len) {
	printf("  .this tx (len %d) rxwp =", len); //%04x\r\n");
	show_rxwp();
}

#if 1
static int moni_read_rxwp(void) { //.diff
	static uint16_t moni_rxwp = 0;
	uint16_t rxwp;
	rxwp = cspi_read_reg(DM9051_RWPAL);
	rxwp |= cspi_read_reg(DM9051_RWPAH) << 8;
	if (rxwp != moni_rxwp) {
		moni_rxwp = rxwp;
		printf(" [%d].%4x", mstep_get_net_index(), moni_rxwp);
		return 1;
	}
	return 0;
}

static int rxwp_read_print_diff(void) { //diff show
	return moni_read_rxwp();
}
#endif

static int ctest_stage_flag = 0;
static int rxtest_stage_flag = 0; //test_stage_flag = 0, 

//Logic 
//  0. start with a loopback terminator connected
//  1. fcr -- flow control mode
//  2. bp -- back pressurr mode
enum {
	TST_START = 0,
	TST_START_CONNECT_LOOPBK_HEAD,
	TST_LINKUP_FOUND,
	TST_SEND_DONE,
	TST_CHANG_CONNECT_PC,
	TST_FCR_INGRESS, //5 //TST_FCR_RECEIVING,
	TST_FCR_DUMPING, //6
	TST_FCR_DUMPING_RDY,
	TST_FCR_DUMPING_RDY2, //8
	TST_BP_INGRESS, //TST_BP_RECEIVING,
	TST_BP_DUMPING,
	TST_END1, //11
	TST_END2, 
	TST_CHECKSUM_SENDS,
	TST_CHECKSUM_RECVS, //TST_CHECKSUM_SENDS_DONE,
	TST_CHECKSUM_END,
	TST_ALL_DONE,
};
static int gtestproc_stage = TST_START; //_TST_LINKUP_FOUND; //to start = ;
void display_state(void) {
	printf("[display gtestproc_stage %d] / [ALL_DONE %d]\r\n", gtestproc_stage, TST_ALL_DONE);
}
int tp_state(int set_tp_stage) {
	return (gtestproc_stage == set_tp_stage);
}
void tp_rotate_state(void) {
	if (tp_state(TST_FCR_INGRESS)) {
		local_time = 0xffffffff;
		gtestproc_stage = TST_FCR_DUMPING;
		printf("FND LINKDOWN: testproc_stage %d", gtestproc_stage);
		printf(" rotate to %d\r\n", gtestproc_stage);
		dumping_start();
	} 
	else if (tp_state(TST_FCR_DUMPING)) {
		gtestproc_stage = TST_FCR_DUMPING_RDY;
		printf("FND LINKDOWN: testproc_stage %d", gtestproc_stage);
		printf(" rotate to %d\r\n", gtestproc_stage);
	}
	else if (tp_state(TST_FCR_DUMPING_RDY2)) {
		
		//gtestproc_stage = TST_BP_INGRESS;
		gtestproc_stage = TST_END1;
		printf("FND LINKDOWN: testproc_stage %d", gtestproc_stage);
		printf(" move to %d\r\n", gtestproc_stage);
		printf("Next do TST_START_CONNECT_LOOPBK_HEAD ...\r\n");
		
	}
	else {
		if (!tp_state(TST_ALL_DONE))
			printf("FND LINKDOWN: testproc_stage %d to %d\r\n", gtestproc_stage, gtestproc_stage);
	}
}

int tp_all_done(void)
{
	return tp_state(TST_ALL_DONE);
}

#define REFINE_RANGE	9 //18 //50
int refineu_count = REFINE_RANGE;
int refined_count;
extern char *display_identity_bannerline_title;

static void tpp_refine_init(void) {
	refineu_count = refined_count = 0;
}
void tpp_refine_linkup_progress(void) {
	if (refineu_count < REFINE_RANGE) {
		refineu_count++;
		printf("..)");
		if (refineu_count == REFINE_RANGE) {
			
			if (tp_state(TST_FCR_DUMPING_RDY)) {
				printf(".wait linbk-up ~ OK2.\r\n");
				gtestproc_stage = TST_FCR_DUMPING_RDY2;
			}
			if (tp_state(TST_END1)) {
				printf(".wait linbk-up ~ OK2.\r\n");
				gtestproc_stage = TST_END2;
			
				//xxx _dm9051_init
				bannerline();
				printf("&xnetif[%d] call dm9051_init ~ BEGIN2.\r\n",  mstep_get_net_index());
				//= dm9051_init_nondual();
				do {
					uint8_t addr[6];
					display_identity_bannerline_title = ": dm9051_init";
					lwip_get_mac(addr);
					dm9051_init(addr);
				} while(0);
			}
		}
	}
}

void tpp_refine_linkdown_progress(void) {
	if (refined_count < REFINE_RANGE) {
		refined_count++;
		if (refined_count == REFINE_RANGE) {
			
			//if (tp_state(TST_FCR_INGRESS)) {
			//	printf(".wait linbk-down2 ~ OK2.\r\n");
			//	tp_rotate_state();
			//}
			if (tp_state(TST_FCR_DUMPING)) {
				printf(".wait linbk-down2 ~ OK2.\r\n");
				tp_rotate_state();
			}
		}
	}
}

void tpp_set_linkup_stage(void) {
	if (tp_state(TST_FCR_DUMPING_RDY)) {
		printf(".wait linbk-up ~ STA.\r\n");
		//.banner_fcr();
		tpp_refine_init();
	} 
	else if (tp_state(TST_END1)) {
		printf(".wait linbk-up ~ STA.\r\n");
		tpp_refine_init();
	}	
	else if (!tp_state(TST_ALL_DONE))
		gtestproc_stage = TST_LINKUP_FOUND;
}
int tpp_get_link_state(void) {
	return tp_state(TST_LINKUP_FOUND);
}

void tpp_set_linkdown_stage(void) {
	if (tp_state(TST_FCR_INGRESS)) {
		printf(".wait linbk-down ~ STA.\r\n");
		
		tp_rotate_state(); //#if 1 //#endif
#if 0		
		if  (refineu_count == REFINE_RANGE) ..
#endif
		//banner_fcr();
		tpp_refine_init();
	} else
		tp_rotate_state();
}
int tpp_recognized_stage(void) {
	return 0;
}

void link_display(char *str, int pincode, char *speed, char *duplex, char *up)
{
  //char *up, *speed, *duplex;
  uint16_t bmsr = dm9051_bmsr_update();
  uint16_t bmcr = dm9051_bmcr_update();
  
  up = (bmsr & 0x0004) ? "up" : "down";
  speed = (bmcr & 0x2000) ? "100M" : "10M";
  duplex = (bmcr & 0x0100) ? "Full" : "Half";
  
  printf(str, pincode, speed, duplex, up); //.print_lnkcb_up()
}

#if 0
void on_cyc_count_malloc(int ctarget)
{
  static int c = 0;

  if (c < ctarget) {
	#define PROGRAMMER_TEST_LEN	MMALLOC_MAX_LEN1 //3KB --> 1kb
	c++;
	if (c == ctarget) {
		char *prefix_str = (char *) malloc(PROGRAMMER_TEST_LEN); //3KB
		printf("  .###. %d/%d main-loop!!@%d [ %lx = malloc( %d ) in looping test for a reference can according to]\r\n", 1, 1, c, prefix_str, PROGRAMMER_TEST_LEN);
		sprintf(prefix_str, "%d/%d main-loop>>", 1, 1);
		printf("  .###. %s@%d [free( %lx ), for startup test for a reference can according to]\r\n", prefix_str, c, prefix_str);
		free(prefix_str);
	}
  } //c
}
#endif

#if 1
//uint8_t flow_control_test_config_init(void)
//{
//	uint8_t fcr_reg;
//	fcr_reg = FCR_DEFAULT_CONF;
//	cspi_write_reg(DM9051_FCR, fcr_reg);
//	return fcr_reg;
//}

void tp_fcr_ingress_init(void) //.(int set_tp_stage)
{
  void dm9051_mac_adr(const uint8_t *macadd);
  const uint8_t *mac = lwip_get_mac_addresse1();
	
	//if (tp_state(_TST_LINKUP_FOUND)) { //I.E. (gtestproc_stage == _TST_LINKUP_FOUND)
	  //gtestproc_stage = set_tp_stage;
		
	  #if 1
	  init_show_rxwp();
	  bannerline();
	  ctest_stage_flag = 1; //"Receiving enable, NOT CALL lwip_rx_hdlr, for testing flow-control ..."
	  #endif
	
	  //uint8_t val = flow_control_test_config_init();
	  //printf("tp_fcr test: cspi_write_reg(DM9051_FCR, value) is %02x)\r\n", val);
		
	  if (OPT_CONFIRM(flowcontrolmode)) {
		printf("  RESET[%d]: %s, fcr value %02x\r\n", mstep_get_net_index(), dm9051opts_descflowcontrolmode(), FCR_DEFAULT_CONF);
		DM9051_Write_Reg(DM9051_FCR, FCR_DEFAULT_CONF);
	  } else
		printf("  RESET[%d]: %s\r\n", mstep_get_net_index(), dm9051opts_descflowcontrolmode());
	
	  //bannerline();
	  banner_fcr_ingress();
	
	printf("dm9051_set_par[%d] %02x%02x%02x%02x%02x%02x\r\n", mstep_get_net_index(), mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	dm9051_mac_adr(mac); //device.set by write
	
	  cspi_write_reg(DM9051_RCR, RCR_DEFAULT | RCR_PRMSC | RCR_RXEN);
	//}
}

void in_ingress_monitor(void)
{
	if (rxwp_read_print_diff()) {
		ctest_stage_flag++;
		if (!(ctest_stage_flag % 16))
		  printf("\r\n");
	}
}

void tp_fcr_ingress(void) //(int set_tp_stage)
{
	//if (gtestproc_stage == set_tp_stage) {
	  if (ctest_stage_flag) { //also starting by set 1
		in_ingress_monitor();
	  }
	//}
}

uint16_t dump_rx_hdlr(void)
{
	  //uint16_t len;
	  //int l = 0;
	  uint8_t *buffer = get_rx_buffer(); //&EthBuff[0].rx;
		
	  //len = 
		return dm9051_rx_dump(buffer);
		//dm9051_rx(buffer); //dm9051_rx_dump(buffer); //dm9051_rx_dump(buffer);
	  //if (!len)
	  //  return NULL;
}

void dumping_start(void)
{
	rxtest_stage_flag = 1; // such as increase one.
	cspi_write_reg(DM9051_RCR, 0); // not RCR_RXEN and all bits others.
	banner("RCR_DISABLE first, only dumping NOT receiving...");
	banner_fcr();
	rxrp_dump_print_init_show(); //_moni_read_rxrp(); //"init"
}

void tp_fcr_dumping(void)
{
  if (sys_now() > local_time) {
	  local_time += DUMP_FREQ_MS;
	  dump_rx_hdlr();
	  if (rxrp_read_print_diff_show()) {  
		rxtest_stage_flag++;
		if (!(rxtest_stage_flag % 16))
			printf("\r\n");
	  }
  }	
}
#endif

void up_checksum_sendings(void)
{
  //if (testproc_stage == TST_CHECKSUM_SENDS) {
	  uint8_t val;
	  uint8_t *buffer = get_tx_buffer(); //&EthBuff[0].tx; //Tx_Buff;
	  int l = 0;
	  
	  banner("UDP checksum offload");
	
	  bannerline();
	  start_show_rxwp();
	  //Send_UDP();

	  out_packet(OUT_TEST_UDP, buffer, l, buffer_discovercc, sizeof(buffer_discovercc));
	  out_packet(OUT_TEST_UDP, buffer, l, buffer_discover00, sizeof(buffer_discover00));
	// #_if_TEST_PLAN_MODE
	//.Write to [TCSCR_UDPCS_ENABLE/TCSCR_TCPCS_ENABLE/TCSCR_IPCS_ENABLE]
	  //.printf("Start-generation cspi_read_reg() = %02x\r\n", cspi_read_reg(_DM9051_CSCR));
	  val = TCSCR_UDPCS_ENABLE | TCSCR_IPCS_ENABLE;
	  val = 0x07;
	  cspi_write_reg(DM9051_CSCR, val); //for more instead of TCSCR_UDPCS_ENABLE | TCSCR_IPCS_ENABLE
	  banner_arg("cspi_write_reg _DM9051_CSCR Reg31H [IPCS_ENABLE/UDPCS_ENABLE] 0x%02x", val);
	  //printf("cspi_read_reg() = %02x\r\n", cspi_read_reg(_DM9051_CSCR));
	// #_endif
	  out_packet(OUT_TEST_UDP, buffer, l, buffer_discovercc, sizeof(buffer_discovercc));
	// #_if_TEST_PLAN_MODE
	//.Write to [0]
	  cspi_write_reg(DM9051_CSCR, 0);
	  //banner("cspi_write_reg _DM9051_CSCR 0x31 [IPCS_DISABLE/UDPCS_DISABLE] 0");
	  //printf("cspi_read_reg(_DM9051_CSCR) = %02x\r\n", cspi_read_reg(_DM9051_CSCR));
	// #_endif
	  //.send_udp_main(); //out_packet(OUT_TEST_UDP, ...);
	  out_packet(OUT_TEST_UDP, buffer, l, buffer_discover, sizeof(buffer_discover));
		
	  bannerline();
	  banner("TCP checksum offload");
	  //Send_TCP();
		
	  out_packet(OUT_TEST_TCP, buffer, l, buffer_syncc, sizeof(buffer_syncc));
	  out_packet(OUT_TEST_TCP, buffer, l, buffer_syn00, sizeof(buffer_syn00));
	//#_if_TEST_PLAN_MODE
	//.Write to [TCSCR_UDPCS_ENABLE/TCSCR_TCPCS_ENABLE/TCSCR_IPCS_ENABLE]
	  //.printf("Start-generation cspi_read_reg() = %02x\r\n", cspi_read_reg(_DM9051_CSCR));
	  val = TCSCR_TCPCS_ENABLE | TCSCR_IPCS_ENABLE;
	  val = 0x07;
	  cspi_write_reg(DM9051_CSCR, val); //v.s. for more instead of  TCSCR_TCPCS_ENABLE | TCSCR_IPCS_ENABLE
	  banner_arg("cspi_write_reg _DM9051_CSCR Reg31H [IPCS_ENABLE/TCPCS_ENABLE] 0x%02x", val);
	  //printf("cspi_read_reg() = %02x\r\n", cspi_read_reg(_DM9051_CSCR));
	//#_endif
	  out_packet(OUT_TEST_TCP, buffer, l, buffer_syncc, sizeof(buffer_syncc)); //v.s. buffer_syn00, sizeof(buffer_syn00)
		//v.s. buffer_syncc, sizeof(buffer_syncc)
	//#_if_TEST_PLAN_MODE
	//.Write to [0]
	  cspi_write_reg(DM9051_CSCR, 0);
	  //banner("cspi_write_reg _DM9051_CSCR 0x31 [IPCS_DISABLE/TCPCS_DISABLE] 0");
	  //printf("cspi_read_reg(_DM9051_CSCR) = %02x\r\n", cspi_read_reg(_DM9051_CSCR));
	//#_endif
	  out_packet(OUT_TEST_TCP, buffer, l, buffer_sync, sizeof(buffer_sync));
		
	  banner("Next, Receiving IP/UDP/TCP packetS ...");
	  
	  delay_ms(650); //(1250);
	  
	#if 1 //STATEs
	  //.testproc_stage = _TST_CHECKSUM_RECVS; //TST_CHECKSUM_SENDS_DONE; //TST_SEND_DONE;  //sending_done_flag = 1; //rx is allowed
 #if 0
	  //.check_set_new(); //logic applied start. (do when need monotor 'rxb')
 #endif
	#endif //STATEs
	  return;
  //}
}

extern int testing_rx_count;
extern int testing_tx_count;

int get_testing_rx_count(void) {
	return testing_rx_count;
}

int do_checksum_for_a_end(void)
{
	if (testing_rx_count == testing_tx_count) {
		//uint8_t save_fcr;
		uint8_t buf[6];
	#if 1
		cspi_read_regs(DM9051_PAR, buf, 6, OPT_CS(csmode)); //enum_csmode(), dm9051opts_csmode_tcsmode()
		//buf[0] = cspi_read_reg(DM9051_PAR); buf[1] = cspi_read_reg(DM9051_PAR+1); buf[2] = cspi_read_reg(DM9051_PAR+2);
		//buf[3] = cspi_read_reg(DM9051_PAR+3); buf[4] = cspi_read_reg(DM9051_PAR+4); buf[5] = cspi_read_reg(DM9051_PAR+5);
	#endif
		printf("Testing done, Mac address: %02x %02x %02x %02x %02x %02x\r\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
		
		printf("= IP/UDP/TCP checksum offload testing done ...\r\n");
		
	#if 0
		banner("Flow-control INGRESS Test");
		delay_ms(1250);
			//cspi_read_reg(DM9051_PAR), cspi_read_reg(DM9051_PAR+1), cspi_read_reg(DM9051_PAR+2),
			//cspi_read_reg(DM9051_PAR+3), cspi_read_reg(DM9051_PAR+4), cspi_read_reg(DM9051_PAR+5)
		
		//cspi_write_reg() flow-control disable state
		// ...
		//cspi_write_reg() flow-control enable state (ENABLE)
		// ...
		
		//.printf("Start-enable cspi_read_reg() fcr = %02x\r\n", cspi_read_reg(DM9051_FCR));
		
		//save_fcr = cspi_read_reg(DM9051_FCR);
		cspi_write_reg(_DM9051_FCR, _FCR_DEFAULT_CONF);
		//do {
		  //banner_arg("Start FCR %02x", FCR_DEFAULT_CONF );
		//} while(0);
		
		//printf("Check-enable cspi_read_reg() fcr = %02x\r\n", cspi_read_reg(DM9051_FCR));
		banner_arg("Start FCR %02x", cspi_read_reg(DM9051_FCR));
		printf("Mac address: %02x %02x %02x %02x %02x %02x\r\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
		
		printf("PLEASE CONNECT TO A PC!\r\n");
		rxwp_read_print_diff(); //_rxwp_read_print_init();
		printf("\r\n");
		#if 0
		//._rxwp_read_print_init();
		//.test_stage_flag = 1; //"Receiving enable, NOT CALL lwip_rx_hdlr, for testing flow-control ..."
		#endif
		
		//testproc_stage = TST_CHANG_CONNECT_PC; //_TST_FCR_RECEIVING; //sending_done_flag = 0;
	#endif
		return 1;
	}
	return 0;
}

extern err_t do_checksum_check_rxhdlr(void);

void proc_test_plan(void)
{
  do {
	  int b;
	  
	  bannerline();
	  //printf("test.s\r\n");
	  //ethcnt_ifdiplay_chipmac();
	  //printf("test.e\r\n");
	  
	  for (b = 0; b < ETHERNET_COUNT; b++) {
		  
		  mstep_set_net_index(b);
		  printf("[TESTING %d ...]\r\n", b);
		  if (netif_is_link_up(&xnetif[b])) {
			printf("[TESTING %d progress, because link-is up...]\r\n", b);
			bannerline();
		  } else {
			printf("[TESTING %d escape, because link-is down...]\r\n", b);
			bannerline();
			continue;
		  }
		  
  //[flow-control test]
  #if 0
		  for (;;) {
		  //for (i = 0; i < ETHERNET_COUNT; i++) {
			//if (i == mstep_get_net_index()) {
			 if (tp_state(TST_LINKUP_FOUND)) {
				gtestproc_stage = TST_FCR_INGRESS;
				 
				  do {
				  char buff[80];
				  sprintf(buff, "Net[%d], Ingress packets, Flow Control eanble mode ...", mstep_get_net_index()); 
				  banner(buff);
				  } while(0);
				
				  do {
				  char *up, *speed, *duplex;
				  link_display("TST_FCR_INGRESS[%d] more check (%s %s %s)\r\n", mstep_get_net_index(), speed, duplex, up);
				  } while(0);
				  
				tp_fcr_ingress_init();
				  
				//bannerline();
				//printf("TESTING.Check.S1s %d ...\r\n", mstep_get_net_index());
				 printf("xnetif-mac[%d] %02x%02x%02x%02x%02x%02x\r\n", b, xnetif[b].hwaddr[0],xnetif[b].hwaddr[1],xnetif[b].hwaddr[2],xnetif[b].hwaddr[3],xnetif[b].hwaddr[4],xnetif[b].hwaddr[5]);
				  
				  do {
				  uint8_t buf[6];
				  cspi_read_regs(DM9051_PAR, buf, 6, dm9051opts_csmode());
				  printf("chip-mac[%d] %02x%02x%02x%02x%02x%02x\r\n", b, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
				  } while(0);

				 display_state();
			 }
			 if (tp_state(TST_FCR_INGRESS))
				tp_fcr_ingress();
			 if (/*tp_state(TST_FCR_DUMPING) ||*/ tp_state(TST_FCR_DUMPING_RDY2))
			 {
				/*if (!netconf_is_link_up()) { //NOT by (!netif_is_up(netif))
					if(tp_state(TST_FCR_DUMPING)) {
						printf(".wait linbk-up ~ DETECTING.\r\n"); //wait linbk-up~
						gtestproc_stage = TST_FCR_DUMPING_RDY; // will progress to 'TST_FCR_DUMPING_RDY2' by eliminate bounce (debounce.)
					}
				} else
				*/
					tp_fcr_dumping();
			 }
			 
			 if ( tp_state(TST_END1) || tp_state(TST_BP_INGRESS)) {
				//printf("TESTING.Check.E1e %d ...\r\n", mstep_get_net_index());
				printf("-------------------- end1 ---------------------------\r\n");
				break;
			 }
			 
			 threads_support();
			//}
		  //}
		  }
  #else
  //skip [flow-control test]
		  #if 0
		  /*for (;;) {
			  if (tp_state(TST_LINKUP_FOUND)) {
				gtestproc_stage = TST_END2;
				break;
			  }
		  }*/
		  #endif
		  
		  #if 1
		  if (netif_is_link_up(&xnetif[b]))
			  gtestproc_stage = TST_END2;
		  #endif
  #endif
		  
  //[checksum-offload test]
		  for (;;) {
			if (tp_state(TST_END2)) { /*tp_state(TST_END1) ||*/ 
			#if 1
				bannerline();
				//printf("TESTING.Check.S2s %d ...\r\n", mstep_get_net_index());
				printf("INDIVIDUALLY[%d]: test rx_promiscous (write_reg, 0x05, (1 << 2))\r\n", mstep_get_net_index());
				DM9051_Write_Reg(DM9051_RCR, RCR_DEFAULT | RCR_PRMSC | RCR_RXEN); //promiscous
			#endif
				gtestproc_stage = TST_CHECKSUM_SENDS;
				up_checksum_sendings();
				gtestproc_stage = TST_CHECKSUM_RECVS; //TST_CHECKSUM_SENDS_DONE; //sending_done_flag = 1; //rx is allowed
			}
			if ( tp_state(TST_CHECKSUM_RECVS)) {
				//gtestproc_stage = _TST_CHECKSUM_RECVS;
				
				if (do_checksum_check_rxhdlr() == ERR_OK)
					testing_rx_count++;
				
				if (do_checksum_for_a_end()) {
					gtestproc_stage = TST_CHECKSUM_END;
					printf("-------------------- end2 ---------------------------\r\n");
					printf("TESTING.Check.E2e %d ...\r\n", mstep_get_net_index());
					break;
				}
			}
			threads_support();
		  }
	  } //b
  
  printf("test_end\r\n");
  printf("NEXT run into normal web server application\r\n");
  bannerline();
	  
  } while(0);
}

void proc_testing(void)
{
#if LWIP_TESTMODE
  proc_test_plan();
#endif
}

void proc_test_plan_alldone(void) {
  gtestproc_stage = TST_ALL_DONE;
}

void proc_alldone_reset(void)
{
#if LWIP_TESTMODE
	int b;
	uint8_t nsr;
   #if 1
	bannerline();
	printf("... Test done, NEXT reset with ethernetif_reset_proc() ...\r\n");
	
	for (b = 0; b < ETHERNET_COUNT; b++) {  
		mstep_set_net_index(b);
		#if 1
		nsr = cspi_read_reg(DM9051_NSR);
		printf("... Start.s nsr %02x\r\n", nsr);
	
		ethernetif_reset_proc();
	
		dm_delay_ms(10);
		nsr = cspi_read_reg(DM9051_NSR);
		printf("... Start.e nsr %02x\r\n", nsr);
		#endif
	}
   #else
	bannerline();
	banner("......SKIP SKIP SKIPING the test-plan functions......");
   #endif
#endif
}
	
enable_t get_testplaninclude(void) {
	return DM_FALSE;
}

void testmode_real(void)
{
#if LWIP_TESTMODE || LWIP_TESTMODE_REAL
  //.set_testplanlog(DM_TRUE); //set_dm9051opts_testplanlog(DM_TRUE);
  if (get_testplaninclude())
	proc_testing();
  
  #if LWIP_TESTMODE_REAL
	  //if. Davicom (can RST such way!)
	  proc_alldone_reset();
	  /*
	   * ethernetif_reset_proc() proc_alldone_reset().
	   */
  #else
	  //else. CH390 (can not as RST above way!)
	  do {
		int b;
		uint8_t nsr;
		for (b = 0; b < ETHERNET_COUNT; b++) {  
			mstep_set_net_index(b);
			#if 1
			nsr = cspi_read_reg(DM9051_NSR);
			printf("... Start.s nsr %02x\r\n", nsr);
			dm_delay_ms(10);
			nsr = cspi_read_reg(DM9051_NSR);
			printf("... Start.e nsr %02x\r\n", nsr);
			#endif
		}
	  } while(0);
  #endif
#endif
}

static void proc_run_handler(int i) //HOW i can suit for!
{
	//if (tp_flowcontrol_test())
	//	return;
	#if 1
		lwip_rx_hdlr(i);
		  
		//#if (APP_ETH_COUNT > 1) 
		//	mstep_next_net_index(); //next_pin_code();
		//#endif
	#else
		lwip_rx_loop_handler(); /* lwip receive handle */
	#endif
}

//static int rxtest_stage_flag = 0; already!
static int dump_rx_stage_flag = 0;
static int dump_odd_count = 0;
static void testfunc(void)
{
	int i;
	uint16_t len;
	for (i = 0; i < ETHERNET_COUNT; i++) {
		mstep_set_net_index(i);
		//in_ingress_monitor();= no need.
		
		//flush_path();=
		#if 1
		  len = dump_rx_hdlr();
		  if (len & 1) {
			  printf("\r\n+ tesfunc +oddfound %d (len %d)\r\n", ++dump_odd_count, len);
		  }
		
		  if (rxrp_read_print_diff_show()) {  
			dump_rx_stage_flag++;
			if (!(dump_rx_stage_flag % 16))
				printf("\r\n");
		  }
		  //threads_support()
		#endif
	}
	threads_support();
}

void testing_loop(void)
{
	int i;
	
	dump_rx_stage_flag = 1; // such as increase one.
	
	for (i = 0; i < ETHERNET_COUNT; i++) {
		mstep_set_net_index(i);
		cspi_write_reg(DM9051_RCR, RCR_DEFAULT | RCR_PRMSC | RCR_RXEN); //promiscuse!
		rxrp_dump_print_init_show(); //_moni_read_rxrp(); //"init"
	}
	
	while(1)
		testfunc();
}

void periodic_loop(void)
{
	int i;
	for (i = 0; i < ETHERNET_COUNT; i++) {
		if (netif_is_link_up(&xnetif[i])) { //Added for exactly~
			mstep_set_net_index(i);
			
			#if 1 //[debug rxwp]
			if (get_testplanlog(test_plan_log))
				in_ingress_monitor();
			#endif
			
			proc_run_handler(i); //PARAM i
		}

		#if 0
		//[Note:] when _ETHERNET_COUNT > 1, then (linkhandler_type() == TYPE_TICKS) still not support!!
		lwip_periodic_handle(); //USE _get_net_index, THE EACH, /*timeout handle*/
		#endif
	}

	threads_support();
	//.on_cyc_count_malloc(12);
}

void proc_runs(void)
{
 for(;;)
	periodic_loop();
}
