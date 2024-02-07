//#include "dm9051_lw.h" 
#include "dm9051_lw_conf.h"

//#include "at32_cm4_device_support.h" //#include "at32f415.h" //"at32f435_437.h" //"at32f413.h" //#include "at32f415_board.h" //#include "at32f415_board.h"
#include "developer_conf.h" //#include "main.h" //#include "developer_conf.h"
#include "netconf.h"
#include "testproc/testproc_lw.h"

#if 1
/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
static void ethernetif_notify_conn_changed(struct netif *netif)
{
  /* NOTE : This is function could be implemented in user file 
			when the callback is needed,
  */
  if (netif_is_link_up(netif)) {
	netif_set_up(netif);

#if LWIP_DHCP
	/*  Creates a new DHCP client for this interface on the first call.
	Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	the predefined regular intervals after starting the client.
	You can peek in the netif->dhcp struct for the actual DHCP status.*/
	printf("dhcp_start(netif)...\r\n"); //........svnwrkj
	dhcp_start(netif);
#endif
	  
#if MQTT_CLIENT_SUPPORT
	// param: 1, start from dm9051_bmsr_update() is true, and then _netif_set_link_up_INITAndTimer, and then NETIF_LINK_CALLBACK() ... and ...
#endif
  }
  else
	netif_set_down(netif);
}

/**
  * @brief  Link callback function, this function is called on change of link status
  *         to update low level driver configuration.
  * @param  netif: The network interface
  * @retval None
  *
  * Called by _tcpip_stack_init() / "netconf.c"
  *
  */
static void env_ethernetif_update_config_cb(struct netif *netif) //tobe = env_linkchang_cb
{
  if(netif_is_link_up(netif))
	delay_ms(300);

  ethernetif_notify_conn_changed(netif);
}

extern char *display_identity_bannerline_title;  //= 

static void cleararpflag_netif_set_up(struct netif *netif) {
	//u8_t savflg = netif->flags;

	netif_clear_flags(netif, NETIF_FLAG_ETHARP);
	netif_set_up(netif);
	//[tobe observed] netif_clear_flags(netif, NETIF_FLAG_UP);

	//netif->flags = savflg;
}

static void setarpflag_for_ever_netif_set_up(struct netif *netif) {
	netif_set_flags(netif, NETIF_FLAG_ETHARP);
	netif_set_flags(netif, NETIF_FLAG_UP);
}

/**
  * @brief  initializes the lwip stack
  * @param  none
  * @retval none
  */
