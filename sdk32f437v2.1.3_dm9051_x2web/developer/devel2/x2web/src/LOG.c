#include "lwip/opt.h"

#include "lwip/apps/httpd.h"
#include "lwip/timeouts.h"
#include "lwip/priv/tcp_priv.h"

#include "lwip/def.h"
#include "lwip/memp.h"
#include "lwip/priv/tcpip_priv.h"

#include "lwip/ip4_frag.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/nd6.h"
#include "lwip/ip6_frag.h"
#include "lwip/mld6.h"
#include "lwip/dhcp6.h"
#include "lwip/sys.h"
#include "lwip/pbuf.h"
#include "netif/ppp/ppp_opts.h"

#include "dm9051_lw_conf.h"
#include "dm9051_lw.h"
#include "developer_conf.h" //#include "main.h" //#include "developer_conf.h"
#include "netconf.h"
#include "testproc/testproc_lw.h"
#include <string.h>

void mqtt_client_init_log(void)
{
  BannerDisplay(); // [log]

  ethcnt_ifdiplay();
	
  //ParamDisplay(); // [log]
  printf("\r\n");
}

void BannerDisplay(void)
{
  printf("\r\n");
  printf("BannerDisplay() MEMP_MAX= %d\r\n", MEMP_MAX);
	
  printf("\r\n");
#if 0
  printf("@example                %s %s\r\n", get_application_name(), get_application_date());
  printf("@version                %s\r\n", get_application_banner());
  //;printf("@date                   %s\r\n", get_application_date());
//.printf("@client_id              %s\r\n", mqtt_connect_info.client_id); //CLIENT_ID
  //.printf("@user_name              %s\r\n", USER_NAME ? USER_NAME : "NULL");
  //.printf("@user_password          %s\r\n", USER_PASSWORD ? USER_PASSWORD : "NULL");

//linkhandler_info
  do {
	int i;
	for (i = 0; i< ETHERNET_COUNT; i++) {
		mstep_set_net_index(i);
		printf("@link_handler_info[%d]   %s\r\n",
			i, (linkhandler_type() == TYPE_TIMEROUTS) ? "link_model_timeouts" : "link_model_ticks");
	}
  } while(0);
  //.printf("@publish_handler        %s\r\n", (get_publishhandler_type() == TYPE_TIMEROUTS) ? "publish_model_timeouts" : "publish_model_ticks");
  //.printf("@quick_publish_func     %d\r\n", MQTT_CLIENT_CONNECT_QUICK_PUBLISHS);
#if 1
/*
	NOT Flash size, This is flash size effect, below:
	.\objects\web_5_2P.axf: Error: L6220E: Execution region RW_IRAM1 size (32784 bytes) exceeds limit (32768 bytes). Region contains 19 bytes of padding and 0 bytes of veneers (total 19 bytes of linker generated content).
*/	
  printf("@lwip_Timeouts           %d\r\n", MEMP_NUM_SYS_TIMEOUT); //=startup_system_timeouts_num_info(); //.Timeouts_info
 #if 0
  printf("@MEMP_NUM_TCP_PCB        %d\r\n", MEMP_NUM_TCP_PCB);
  printf("@MEMP_NUM_TCP_PCB_LISTEN %d\r\n", MEMP_NUM_TCP_PCB_LISTEN);
 #endif
  printf("@HTTPD_SERVER_PORT       %d\r\n", HTTPD_SERVER_PORT);
 #if 0
  printf("\r\n");
  printf("@TCP_MSS                 %d\r\n", TCP_MSS);
  printf("@TCP_SND_QUEUELEN        %d\r\n", TCP_SND_QUEUELEN);
  printf("@TCP_SND_BUF             %d\r\n", TCP_SND_BUF);
  printf("@TCP_WND                 %d\r\n", TCP_WND);
 #endif
  printf("@xnetif[0].mtu           %d\r\n", xnetif[0].mtu);
  printf("@xnetif[1].mtu           %d\r\n", xnetif[1].mtu);
  printf("@PBUF_POOL_BUFSIZE       %d\r\n", PBUF_POOL_BUFSIZE);
#endif
#endif
}

