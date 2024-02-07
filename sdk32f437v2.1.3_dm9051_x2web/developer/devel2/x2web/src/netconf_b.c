/**
  **************************************************************************
  * @file     netconf_b.c
  * @version  v1.0
  * @date     2023-04-28
  * @brief    network connection configuration
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
#include <stdint.h>
#include "stdio.h"
#include "dm9051_lw_conf.h"
#include "developer_conf.h"
//#include "tmr_init.h"
#include "netconf.h"
#include "netconf_b.h"
//#include "timeouts.h"
#include "dm9051_lw.h"
#include "test/test_state.h" //driver also
#include "testproc/testproc_lw.h"

typedef enum {
	SPRING_TYPE,
	RE_TYPE,
} lnktmr_t;

static void linktmr_cb(net_t *net, void *arg, uint8_t status, lnktmr_t type) {
  int pin = net->netif_pin_code;
  if (status == NET_LINK_UP) {
	  printf("dm9051[%d] link up\r\n", pin);
	  //if (net->link_state == NET_LINK_DOWN)
	  net->link_state = NET_LINK_UP;
	  if (type == SPRING_TYPE)	netif_set_flags(&xnetif[pin], NETIF_FLAG_LINK_UP);
	  if (type == RE_TYPE)		netif_set_link_up(&xnetif[pin]);
  }
  if (status == NET_LINK_DOWN) {
	  printf(" ... down\r\n"); //printf("dm9051_spring_test[%d] link down\r\n", 0);
	  //if (net->link_state == NET_LINK_UP)
	  net->link_state = NET_LINK_DOWN;
	  if (type == SPRING_TYPE)	netif_clear_flags(&xnetif[pin], NETIF_FLAG_LINK_UP);
	  if (type == RE_TYPE)		netif_set_link_down(&xnetif[pin]);
  }
}

void linkup_spring_cb(net_t *net, void *arg, u8_t status) {
  int pin = net->netif_pin_code;
  linktmr_cb(net, arg, status, SPRING_TYPE);
	
  if (status == NET_LINK_UP) {
	tpp_set_linkup_stage(); //gtestproc_stage = _TST_LINKUP_FOUND;
	if (tpp_get_link_state()) {
	  waitdone_set_true(pin); //printf("dm9051[%d] link up\r\n", pin);
	}
  }
  if (status == NET_LINK_DOWN) {
  #if LWIP_TESTMODE // && LWIP_TESTMODE_REAL
	if (!tp_all_done()) {
		display_state();
		tpp_set_linkdown_stage();
		if (!tpp_recognized_stage())
		  printf("dm9051_spring_testt[%d] link down\r\n", pin);
	}
  #endif
  }
  
  //if (status == NET_LINK_UP)
	//netif_set_flags(&xnetif[pin], NETIF_FLAG_LINK_UP);
  //if (status == NET_LINK_DOWN)
	//netif_clear_flags(&xnetif[pin], NETIF_FLAG_LINK_UP);
}

void linkup_re_cb(net_t *net, void *arg, u8_t status) {
  int pin = net->netif_pin_code;
  linktmr_cb(net, arg, status, RE_TYPE);
	
  if (status == NET_LINK_UP) {
	tpp_set_linkup_stage(); //if (tpp_get_link_state()) { printf("dm9051[%d] link up\r\n", pin);}
   #if LWIP_NETIF_LINK_CALLBACK == 0
	env_ethernetif_update_config_cb(&xnetif[pin]);
   #endif
  }
  if (status == NET_LINK_DOWN) {
   #if LWIP_TESTMODE // && LWIP_TESTMODE_REAL
	if (!tp_all_done()) {
		display_state();
		tpp_set_linkdown_stage();
		if (!tpp_recognized_stage())
		  printf("dm9051_re_testt[%d] link down\r\n", pin);
	}
   #endif
	#if MQTT_CLIENT_SUPPORT
	mqttc_close(); //= _mqtt_close(client, MQTT_CONNECT_DISCONNECTED);
	#endif
  }
  
  //if (status == NET_LINK_UP)
	//netif_set_link_up(&xnetif[pin]); //netif_set_link_up_INITAndTimer(&netif);
  //if (status == NET_LINK_DOWN)
	//netif_set_link_down(&xnetif[pin]); //netif_set_link_down_Timer(&netif);
}

/*static*/
#define MONITER_NUM 3
static int nMoniter_phy_link_timer[ETHERNET_COUNT] = { 3 };
static int xcarry[ETHERNET_COUNT] = { 0 }, xc[ETHERNET_COUNT] = { 0 };