void tcpip_stack_init(void)
{
  int pin = mstep_get_net_index();
  /* 1. Early */
  /* In netif->flags, NETIF_FLAG_LINK_UP is run-tim flag, early setting here, for lwip_init(), so than netif_init() ... */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  //. netif.flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	//netif.flags = 0;
//	if (netif.link_callback)
//		printf("[.] _tcpip_stack_init.s : netif.flags = %02x, %s\r\n", netif.flags, (netif.link_callback) ? "yes lnkchg_cb" : "no lnkchg_cb");
	//. by _low_level_init

  /* 2. Early */
  /* To must early, than lwip_init(), so than netif_init() */ 
  /* In order, netif_init() use _netif_set_link_up_JJINITAndTimerUsed() to call */
  /* NETIF_LINK_CALLBACK() who can call the callback func= env_ethernetif_update_config_cb() */
  /* Set the link callback function, this function is called on change of link status*/
  //. netif_set_link_callback(&netif, env_ethernetif_update_config_cb);
//  if (netif.link_callback)
//	printf("[.] _tcpip_stack_init.do : netif.flags = %02x, %s\r\n", netif.flags, (netif.link_callback) ? "yes lnkchg_cb" : "no lnkchg_cb");
	
	//bannerline();
	//printf(": tcpip_stack_init[%d]\r\n", pin); =
	display_identity_bannerline_title = ": tcpip_stack_init";
	
  if (1) {
	  /* 3. stack_start_core()= */
	  const uint8_t *mac_address; //uint8_t mac_address[MAC_ADDR_LENGTH];
	  ip_addr_t cfg_ipaddr;
	  ip_addr_t cfg_netmask;
	  ip_addr_t cfg_gw;
	  
	#if 0
	  //.onchange_timeouts_log("lwip_init-end");
	#endif

	#if LWIP_DHCP  //need DHCP server
	  #if LWIP_IPV4 && LWIP_IPV6
	  cfg_ipaddr.u_addr.ip4.addr = 0; //.addr = 0;
	  cfg_netmask.u_addr.ip4.addr = 0; //netmask.addr = 0;
	  cfg_gw.u_addr.ip4.addr = 0; //gw.addr = 0;
	  #else
	  cfg_ipaddr.addr = 0; //.addr = 0;
	  cfg_netmask.addr = 0; //netmask.addr = 0;
	  cfg_gw.addr = 0; //gw.addr = 0;
	  #endif
	#else
	  #ifdef ETHERNETIF_COPY
	  do {
		uint8_t ipv4_addr[ADDR_LENGTH];
	    lwip_copy_ip_addresse(ipv4_addr);
	    IP4_ADDR(&cfg_ipaddr, ipv4_addr[0], ipv4_addr[1], ipv4_addr[2], ipv4_addr[3]);
	    lwip_copy_mask_addresse(ipv4_addr);
	    IP4_ADDR(&cfg_netmask, ipv4_addr[0], ipv4_addr[1], ipv4_addr[2], ipv4_addr[3]); //cfg_local_mask[0], cfg_local_mask[1], cfg_local_mask[2], cfg_local_mask[3]
	    lwip_copy_qw_addresse(ipv4_addr);
	    IP4_ADDR(&cfg_gw, ipv4_addr[0], ipv4_addr[1], ipv4_addr[2], ipv4_addr[3]); //cfg_local_gw[0], cfg_local_gw[1], cfg_local_gw[2], cfg_local_gw[3]);
	  } while(0);
	  #else
	  do { /* should open this implement source file, so can use as changeable in "_developer_conf.h" */
	    //extern uint8_t _cfg_local_ipe[ADDR_LENGTH];
	    //extern uint8_t _cfg_local_gwe[ADDR_LENGTH];
	    //extern uint8_t _cfg_local_maske[ADDR_LENGTH];
		const uint8_t *dev;
		#if LWIP_IPV4 && LWIP_IPV6
		dev = lwip_get_ip_addresse();
	    IP4_ADDR(&cfg_ipaddr.u_addr.ip4, dev[0], dev[1], dev[2], dev[3]);
		dev = lwip_get_qw_addresse();
	    IP4_ADDR(&cfg_gw.u_addr.ip4, dev[0], dev[1], dev[2], dev[3]);
		dev = lwip_get_mask_addresse();
	    IP4_ADDR(&cfg_netmask.u_addr.ip4, dev[0], dev[1], dev[2], dev[3]);
		#else
		dev = lwip_get_ip_addresse();
	    IP4_ADDR(&cfg_ipaddr, dev[0], dev[1], dev[2], dev[3]);
		dev = lwip_get_qw_addresse();
	    IP4_ADDR(&cfg_gw, dev[0], dev[1], dev[2], dev[3]);
		dev = lwip_get_mask_addresse();
	    IP4_ADDR(&cfg_netmask, dev[0], dev[1], dev[2], dev[3]);
		#endif
	  } while(0);
	  #endif
	#endif

	  //lwip_get_mac_addresse0(mac_address);
	  mac_address = lwip_get_mac_addresse1();
	  lwip_set_mac(mac_address); //tcpip_set_mac_address(); //_lwip_set_mac_address(mac_address);

	  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
				struct ip_addr *netmask, struct ip_addr *gw,
				void *state, err_t (* init)(struct netif *netif),
				err_t (* input)(struct pbuf *p, struct netif *netif))

	   Adds your network interface to the netif_list. Allocate a struct
	  netif and pass a pointer to this structure as the first argument.
	  Give pointers to cleared ip_addr structures when using DHCP,
	  or fill them with sane numbers otherwise. The state pointer may be NULL.

	  The init function pointer must point to a initialization function for
	  your ethernet netif interface. The following code illustrates it's use.*/

	//printf(".netif_add()\r\n");
	  if
	#if LWIP_IPV4 && LWIP_IPV6
		  (netif_add(&xnetif[pin], &cfg_ipaddr.u_addr.ip4, &cfg_netmask.u_addr.ip4, &cfg_gw.u_addr.ip4, NULL, &ethernetif_init, &netif_input) == NULL)
	#else
		  (netif_add(&xnetif[pin], &cfg_ipaddr, &cfg_netmask, &cfg_gw, NULL, &ethernetif_init, &netif_input) == NULL)
	#endif
	  {
		while(1);
	  }

	  /*  Registers the default network interface.*/
	  netif_set_default(&xnetif[pin]);

	  /*  When the netif is fully configured this function must be called.*/
#if 1
	//printf("Note: netif is fully configuration.s\r\n");
	printf("  ! A[%d] [Send ARP events, e.g. on link-up/netif-up or addr-change] but we think, only link-up or addr-chang IS enought!\r\n", pin); /** Send ARP/IGMP/MLD/RS events, e.g. on link-up/netif-up or addr-change */  
	cleararpflag_netif_set_up(&xnetif[pin]); //has temp: xnetif[pin].flags &= ~NETIF_FLAG_ETHARP;
	  
	//printf("Note: netif is fully configuration.e\r\n");
#else
	printf("netif_set_up(&xnetif[pin]).s\r\n");
	  netif_set_up(&xnetif[pin]);
	printf("netif_set_up(&xnetif[pin]).e\r\n");
#endif 
  }
  
  /* 3. Post */
  /* To must early, than lwip_init(), so than netif_init() */ 
  /* In order, netif_init() use _netif_set_link_up_JJINITAndTimerUsed() to call */
  /* NETIF_LINK_CALLBACK() who can call the callback func= env_ethernetif_update_config_cb() */
  /* Set the link callback function, this function is called on change of link status*/
 #if LWIP_NETIF_LINK_CALLBACK
  netif_set_link_callback(&xnetif[pin], env_ethernetif_update_config_cb);
 #endif
  //.if (netif.link_callback)
	//.printf("[.] _tcpip_stack_init.e : netif.flags = %02x, %s\r\n", netif.flags, (netif.link_callback) ? "yes lnkchg_cb" : "no lnkchg_cb");

  #if 1
  xnetif[pin].flags &= ~NETIF_FLAG_LINK_UP; // prepare for '_netif_set_link_up_JJINITAndTimerUsed'
  #endif
}

void since_tcpip_stack_init(void)
{
  int pin = mstep_get_net_index();
  printf("  ! B [Send ARP events, can then accepted, and do netif-up] but we think, here do re-init-config is also OK!\r\n"); /** Send ARP/IGMP/MLD/RS events, e.g. on link-up/netif-up or addr-change */
  setarpflag_for_ever_netif_set_up(&xnetif[pin]);
}
#endif