void NetifDisplay(int i)
{
  //uint8_t mac[MAC_ADDR_LENGTH]; lwip_get_mac_addresse0(mac);
  printf("\r\n");
	
  /*printf("mac:                    %02x:%02x:%02x:%02x:%02x:%02x\r\n", 
							      mac[0], mac[1],
							      mac[2], mac[3],
							      mac[4], mac[5]);
  printf("ip:                     %"U16_F".%"U16_F".%"U16_F".%"U16_F"\r\n", 
							      ip4_addr1_16(netif_ip4_addr(&xnetif[0])), 
							      ip4_addr2_16(netif_ip4_addr(&xnetif[0])),
							      ip4_addr3_16(netif_ip4_addr(&xnetif[0])), 
							      ip4_addr4_16(netif_ip4_addr(&xnetif[0])));
  printf("msk:                    %"U16_F".%"U16_F".%"U16_F".%"U16_F"\r\n", 
							      ip4_addr1_16(netif_ip4_netmask(&xnetif[0])), 
							      ip4_addr2_16(netif_ip4_netmask(&xnetif[0])),
							      ip4_addr3_16(netif_ip4_netmask(&xnetif[0])), 
							      ip4_addr4_16(netif_ip4_netmask(&xnetif[0])));
  printf("gw:                     %"U16_F".%"U16_F".%"U16_F".%"U16_F"\r\n", 
							      ip4_addr1_16(netif_ip4_gw(&xnetif[0])), 
							      ip4_addr2_16(netif_ip4_gw(&xnetif[0])),
							      ip4_addr3_16(netif_ip4_gw(&xnetif[0])), 
							      ip4_addr4_16(netif_ip4_gw(&xnetif[0])));*/
								  
 #if 0
  const uint8_t * mac = lwip_get_mac_addresse1();
  printf("mac[%d]:                %02x:%02x:%02x:%02x:%02x:%02x\r\n", i,
							      mac[0], mac[1],
							      mac[2], mac[3],
							      mac[4], mac[5]);
  printf("ip[%d]:                 %"U16_F".%"U16_F".%"U16_F".%"U16_F"\r\n", i,
							      ip4_addr1_16(netif_ip4_addr(&xnetif[i])), 
							      ip4_addr2_16(netif_ip4_addr(&xnetif[i])),
							      ip4_addr3_16(netif_ip4_addr(&xnetif[i])), 
							      ip4_addr4_16(netif_ip4_addr(&xnetif[i])));
  printf("msk[%d]:                %"U16_F".%"U16_F".%"U16_F".%"U16_F"\r\n", i,
							      ip4_addr1_16(netif_ip4_netmask(&xnetif[i])), 
							      ip4_addr2_16(netif_ip4_netmask(&xnetif[i])),
							      ip4_addr3_16(netif_ip4_netmask(&xnetif[i])), 
							      ip4_addr4_16(netif_ip4_netmask(&xnetif[i])));
  printf("gw[%d]:                 %"U16_F".%"U16_F".%"U16_F".%"U16_F"\r\n", i,
							      ip4_addr1_16(netif_ip4_gw(&xnetif[i])), 
							      ip4_addr2_16(netif_ip4_gw(&xnetif[i])),
							      ip4_addr3_16(netif_ip4_gw(&xnetif[i])), 
							      ip4_addr4_16(netif_ip4_gw(&xnetif[i])));
 #endif
}

void EepromDisplay(int pin)
{
	int i;
	printf("--EEPROM[%d] word", pin);
	for (i = 0; i < 9; i++) {
		uint16_t value;
		value = eeprom_read(i);
		printf("%s%04x", 
			!(i % 4) ? "  " : " ",
			value);
	}
	bannerline();
}

#if 0

//#define startup_system_timeouts_num_info() \
// printf(".[Lwip_Timeouts]        %d\r\n", MEMP_NUM_SYS_TIMEOUT)
//.printf(" .timeouts num          %d\r\n", MEMP_NUM_SYS_TIMEOUT)

#define startup_system_total_reqlist_info() \
  printf(" .request list space:   %d\r\n", MQTT_REQ_MAX_IN_FLIGHT); \
  printf(" .max publish number:   %d\r\n", get_mqtt_max_publish_num()); \
  printf(" .max subscribe number: %d\r\n", get_mqtt_max_subscribe_num())

static void print_broker_info(char *broker_ip, uint16_t server_port)
{
  ip_addr_t server_ip;
  ip4_addr_set_u32(&server_ip, ipaddr_addr(broker_ip));

	printf("[Broker_info]           %d.%d.%d.%d:%u\r\n", (server_ip.addr >> 0) & 0xff, (server_ip.addr >> 8) & 0xff, 
	(server_ip.addr >> 16) & 0xff, (server_ip.addr >> 24) & 0xff, server_port);
}

#if MQTT_CLIENT_SUPPORT
static u16_t calc_calc_watch_dog1(u16_t keep_alive)
{
	u16_t sec = calc_calc_pingreg1(keep_alive);
	return sec + (sec/2);
}