void func_link_timer(net_t *net, uint16_t bmsr) {
  #define CARRY_NUM 2 //3 //7
  #define CYC_NUM 3 //5 //30
  uint8_t chip_link_up = bmsr & 0x0004; //= (uint8_t) dm9051_link_update_dual(); //.dm9051_bmsr_update();
	
  int pin = mstep_get_net_index(); //= net->netif_pin_code;
  if (nMoniter_phy_link_timer[pin]) {
	printf("&xnetif[%d][%02d] bmsr %04x, Monitr_timeouts %d. link_timer still running..\r\n", pin, xcarry[pin], bmsr, nMoniter_phy_link_timer[pin]);
	nMoniter_phy_link_timer[pin]--;
  }

  if (net->link_state == NET_LINK_DOWN) {
		if (xcarry[pin] < CARRY_NUM) {
			xc[pin]++;
			if (xc[pin] == CYC_NUM) {
				xc[pin] = 0;
				xcarry[pin]++;
				if (xcarry[pin] == CARRY_NUM)
					printf("dm9051[%d] link down\r\n", pin);
			}
		}
  } else {
		xc[pin] = 0;
		xcarry[pin] = 0;
  }
  
  if (net->link_state == NET_LINK_DOWN)
  {
    if (chip_link_up) {
		xcarry[pin] = 0; //[for print-log below,]
		printf("dm9051[%d] up, %u.%u.%u.%u (bmsr %04x)\r\n", pin, 
			ip4_addr1_16(netif_ip4_addr(&xnetif[pin])), 
			ip4_addr2_16(netif_ip4_addr(&xnetif[pin])),
			ip4_addr3_16(netif_ip4_addr(&xnetif[pin])), 
			ip4_addr4_16(netif_ip4_addr(&xnetif[pin])),
			bmsr);
		net->cbf(net, net->net_arg, NET_LINK_UP);
	}
  }
  else if (net->link_state == NET_LINK_UP)
  {
    if (!chip_link_up) {
		net->cbf(net, net->net_arg, NET_LINK_DOWN);
	}
  }
}

/*static*/ 
void phy_link_timer(void *arg) //=to_time_link() (are to phase-out, BUT keep called API-exist.)
{
  uint16_t bmsr;
  int pinsave = mstep_get_net_index(); //HERE save: pin = mstep_get_net_index();
  mstep_set_net_index(((net_t *)arg)->netif_pin_code); //HERE SET: mstep_set_net_index(x);
  bmsr = dm9051_bmsr_update();
  func_link_timer(arg, bmsr); //.op_link_timer(pincode, net);
	
  if (linkhandler_type() == TYPE_TIMEROUTS)
	sys_timeout(get_link_handler_ms(), phy_link_timer, arg);

  onchange_timeouts_log("in-phy_link_timer");
  mstep_set_net_index(pinsave); //HERE restore: pin = mstep_get_net_index();
}

//static
void net_new_task(net_t *net, net_link_cb_t cb, void *arg) //(void *arg)= NULL
{
  int pin = mstep_get_net_index(); //mstep_get_net_index();

  nMoniter_phy_link_timer[pin] = MONITER_NUM;
  xcarry[pin] = xc[pin] = 0;

  net->netif_pin_code = pin;
  net->link_state = NET_LINK_DOWN; //net->cyclic_tick = 0;
  net->net_arg = arg;
  net->cbf = cb;
	
  /* Start cyclic link_handle timer */
  sys_timeout(get_link_handler_ms(), phy_link_timer, net); //(no sys_untimeout(), THIS IS FOREEVER LOOP CYCLE, such as 'link_handle()')
}

//static
void net_renew_task(net_t *net, net_link_cb_t cb) //(, void *arg)
{
  net->cbf = cb;
}
