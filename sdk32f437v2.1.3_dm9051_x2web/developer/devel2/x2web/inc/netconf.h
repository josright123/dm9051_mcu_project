/**
  **************************************************************************
  * @file     netconf.h
  * @version  v2.1.2
  * @date     2022-08-16
  * @brief    This file contains all the functions prototypes for the netconf.c
  *           file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETCONF_H
#define __NETCONF_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "ethernetif.h"
//#include "_dm9051_lw_conf.h"

extern struct netif xnetif[ETHERNET_COUNT];
extern int n_verify_id;
	
enum {
  NET_LINK_DOWN,
  NET_LINK_UP
};

typedef struct net_s net_t;
typedef void (*net_link_cb_t)(net_t *net, void *arg, u8_t status);

struct net_s
{
  /** from 'net_' bind to 'ethernetif_' to 'ETHERNET_COUNT_' */
  int netif_pin_code;
  /** link state */
  u8_t link_state;
  /** net callback */
  void *net_arg;
  net_link_cb_t cbf; //=net_cb;
};

/* Includes ------------------------------------------------------------------*/
void tcpip_stack_init(void);
void since_tcpip_stack_init(void);
//static void lwip_pkt_handle(void);

err_t lwip_rx_hdlr(int i);
void lwip_rx_loop_handler(void);  /* (Called by main()'s for-loop / "main.c" */

void lwip_periodic_handle(void);
void publish_handle1(void); //void publish_handle(void);

//#if LINK_DETECTION_NET
void waitdone_set_true(int i);
void netlink_spring_wait(void);
void netlink_spring_init(void); // (are to phase-in, NOW start API-skelton.)
void netlink_reinit(void);
//#endif
void linkup_spring_cb(net_t *net, void *arg, u8_t status);
void linkup_re_cb(net_t *net, void *arg, u8_t status);

//err_t publish_module_init(void);
void publish_new_task(void *pubdata, void *null_cb, void *arg);
void publish_end_task(void *pubdata, void *null_cb, void *arg);

/* A function env_ethernetif_update_config_cb(struct netif *netif)
 */
//.void env_ethernetif_update_config_cb(struct netif *netif); /* Used by in ._tcpip_stack_init() */
 
/* A function env_ethernetif_set_link_Timer(void const *argument)
 */
//void env_ethernetif_set_link_Timer(void const *argument); /* Called by _lwip_periodic_handle() */

#ifdef __cplusplus
}
#endif

#endif /* __NETCONF_H */