void print_calc_pingreg_watchdog(u16_t keep_alive)
{
	u16_t sec = calc_calc_pingreg1(keep_alive);
	u16_t wsec = calc_calc_watch_dog1(keep_alive);
	printf("[Keep_alive_info]      %3u sec pingreq time %3u watch-dog %3u\r\n", keep_alive, sec, wsec);
	//.printf(" .while if keep_alive  %3u sec pingreq time %3u watch-dog %3u\r\n", keep_alive, sec, wsec);
}
#endif

#if 0
void ParamDisplay(void)
{
  printf("\r\n");
  print_broker_info(lwip_get_broker_ipe(), get_broker_listen_port()); //printf(".[Broker_info]\r\n");

  //printf("\r\n");
  //startup_system_timeouts_num_info(); //printf(".[Lwip_Timeouts]\r\n"); //.Timeouts_info

  //#if MQTT_CLIENT_SUPPORT
  //printf("\r\n");
  //print_calc_pingreg_watchdog(mqtt_connect_info.keep_alive); //printf(".[Keep_alive_info]\r\n");
  //#endif

  //#if MQTT_CLIENT_SUPPORT && MQTT_DEBUG_TIME
  //printf("\r\n");
  //printf("[Keep_alive_debug]\r\n");
  //do {
	//  u16_t i;
	//  printf("\r\n");
	//  for (i = 0; i < 121; i++)
	//	print_calc_pingreg_watchdog(i);
  //} while(0);
  //#endif
  
  printf("\r\n");
  printf("[Request_info]\r\n");
  startup_system_total_reqlist_info();
  
  #if 0
  //printf("\r\n");
  //printf(".[Publish_info]\r\n");
  //print_publish_info(get_mqtt_publish_topic_number());
  //printf(".[Subscribe_info]\r\n");
  //print_subscribe_info();
  #endif
  printf("\r\n");
}
#endif

#if MQTT_MAIN_CLIENT_DO_PUBLISH_LF && MQTT_CLIENT_CONNECT_QUICK_PUBLISHS
	void quick_publish_head_str(void)
	{
	  printf("\r\n");
	  printf(".[publish.all.after.connected].sx\r\n");
	}

	void quick_publish_tail_str(int i)
	{
	  int last_index = get_mqtt_publish_topic_number() - 1;
	  uint16_t last_enum_interval = get_link_handler_ms()*((uint16_t)last_index); //"LINK_HANDLER_MS"
		
	  if (get_mqtt_publish_topic_number() == 1) {
		  uint16_t first_enum_interval = get_link_handler_ms()/2; // refer to '_publish_handle_news()'
		  last_enum_interval = first_enum_interval;
	  }
		
	  /* line-feed */
	  if (i == last_index &&
		get_publish_disp_time(i) == last_enum_interval) {
		printf(".[publish.all.after.connected].ex\r\n");
		printf("\r\n");
	  }
	}
#endif

void print_will_topic_info(void) {
  printf(" .will info\r\n");
  printf("  .will.topic           %s\r\n", WILL_TOPIC);
  printf("  .will.message         %s\r\n", WILL_MSG);
  printf("  .will.qos             %d\r\n", WILL_QOS);
  printf("  .will.retain          %d\r\n", WILL_RETAIN);
}

void print_publish_info(int publish_topic_num)
{
  int i;
	
  printf(" .publish topic num %d\r\n", publish_topic_num);
  for (i= 0; i< publish_topic_num; i++)
    printf("  .publish %02d.topic len %3d       %5d ms qos %d %s\r\n", 
		i, strlen(get_publish_topic(i)), get_publish_send_ms(i), get_publish_qos(i), get_publish_topic(i));
}

void print_subscribe_info(void)
{
  int i;
  printf(" .subscribe topic num %d\r\n", get_mqtt_subscribe_topic_number());
  for (i= 0; i< get_mqtt_subscribe_topic_number(); i++)
    printf("  .subscribe %02d.topic len %3d     %s qos %d %s\r\n", 
		i, strlen(get_subscribe_topic(i)), "        ", get_subscribe_qos(i), get_subscribe_topic(i));
}
#endif

#if MQTT_DEBUG_TIME
#define LWIP_MAX_TIMEOUT  0x7fffffff
#define TIME_LESS_THAN(t, compare_to) ( (((u32_t)((t)-(compare_to))) > LWIP_MAX_TIMEOUT) ? 1 : 0 )
#endif

