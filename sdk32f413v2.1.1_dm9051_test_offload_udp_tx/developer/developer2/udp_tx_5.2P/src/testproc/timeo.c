#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_DHCP /* don't build if not configured for use in lwipopts.h */
#include "lwip/stats.h"
#include "lwip/mem.h"
#include "lwip/udp.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip_addr.h" //.#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/def.h"
#include "lwip/dhcp.h"
//void dhcp_new_jj(void);
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "lwip/etharp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/prot/iana.h"
#endif //LWIP_IPV4 && LWIP_DHCP

#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "testproc/testproc_lw.h" //#include "dm9051_lw_testproc.h"
#include "testproc/timeo.h"
#include "tmr_init.h"

#define sys_timeout(msecs, handler, arg) sys_timeout_debug(msecs, handler, arg, #handler)

#if 1
#define NETIF_FLAG_LINK_UP      0x04U
#define netif_set_flags1(flags, set_flags)     do { flags = (uint8_t)(flags | (set_flags)); } while(0)
#define netif_clear_flags1(flags, clr_flags)   do { flags = (uint8_t)(flags & (uint8_t)(~(clr_flags) & 0xff)); } while(0)

#define netif_is_link_up1(flags) ((flags & NETIF_FLAG_LINK_UP) ? (uint8_t)1 : (uint8_t)0)
#endif

net_t __net_instance;
uint8_t netflags = 0;

void
netif_set_link_up1(void)
{
  if (!(netflags & NETIF_FLAG_LINK_UP))
	  netif_set_flags1(netflags, NETIF_FLAG_LINK_UP);
}

void
netif_set_link_down1(void)
{
  if (netflags & NETIF_FLAG_LINK_UP)
	  netif_clear_flags1(netflags, NETIF_FLAG_LINK_UP);
}

#if 1
//
// "sys_timeout.c" (timeouts.c)
//

struct sys_timeo mempfixed_desc_tab; //Global.

void *memp_malloc(memp_t type)
{
  void *memp;
  memp = &mempfixed_desc_tab;
#if 0
  memp = do_memp_malloc_pool(memp_pools[type]); //type = 'MEMP_SYS_TIMEOUT'
#endif
  return memp;
}

void
memp_free(memp_t type, void *mem)
{
  if (mem == NULL)
    return;
#if 0
  do_memp_free_pool(memp_pools[type], mem);
#endif
}

void phy_link_timer(void *arg)
{
  net_t *net = arg;
	uint8_t chip_link_up = (dm9051_link_update() & 0x0004) ? 1 : 0;
	
  if(!netif_is_link_up1(netflags)) {
    if (chip_link_up) {
		net->link_state = NET_LINK_UP;
		net->cbf(net, net->net_arg, NET_LINK_UP);
	}
  } else if (netif_is_link_up1(netflags)) {
	if (!chip_link_up) {
		net->link_state = NET_LINK_DOWN;
		net->cbf(net, net->net_arg, NET_LINK_DOWN);
	}
  }

#if 1  
  sys_timeout(MQTT_CLIENT_LINK_TMR_MS, phy_link_timer, net);
#endif
}

//static void net_new_task(net_t *net, net_link_cb_t cb, void *arg)
//{
//  net->link_state = NET_LINK_DOWN; //net->cyclic_tick = 0;
//  net->cbf = cb;
//  net->net_arg = arg;
//  sys_timeout(MQTT_CLIENT_LINK_TMR_MS, phy_link_timer, net);
//}

void netlink_init(void) // (are to phase-in, NOW start API-skelton.)
{
  //net_new_task(&__net_instance, linkup_cb, NULL);
	__net_instance.link_state = NET_LINK_DOWN;
	__net_instance.cbf = linkup_cb;
	__net_instance.net_arg = NULL;
  /* Start cyclic link_handle timer */
#if 1
  sys_timeout(MQTT_CLIENT_LINK_TMR_MS, phy_link_timer, &__net_instance); //(no sys_untimeout(), THIS IS FOREEVER LOOP CYCLE, such as 'link_handle()')
#endif
}
#endif