//void onchange_timeouts_log(const char *headstr)
//{
//#if MQTT_DEBUG_SYS_TIMEOUTS_SHOW
//	sys_print_timeouts_times1(headstr, sys_count_timeouts());
//#endif
//}

#if LWIP_DEBUG_TIMERNAMES == LWIP_DBG_OFF
void tcpip_tcp_timer(void *arg);
//.void mqtt_cyclic_timer(void *arg); /*static*/ 
//.void publish_cyclic_timer(void *arg);
#endif

#if MQTT_DEBUG_SYS_TIMEOUTS_SHOW //if we are to use, migrated to such as "netconf2.c"
#if LWIP_TESTMODE
static struct sys_timeo *GET_HDLR(void)
{
	struct sys_timeo** HDLR= sys_timeouts_get_next_timeout();
	return *HDLR;
}

int sys_count_timeouts(void) //JJ.tmp
{
  struct sys_timeo *next_timeout = GET_HDLR();
  struct sys_timeo *go_timeout = next_timeout;
  int n = 0;
  do {
    struct sys_timeo *tmptimeout;
    tmptimeout = go_timeout;
    if (tmptimeout == NULL) {
      break; //return n;
    }
	n++;
	go_timeout = tmptimeout->next;
  } while (1); // Repeat until all timers have been count
  return n;
}

static int monnum = 0;

void reset_total_timeouts(void) {
	monnum = 0;
}

static int new_total_timeouts(int num) {
  //static int monnum = 0;
  if (monnum != num) {
	monnum = num;
	return 1;
  }	
  return 0;
}

static void sys_print_basic_lwip(const char *headstr)
{
#if MQTT_DEBUG_TIME
	/*static*/ void lwip_cyclic_timer(void *arg);
	  u32_t ret;
	  int n = 0;
	  struct sys_timeo *next_timeout = GET_HDLR();
	  struct sys_timeo *go_timeout = next_timeout;
	  u32_t now = sys_now();
		  
	#if LWIP_DEBUG_TIMERNAMES
	#if LWIP_TCP
	//printf("ms-list (IP_TMR_INTERVAL %u ms, ARP_TMR_INTERVAL %u ms, IGMP_TMR_INTERVAL %u ms)\r\n",
	//	   IP_TMR_INTERVAL, ARP_TMR_INTERVAL, IGMP_TMR_INTERVAL);
	#else
	//printf("ms-list (TCP_TMR_INTERVAL %u ms, IP_TMR_INTERVAL %u ms, ARP_TMR_INTERVAL %u ms, IGMP_TMR_INTERVAL %u ms)\r\n",
	//	   TCP_TMR_INTERVAL, IP_TMR_INTERVAL, ARP_TMR_INTERVAL, IGMP_TMR_INTERVAL);
	#endif
	#endif
	
	  //printf("tmrs\r\n");
	
	  do {
		char hname[20]; //u8 bflg = 0;
		struct sys_timeo *tmptimeout;
		tmptimeout = go_timeout;
		if (tmptimeout == NULL)
		  break;
		
	//#if LWIP_DEBUG_TIMERNAMES
	//printf("on enum %s (%u ms)\r\n", tmptimeout->handler_name, tmptimeout->intvl_msec);
	//#endif

		n++;
		go_timeout = tmptimeout->next;
		
		if (TIME_LESS_THAN(tmptimeout->time, now))
			ret = 0;
		else
			ret = tmptimeout->time - now;
		
	#if LWIP_DEBUG_TIMERNAMES
	
	memset(hname, ' ', 20);
	hname[19] = 0;
	memcpy(hname, tmptimeout->handler_name, strlen(tmptimeout->handler_name));
		
		/*if (!strcmp("igmp_tmr", tmptimeout->handler_name) ||
		!strcmp("ip_reass_tmr", tmptimeout->handler_name) ||
		!strcmp("etharp_tmr", tmptimeout->handler_name) ||
		!strcmp("lwip_cyclic_timer", tmptimeout->handler_name)
		) {
		  printf(" [%5u.b %s]\t  on enum %s (%u ms)\r\n", ret, hname, hname, tmptimeout->intvl_msec);
		  continue; //b. basic, always (igmp_tmr, ip_reass_tmr, etharp_tmr)
		}*/
		
		if (!strcmp("igmp_tmr", tmptimeout->handler_name)) {
		  printf(" [%5u.b %s]\t  on enum %s (%4u ms IGMP_TMR_INTERVAL)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);
		  continue;
		}
		if (!strcmp("ip_reass_tmr", tmptimeout->handler_name)) {
		  printf(" [%5u.b %s]\t  on enum %s (%4u ms IP_TMR_INTERVAL)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);
		  continue;
		}
		if (!strcmp("etharp_tmr", tmptimeout->handler_name)) {
		  printf(" [%5u.b %s]\t  on enum %s (%4u ms ARP_TMR_INTERVAL)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);
		  continue;
		}
		if (!strcmp("lwip_cyclic_timer", tmptimeout->handler_name)) {
		  printf(" [%5u.?b %s]\t  on enum %s (%4u ms ?)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);
		  continue;
		}
		 
		if (!strcmp("tcpip_tcp_timer", tmptimeout->handler_name)) {
		  printf(" [%5u.c %s]\t  on enum %s (%4u ms *)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);
		  continue; //c. basic, exist when need (tcpip_tcp_timer)
		}
	
		//a. user driver, always (e.g. _phy_link_timer)
		//c. user app, exist when need (mqtt_cyclic_timer)
		//?. unknow condition!
		if (!strcmp("phy_link_timer", tmptimeout->handler_name))
		  printf(" [%5u.a %s]\t  on enum %s (%4u ms *)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);
		else if (!strcmp("publish_timer", tmptimeout->handler_name)) {
		  printf(" [%5u.a %s]\t  on enum %s (%4u ms *)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);
			
#if MQTT_CLIENT_SUPPORT
//#if MQTT_CLIENT_LINK_TMR_TYPE		==		TYPE_TICKS
		  do {
			  int i, flg = 0;
			  uint32_t same_local_timebase = all_local_time;
			  for (i = 0; i< get_mqtt_publish_topic_number(); i++)
				if (get_publish_expr_time(i))
					flg = 1;
			  if (flg) {
				  printf(" [     \t\t  %u.topics  ]\t  ", get_mqtt_publish_topic_number());
				  for (i = 0; i< get_mqtt_publish_topic_number(); i++)
					  printf("%u. ", get_publish_expr_time(i) - same_local_timebase);
				  printf("\r\n");
			  }
		  } while(0);
//#endif
#endif
		  
		} else if (!strcmp("mqtt_cyclic_timer", tmptimeout->handler_name))
		  printf(" [%5u.c %s]\t  on enum %s (%4u ms *)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);
		else
		  printf(" [%5u.a %s]\t  on enum %s (%4u ms ?)\r\n", ret, hname, hname, 0 ); //tmptimeout->intvl_msec);

	#else
		//if (!strcmp("igmp_tmr", tmptimeout->handler_name) ||
		//	!strcmp("ip_reass_tmr", tmptimeout->handler_name) ||
		//	!strcmp("etharp_tmr", tmptimeout->handler_name) ||
		//	!strcmp("lwip_cyclic_timer", tmptimeout->handler_name) ||
		//	!strcmp("tcpip_tcp_timer", tmptimeout->handler_name))
		/*if (tmptimeout->h == tcpip_tcp_timer ||
			tmptimeout->h == lwip_cyclic_timer) */
		
		if (tmptimeout->h == lwip_cyclic_timer ||
			tmptimeout->h == tcpip_tcp_timer)
		  printf(" [%5u.b]\t  (%u ms)\r\n", ret, tmptimeout->intvl_msec);
		else
		  printf(" [%5u.a]\t  (%u ms)\r\n", ret, tmptimeout->intvl_msec);
	#endif
		
		/* Repeat until all timers have been count */
	  } while (1);

	  printf("  %d tmrs\r\n", n);
#endif
}

void sys_print_timeouts_times0(int n) //JJ.tmp
{
	new_total_timeouts(n);
}

void sys_print_timeouts_times1(const char *headstr, int n) //JJ.tmp
{
  if (new_total_timeouts(n)) {
	/* split to two, for easy log-reading */  
  //printf(" [tmrs number change at : %s]\r\n", headstr);
	//printf("%s : . . . tmrs change . . . . .  . . . . . . . . . %d tmrs . . (lwip_num_cyclic_timers %d)\r\n",
	//	 headstr, n, timeouts_init_num_cyclic_timers);
	printf("%s : . . . tmrs change . . .\r\n", headstr);
	sys_print_basic_lwip(headstr);
	//.sys_print_add_on_mqtt(headstr);
  }
}
#endif
#endif //MQTT_DEBUG_SYS_TIMEOUTS_SHOW 
 //if we are to use, migrated to such as "netconf2.c"
 //after that, migrated to shch as "dm9051_env.c"
 //after that, migrated to shch as "netconf2 - SHOWc"
