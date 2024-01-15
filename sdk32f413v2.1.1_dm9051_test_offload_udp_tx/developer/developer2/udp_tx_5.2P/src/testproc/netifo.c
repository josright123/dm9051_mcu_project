/*
#include <string.h> //#include "lwip/mem.h"
#include "lwip/netif.h"

//
//#include "lwip/etharp.h"
//=

//void dhcp_new_jj(void);
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "lwip/etharp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/prot/iana.h"

#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "testproc/testproc_lw.h" //#include "dm9051_lw_testproc.h"
#include "testproc/ethernetif.h"
*/

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
void dhcp_new_jj(void);
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "lwip/etharp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/prot/iana.h"

#include <string.h>
#endif //LWIP_IPV4 && LWIP_DHCP

#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "testproc/testproc_lw.h" //#include "dm9051_lw_testproc.h"
#include "testproc/ethernetif.h"

#include "testproc/timeo.h"
#include "tmr_init.h"

#define IFNAME0 'd'
#define IFNAME1 'm'

union {
	uint8_t rx;
	uint8_t tx;
} EthBuff[RXBUFF_OVERSIZE_LEN]; //[Single Task project.] not occupied by concurrently used.

//[debug]
extern int testing_tx_count;

#if 1
//.extern union {
//.	uint8_t rx;
//.	uint8_t tx;
//.} EthBuff[RXBUFF_OVERSIZE_LEN]; //[Single Task project.] not occupied by concurrently used.
uint8_t *get_rx_buffer(void) {
	return &EthBuff[0].rx;
}
uint8_t *get_tx_buffer(void) {
	return &EthBuff[0].tx;
}
#endif

err_t
out_packet(outpacket_t opt, uint8_t *buffer, uint16_t l, const uint8_t *update_buffer, uint16_t upate_l)
{
	if (update_buffer) {
		memcpy(buffer, update_buffer, upate_l);
		l = upate_l;

		//.printf("[ temp change to be always %u bytes ]\r\n", l);
	}


	#if 1
	//switch(opt)
	//{}
		if (opt == OUT_TEST_UDP)
		{ //case OUT_TEST_UDP:
		printf("[ Send ip checksum %02x %02x]\r\n", buffer[24], buffer[25]); //IPCL, IPCH
		printf("[ Send udp checksum %02x %02x]\r\n", buffer[40], buffer[41]); //UDCL, UDCH
		} //break;
		if (opt == OUT_TEST_TCP)
		{ //case OUT_TEST_TCP:
		printf("[ Send ip checksum %02x %02x]\r\n", buffer[24], buffer[25]); //IPCL, IPCH
		printf("[ Send tcp checksum %02x %02x]\r\n", buffer[50], buffer[51]); //TCCL, TCCH
		} //break;
	//}
	#endif

#if 0
	p->tot_len= p->len= 384;
	l = 384;
	printf(".to tx len = %d\r\n", l);
	printf("low_level_output, p->tot_len = %u, p->len = %u\r\n", p->tot_len, p->len);
#endif
	  
	testing_tx_count++;
	printf(".to tx (len %d)\r\n", l);
	dm9051_txlog_monitor_tx(2, buffer, l);
	//dm9051_txlog_monitor_tx_all(2, buffer, l); //HEAD_SPC
	  
	  dm9051_tx(buffer, (uint16_t) l); //= dm9051_tx_dual(buffer, (uint16_t) l); //dm9051_tx(.);

	#if 0
	  dm9051_txlog_disp(buffer, l); //= process_txlog_debug(buffer, l);
	#endif

	return ERR_OK;
}

#if 1
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int l = 0;
  uint8_t *buffer = get_tx_buffer(); //&EthBuff[0].tx; //Tx_Buff;
	
  for(q = p; q != NULL; q = q->next)
  {
    memcpy((u8_t*)&buffer[l], q->payload, q->len);
    l = l + q->len;
  }
  
/*0000   ff ff ff ff ff ff 00 c0 ca 34 01 f4 08 00 45 00
0010   01 72 45 09 00 00 40 11 34 73 00 00 00 00 ff ff
0020   ff ff 00 44 00 43 01 5e 24 16 01 01 06 00 85 eb
0030   7c 86 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0040   00 00 00 00 00 00 00 c0 ca 34 01 f4 00 00 00 00
0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0060   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0070   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0080   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0090   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00a0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00b0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00c0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00d0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00e0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00f0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0100   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0110   00 00 00 00 00 00 63 82 53 63 35 01 01 3d 07 01
0120   00 c0 ca 34 01 f4 50 00 74 01 01 39 02 05 c0 3c
0130   2e 64 68 63 70 63 64 2d 38 2e 31 2e 32 3a 4c 69
0140   6e 75 78 2d 35 2e 31 35 2e 33 34 2d 76 37 6c 2b
0150   3a 61 72 6d 76 37 6c 3a 42 43 4d 32 37 31 31 0c
0160   0b 72 61 73 70 62 65 72 72 79 70 69 91 01 01 37
0170   0e 01 79 21 03 06 0c 0f 1a 1c 33 36 3a 3b 77 ff*/
	
  return out_packet(OUT_TEST_UDP, buffer, l, buffer_discover, 384);
}

static void
low_level_init0(struct netif *netif)
{
  /* set MAC hardware address */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;
  netif->hwaddr[0] =  MACaddr[0];
  netif->hwaddr[1] =  MACaddr[1];
  netif->hwaddr[2] =  MACaddr[2];
  netif->hwaddr[3] =  MACaddr[3];
  netif->hwaddr[4] =  MACaddr[4];
  netif->hwaddr[5] =  MACaddr[5];

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
 // dm9051_init_dual(MACaddr); //dm9051_init(MACaddr);
}

err_t ethernetif_init(struct netif *netif)
{
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
	
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  netif->hwaddr_len = ETHARP_HWADDR_LEN;
	
  /* initialize the hardware */
  low_level_init0(netif);
  return ERR_OK;
}
#endif

//
// ip4_addr4.c
//

/* used by IP4_ADDR_ANY and IP_ADDR_BROADCAST in ip_addr.h */
const ip_addr_t ip_addr_any = IPADDR4_INIT(IPADDR_ANY);
const ip_addr_t ip_addr_broadcast = IPADDR4_INIT(IPADDR_BROADCAST);

//
// netif.d
//

static u8_t netif_num; //.PEEK

err_t
netif_input(struct pbuf *p, struct netif *inp)
{
  return ERR_OK;
}

//
// netif.d
//

#if LWIP_IPV4
static int
netif_do_set_ipaddr(struct netif *netif, const ip4_addr_t *ipaddr, ip_addr_t *old_addr)
{
  LWIP_ASSERT("invalid pointer", ipaddr != NULL);
  LWIP_ASSERT("invalid pointer", old_addr != NULL);

  /* address is actually being changed? */
  if (ip4_addr_cmp(ipaddr, netif_ip4_addr(netif)) == 0) {
    ip_addr_t new_addr;
    *ip_2_ip4(&new_addr) = *ipaddr;
    IP_SET_TYPE_VAL(new_addr, IPADDR_TYPE_V4);

    ip_addr_copy(*old_addr, *netif_ip_addr4(netif));

    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif_set_ipaddr: netif address being changed\n"));
    
	printf("netif_do_ip_addr_changed(old_addr, &new_addr);\r\n"); //netif_do_ip_addr_changed(old_addr, &new_addr);

    printf("mib2_remove_ip4(netif);\r\n"); //mib2_remove_ip4(netif);
    printf("mib2_remove_route_ip4(0, netif);\r\n"); //mib2_remove_route_ip4(0, netif);
    /* set new IP address to netif */
    ip4_addr_set(ip_2_ip4(&netif->ip_addr), ipaddr);
    IP_SET_TYPE_VAL(netif->ip_addr, IPADDR_TYPE_V4);
    printf("mib2_add_ip4(netif);\r\n"); //mib2_add_ip4(netif);
    printf("mib2_add_route_ip4(0, netif);\r\n"); //mib2_add_route_ip4(0, netif);

    printf("netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4);\r\n"); //netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4);

    printf("NETIF_STATUS_CALLBACK(netif);\r\n"); //NETIF_STATUS_CALLBACK(netif);
    return 1; /* address changed */
  }
  return 0; /* address unchanged */
}

static int
netif_do_set_netmask(struct netif *netif, const ip4_addr_t *netmask, ip_addr_t *old_nm)
{
  /* address is actually being changed? */
  if (ip4_addr_cmp(netmask, netif_ip4_netmask(netif)) == 0) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    LWIP_ASSERT("invalid pointer", old_nm != NULL);
    ip_addr_copy(*old_nm, *netif_ip_netmask4(netif));
#else
    LWIP_UNUSED_ARG(old_nm);
#endif
    printf("mib2_remove_route_ip4(0, netif);\r\n"); //mib2_remove_route_ip4(0, netif);
    /* set new netmask to netif */
    ip4_addr_set(ip_2_ip4(&netif->netmask), netmask);
    IP_SET_TYPE_VAL(netif->netmask, IPADDR_TYPE_V4);
    printf("mib2_add_route_ip4(0, netif);\r\n"); //mib2_add_route_ip4(0, netif);
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("netif: netmask of interface %c%c set to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
                netif->name[0], netif->name[1],
                ip4_addr1_16(netif_ip4_netmask(netif)),
                ip4_addr2_16(netif_ip4_netmask(netif)),
                ip4_addr3_16(netif_ip4_netmask(netif)),
                ip4_addr4_16(netif_ip4_netmask(netif))));
    return 1; /* netmask changed */
  }
  return 0; /* netmask unchanged */
}

static int
netif_do_set_gw(struct netif *netif, const ip4_addr_t *gw, ip_addr_t *old_gw)
{
  /* address is actually being changed? */
  if (ip4_addr_cmp(gw, netif_ip4_gw(netif)) == 0) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    LWIP_ASSERT("invalid pointer", old_gw != NULL);
    ip_addr_copy(*old_gw, *netif_ip_gw4(netif));
#else
    LWIP_UNUSED_ARG(old_gw);
#endif

    ip4_addr_set(ip_2_ip4(&netif->gw), gw);
    IP_SET_TYPE_VAL(netif->gw, IPADDR_TYPE_V4);
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("netif: GW address of interface %c%c set to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
                netif->name[0], netif->name[1],
                ip4_addr1_16(netif_ip4_gw(netif)),
                ip4_addr2_16(netif_ip4_gw(netif)),
                ip4_addr3_16(netif_ip4_gw(netif)),
                ip4_addr4_16(netif_ip4_gw(netif))));
    return 1; /* gateway changed */
  }
  return 0; /* gateway unchanged */
}
#endif /* LWIP_IPV4*/

#if LWIP_IPV4
/**
 * @ingroup netif_ip4
 * Change IP address configuration for a network interface (including netmask
 * and default gateway).
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 * @param netmask the new netmask
 * @param gw the new default gateway
 */
void
netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask,
               const ip4_addr_t *gw)
{
#if LWIP_NETIF_EXT_STATUS_CALLBACK
  netif_nsc_reason_t change_reason = LWIP_NSC_NONE;
  netif_ext_callback_args_t cb_args;
  ip_addr_t old_nm_val;
  ip_addr_t old_gw_val;
  ip_addr_t *old_nm = &old_nm_val;
  ip_addr_t *old_gw = &old_gw_val;
#else
  ip_addr_t *old_nm = NULL;
  ip_addr_t *old_gw = NULL;
#endif
  ip_addr_t old_addr;
  int remove;

  LWIP_ASSERT_CORE_LOCKED();

  /* Don't propagate NULL pointer (IPv4 ANY) to subsequent functions */
  if (ipaddr == NULL) {
    ipaddr = IP4_ADDR_ANY4;
  }
  if (netmask == NULL) {
    netmask = IP4_ADDR_ANY4;
  }
  if (gw == NULL) {
    gw = IP4_ADDR_ANY4;
  }

  remove = ip4_addr_isany(ipaddr);
  if (remove) {
    /* when removing an address, we have to remove it *before* changing netmask/gw
       to ensure that tcp RST segment can be sent correctly */
    if (netif_do_set_ipaddr(netif, ipaddr, &old_addr)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
      change_reason |= LWIP_NSC_IPV4_ADDRESS_CHANGED;
      cb_args.ipv4_changed.old_address = &old_addr;
#endif
    }
  }
  if (netif_do_set_netmask(netif, netmask, old_nm)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    change_reason |= LWIP_NSC_IPV4_NETMASK_CHANGED;
    cb_args.ipv4_changed.old_netmask = old_nm;
#endif
  }
  if (netif_do_set_gw(netif, gw, old_gw)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    change_reason |= LWIP_NSC_IPV4_GATEWAY_CHANGED;
    cb_args.ipv4_changed.old_gw = old_gw;
#endif
  }
  if (!remove) {
    /* set ipaddr last to ensure netmask/gw have been set when status callback is called */
    if (netif_do_set_ipaddr(netif, ipaddr, &old_addr)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
      change_reason |= LWIP_NSC_IPV4_ADDRESS_CHANGED;
      cb_args.ipv4_changed.old_address = &old_addr;
#endif
    }
  }

#if LWIP_NETIF_EXT_STATUS_CALLBACK
  if (change_reason != LWIP_NSC_NONE) {
    change_reason |= LWIP_NSC_IPV4_SETTINGS_CHANGED;
    netif_invoke_ext_callback(netif, change_reason, &cb_args);
  }
#endif
}
#endif /* LWIP_IPV4*/

#if LWIP_IPV4
/** Dummy IPv4 output function for netifs not supporting IPv4
 */
static err_t
netif_null_output_ip4(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
  LWIP_UNUSED_ARG(netif);
  LWIP_UNUSED_ARG(p);
  LWIP_UNUSED_ARG(ipaddr);

  return ERR_IF;
}
#endif /* LWIP_IPV4 */

struct netif *
netif_add(struct netif *netif,
#if LWIP_IPV4
          const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw,
#endif /* LWIP_IPV4 */
          void *state, netif_init_fn init, netif_input_fn input)
{
#if LWIP_IPV4
  if (ipaddr == NULL) {
    ipaddr = ip_2_ip4(IP4_ADDR_ANY);
  }
  if (netmask == NULL) {
    netmask = ip_2_ip4(IP4_ADDR_ANY);
  }
  if (gw == NULL) {
    gw = ip_2_ip4(IP4_ADDR_ANY);
  }

  /* reset new interface configuration state */
  ip_addr_set_zero_ip4(&netif->ip_addr);
  ip_addr_set_zero_ip4(&netif->netmask);
  ip_addr_set_zero_ip4(&netif->gw);
  netif->output = netif_null_output_ip4;
#endif /* LWIP_IPV4 */
  
  NETIF_SET_CHECKSUM_CTRL(netif, NETIF_CHECKSUM_ENABLE_ALL);
  netif->mtu = 0;
  netif->flags = 0;
  
  //memset(netif->client_data, 0, sizeof(netif->client_data)); //?

  /* remember netif specific state information data */
  netif->state = state;
  netif->num = netif_num;
  netif->input = input;
  
#if LWIP_IPV4
  netif_set_addr(netif, ipaddr, netmask, gw);
#endif /* LWIP_IPV4 */

  /* call user specified initialization function for netif */
  if (init(netif) != ERR_OK) {
    return NULL;
  }
  
  //printf(" netif: added interface %c%c\r\n", netif->name[0], netif->name[1]);
  printf(" addr " "%" U16_F ".%" U16_F ".%" U16_F ".%" U16_F "\r\n",
	ip4_addr1_16(ipaddr), ip4_addr2_16(ipaddr), ip4_addr3_16(ipaddr), ip4_addr4_16(ipaddr));
  printf(" netmask " "%" U16_F ".%" U16_F ".%" U16_F ".%" U16_F "\r\n",
	ip4_addr1_16(netmask), ip4_addr2_16(netmask), ip4_addr3_16(netmask), ip4_addr4_16(netmask));
  printf(" gw " "%" U16_F ".%" U16_F ".%" U16_F ".%" U16_F "\r\n",
	ip4_addr1_16(gw), ip4_addr2_16(gw), ip4_addr3_16(gw), ip4_addr4_16(gw));
  
  return netif;
}

#if 0
//
//
// misc.
//
//

#if 1
//
// ip4_addr4.c
//

/* used by IP4_ADDR_ANY and IP_ADDR_BROADCAST in ip_addr.h */
const ip_addr_t ip_addr_any = IPADDR4_INIT(IPADDR_ANY);
const ip_addr_t ip_addr_broadcast = IPADDR4_INIT(IPADDR_BROADCAST);
#endif

//
// pbuf.c
//

#define EST_PAYLOAD_LEN			  120 //Joseph's estimated reserved
#define SIZEOF_STRUCT_PBUF        LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf))

/* Initialize members of struct pbuf after allocation */
static void
pbuf_init_alloced_pbuf(struct pbuf *p, void *payload, u16_t tot_len, u16_t len, pbuf_type type, u8_t flags)
{
  p->next = NULL;
  p->payload = payload;
  p->tot_len = tot_len;
  p->len = len;
  p->type_internal = (u8_t)type;
  p->flags = flags;
  p->ref = 1;
  p->if_idx = NETIF_NO_INDEX;
  printf("pbuf_alloc1 pbuf_init_alloced_pbuf, p->tot_len = %u, p->len = %u\r\n", tot_len, len);
}

char pbuf_buff[LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF)+EST_PAYLOAD_LEN];
struct dhcp_msg dhcp_msg_buff;

struct pbuf *
pbuf_alloc1(pbuf_layer layer, u16_t length, pbuf_type type)
{
  //struct pbuf *p = &dhcp_msg_buff;
  struct pbuf *p;
  u16_t offset = (u16_t)layer;
	
	//PBUF_TRANSPORT/ PBUF_RAM
  switch (type) {
	  case PBUF_RAM: {
		u16_t payload_len = (u16_t)(LWIP_MEM_ALIGN_SIZE(offset) + LWIP_MEM_ALIGN_SIZE(length));
		mem_size_t alloc_len = (mem_size_t)(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF) + payload_len);

		/* bug #50040: Check for integer overflow when calculating alloc_len */
		if ((payload_len < LWIP_MEM_ALIGN_SIZE(length)) ||
		  (alloc_len < LWIP_MEM_ALIGN_SIZE(length))) {
			return NULL;
		}

		/* If pbuf is to be allocated in RAM, allocate memory for it. */
		p = (struct pbuf *)pbuf_buff; //= mem_malloc(alloc_len);
		if (p == NULL) {
		return NULL;
		}
		printf("pbuf_alloc1 need buffsize %u, prepare-size %d\r\n", length, sizeof(struct dhcp_msg));
		pbuf_init_alloced_pbuf(p, /*LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset)),*/ &dhcp_msg_buff,
							 length, length, type, 0);
        break;
      }
  }
  return p;
}
void
pbuf_realloc(struct pbuf *p, u16_t new_len)
{
  struct pbuf *q;
  u16_t rem_len; /* remaining length */
	
  rem_len = new_len;
	
  q = p;
	
  q->len = rem_len;
  q->tot_len = q->len;
	
  /* q is last packet in chain */
  q->next = NULL;
}

u8_t
pbuf_free(struct pbuf *p)
{
	return 0;
}

//
// udp.c
//

err_t
udp_sendto_if_src_local(struct udp_pcb *pcb, struct pbuf *q,
                  const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif, const ip_addr_t *src_ip)
{
  err_t err;
	
  u8_t ip_proto = IP_PROTO_UDP; //ip_proto = IP_PROTO_UDPLITE; //or
  u8_t ttl = pcb->ttl;
	
  //struct pbuf *q; /* q will be sent down the stack */
  //q = p;
  err = ip_output_if_src(q, src_ip, dst_ip, ttl, pcb->tos, ip_proto, netif);
  return err;
}

err_t
udp_sendto_if_src(struct udp_pcb *pcb, struct pbuf *p,
                  const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif, const ip_addr_t *src_ip)
{
  //struct pbuf *q; /* q will be sent down the stack */
  //q = p;
	
  struct udp_hdr *udphdr;
  struct pbuf *q; /* q will be sent down the stack */
  //.u8_t ip_proto;
  //.u8_t ttl;

#if 0
  if (!IP_ADDR_PCB_VERSION_MATCH(pcb, src_ip) ||
      !IP_ADDR_PCB_VERSION_MATCH(pcb, dst_ip)) {
    return ERR_VAL;
  }
#endif
  
#if LWIP_IPV4 && IP_SOF_BROADCAST
  /*err_t err;
  // broadcast filter?
  if (!ip_get_option(pcb, SOF_BROADCAST) &&
      ip_addr_isbroadcast(dst_ip, netif)) {
    LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("udp_sendto_if: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
    return ERR_VAL;
  }*/
#endif /* LWIP_IPV4 && IP_SOF_BROADCAST */

#if 0
  /* if the PCB is not yet bound to a port, bind it here */
  if (pcb->local_port == 0) {
    err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
    if (err != ERR_OK) {
      return err;
    }
  }
#endif
  
#if 0
  /* packet too large to add a UDP header without causing an overflow? */
  if ((u16_t)(p->tot_len + UDP_HLEN) < p->tot_len) {
    return ERR_MEM;
  }
#endif
  
  /* not enough space to add an UDP header to first pbuf in given p chain? */
  if (pbuf_add_header(p, UDP_HLEN)) {
	  
   printf("? Here pbuf_chain() ...\r\n");
   #if 0
    /* allocate header in a separate new pbuf */
    q = pbuf_alloc(PBUF_IP, UDP_HLEN, PBUF_RAM);
    /* new header pbuf could not be allocated? */
    if (q == NULL) {
      LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("udp_send: could not allocate header\n"));
      return ERR_MEM;
    }
    if (p->tot_len != 0) {
      /* chain header q in front of given pbuf p (only if p contains data) */
      pbuf_chain(q, p);
    }
    /* first pbuf q points to header pbuf */
    LWIP_DEBUGF(UDP_DEBUG,
                ("udp_send: added header pbuf %p before given pbuf %p\n", (void *)q, (void *)p));
   #endif
  } else {
    /* adding space for header within p succeeded */
    /* first pbuf q equals given pbuf */
    q = p;
    LWIP_DEBUGF(UDP_DEBUG, ("udp_send: added header in given pbuf %p\n", (void *)p));
  }
  LWIP_ASSERT("check that first pbuf can hold struct udp_hdr",
              (q->len >= sizeof(struct udp_hdr)));

  /* q now represents the packet to be sent */
  udphdr = (struct udp_hdr *)q->payload;
  udphdr->src = lwip_htons(pcb->local_port);
  udphdr->dest = lwip_htons(dst_port);
  /* in UDP, 0 checksum means 'no checksum' */
  udphdr->chksum = 0x0000;

  /* Multicast Loop? */
#if LWIP_MULTICAST_TX_OPTIONS
  .............jnbkjbkjbk....................
  if (((pcb->flags & UDP_FLAGS_MULTICAST_LOOP) != 0) && ip_addr_ismulticast(dst_ip)) {
    q->flags |= PBUF_FLAG_MCASTLOOP;
  }
#endif

  LWIP_DEBUGF(UDP_DEBUG, ("udp_send: sending datagram of length %"U16_F"\n", q->tot_len));

  if (1)
  {      /* UDP */
    LWIP_DEBUGF(UDP_DEBUG, ("udp_send: UDP packet length %"U16_F"\n", q->tot_len));
    udphdr->len = lwip_htons(q->tot_len);
    /* calculate checksum */
#if CHECKSUM_GEN_UDP
    IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_UDP) {
      /* Checksum is mandatory over IPv6. */
      if (IP_IS_V6(dst_ip) || (pcb->flags & UDP_FLAGS_NOCHKSUM) == 0) {
        u16_t udpchksum;
        {
          udpchksum = ip_chksum_pseudo(q, IP_PROTO_UDP, q->tot_len,
                                       src_ip, dst_ip);
        }

        /* chksum zero must become 0xffff, as zero means 'no checksum' */
        if (udpchksum == 0x0000) {
          udpchksum = 0xffff;
        }
        udphdr->chksum = udpchksum;
      }
    }
#endif /* CHECKSUM_GEN_UDP */
    //.ip_proto = IP_PROTO_UDP;
  }

  /* Determine TTL to use */
  //.ttl = pcb->ttl;
	
  return udp_sendto_if_src_local(pcb, q, dst_ip, dst_port, netif, src_ip);
}

//
// ip4.c
//

err_t
ip4_output_if_src(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
                  u8_t ttl, u8_t tos,
                  u8_t proto, struct netif *netif)
{
#if 1
  //return netif->output(netif, p, dest);
	return netif->output(netif, p, dest);
#endif
}

//
// pbuf.c
//

char head_and_dhcp_msg[SIZEOF_ETH_HDR+sizeof(struct dhcp_msg)]; //struct dhcp_msg dhcp_msg_buff;

/*static u8_t pbuf_add_header_impl(struct pbuf *p, size_t header_size_increment, u8_t force)
{
  void *payload;
  u16_t increment_magnitude;
	
  increment_magnitude = (u16_t)header_size_increment;

  // modify pbuf fields
  payload = &head_and_dhcp_msg[0];
  memcpy((u8_t *)payload + increment_magnitude, p->payload, p->len);
	
  p->payload = payload;
  p->len = (u16_t)(p->len + increment_magnitude);
  p->tot_len = (u16_t)(p->tot_len + increment_magnitude);
	
  return 0;
}*/

//
// udp.c
//

struct udp_pcb udppcbbuf;

struct udp_pcb *
udp_new(void)
{
  struct udp_pcb *pcb;

  pcb = (struct udp_pcb *) &udppcbbuf; //memp_malloc(MEMP_UDP_PCB);
  /* could allocate UDP PCB? */
  if (pcb != NULL) {
    /* UDP Lite: by initializing to all zeroes, chksum_len is set to 0
     * which means checksum is generated over the whole datagram per default
     * (recommended as default by RFC 3828). */
    /* initialize PCB to all zeroes */
    memset(pcb, 0, sizeof(struct udp_pcb));
    pcb->ttl = UDP_TTL;
  }
  return pcb;
}

struct dhcp dhcpstructbuff;

struct dhcp *
create_netif_dhcp_data_jj(void)
{
	dhcpstructbuff.state = DHCP_STATE_INIT;
	dhcpstructbuff.xid = 0; //JJ.INIT!
	return &dhcpstructbuff;
}

//
// pbuf.c
//

#define EST_PAYLOAD_LEN			  120 //Joseph's estimated reserved
#define SIZEOF_STRUCT_PBUF        LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf))

/* Initialize members of struct pbuf after allocation */
static void
pbuf_init_alloced_pbuf(struct pbuf *p, void *payload, u16_t tot_len, u16_t len, pbuf_type type, u8_t flags)
{
  p->next = NULL;
  p->payload = payload;
  p->tot_len = tot_len;
  p->len = len;
  p->type_internal = (u8_t)type;
  p->flags = flags;
  p->ref = 1;
  p->if_idx = NETIF_NO_INDEX;
  printf("pbuf_alloc1 pbuf_init_alloced_pbuf, p->tot_len = %u, p->len = %u\r\n", tot_len, len);
}

char pbuf_buff[LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF)+EST_PAYLOAD_LEN];
struct dhcp_msg dhcp_msg_buff;

struct pbuf *
pbuf_alloc1(pbuf_layer layer, u16_t length, pbuf_type type)
{
  //struct pbuf *p = &dhcp_msg_buff;
  struct pbuf *p;
  u16_t offset = (u16_t)layer;
	
	//PBUF_TRANSPORT/ PBUF_RAM
  switch (type) {
	  case PBUF_RAM: {
		u16_t payload_len = (u16_t)(LWIP_MEM_ALIGN_SIZE(offset) + LWIP_MEM_ALIGN_SIZE(length));
		mem_size_t alloc_len = (mem_size_t)(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF) + payload_len);

		/* bug #50040: Check for integer overflow when calculating alloc_len */
		if ((payload_len < LWIP_MEM_ALIGN_SIZE(length)) ||
		  (alloc_len < LWIP_MEM_ALIGN_SIZE(length))) {
			return NULL;
		}

		/* If pbuf is to be allocated in RAM, allocate memory for it. */
		p = (struct pbuf *)pbuf_buff; //= mem_malloc(alloc_len);
		if (p == NULL) {
		return NULL;
		}
		printf("pbuf_alloc1 need buffsize %u, prepare-size %d\r\n", length, sizeof(struct dhcp_msg));
		pbuf_init_alloced_pbuf(p, /*LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset)),*/ &dhcp_msg_buff,
							 length, length, type, 0);
        break;
      }
  }
  return p;
}
void
pbuf_realloc(struct pbuf *p, u16_t new_len)
{
  struct pbuf *q;
  u16_t rem_len; /* remaining length */
	
  rem_len = new_len;
	
  q = p;
	
  q->len = rem_len;
  q->tot_len = q->len;
	
  /* q is last packet in chain */
  q->next = NULL;
}

u8_t
pbuf_free(struct pbuf *p)
{
	return 0;
}

//
// udp.c
//

err_t
udp_sendto_if_src_local(struct udp_pcb *pcb, struct pbuf *q,
                  const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif, const ip_addr_t *src_ip)
{
  err_t err;
	
  u8_t ip_proto = IP_PROTO_UDP; //ip_proto = IP_PROTO_UDPLITE; //or
  u8_t ttl = pcb->ttl;
	
  //struct pbuf *q; /* q will be sent down the stack */
  //q = p;
  err = ip_output_if_src(q, src_ip, dst_ip, ttl, pcb->tos, ip_proto, netif);
  return err;
}

err_t
udp_sendto_if_src(struct udp_pcb *pcb, struct pbuf *p,
                  const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif, const ip_addr_t *src_ip)
{
  //struct pbuf *q; /* q will be sent down the stack */
  //q = p;
	
  struct udp_hdr *udphdr;
  struct pbuf *q; /* q will be sent down the stack */
  //.u8_t ip_proto;
  //.u8_t ttl;

#if 0
  if (!IP_ADDR_PCB_VERSION_MATCH(pcb, src_ip) ||
      !IP_ADDR_PCB_VERSION_MATCH(pcb, dst_ip)) {
    return ERR_VAL;
  }
#endif
  
#if LWIP_IPV4 && IP_SOF_BROADCAST
  /*err_t err;
  // broadcast filter?
  if (!ip_get_option(pcb, SOF_BROADCAST) &&
      ip_addr_isbroadcast(dst_ip, netif)) {
    LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("udp_sendto_if: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
    return ERR_VAL;
  }*/
#endif /* LWIP_IPV4 && IP_SOF_BROADCAST */

#if 0
  /* if the PCB is not yet bound to a port, bind it here */
  if (pcb->local_port == 0) {
    err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
    if (err != ERR_OK) {
      return err;
    }
  }
#endif
  
#if 0
  /* packet too large to add a UDP header without causing an overflow? */
  if ((u16_t)(p->tot_len + UDP_HLEN) < p->tot_len) {
    return ERR_MEM;
  }
#endif
  
  /* not enough space to add an UDP header to first pbuf in given p chain? */
  if (pbuf_add_header(p, UDP_HLEN)) {
	  
   printf("? Here pbuf_chain() ...\r\n");
   #if 0
    /* allocate header in a separate new pbuf */
    q = pbuf_alloc(PBUF_IP, UDP_HLEN, PBUF_RAM);
    /* new header pbuf could not be allocated? */
    if (q == NULL) {
      LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("udp_send: could not allocate header\n"));
      return ERR_MEM;
    }
    if (p->tot_len != 0) {
      /* chain header q in front of given pbuf p (only if p contains data) */
      pbuf_chain(q, p);
    }
    /* first pbuf q points to header pbuf */
    LWIP_DEBUGF(UDP_DEBUG,
                ("udp_send: added header pbuf %p before given pbuf %p\n", (void *)q, (void *)p));
   #endif
  } else {
    /* adding space for header within p succeeded */
    /* first pbuf q equals given pbuf */
    q = p;
    LWIP_DEBUGF(UDP_DEBUG, ("udp_send: added header in given pbuf %p\n", (void *)p));
  }
  LWIP_ASSERT("check that first pbuf can hold struct udp_hdr",
              (q->len >= sizeof(struct udp_hdr)));

  /* q now represents the packet to be sent */
  udphdr = (struct udp_hdr *)q->payload;
  udphdr->src = lwip_htons(pcb->local_port);
  udphdr->dest = lwip_htons(dst_port);
  /* in UDP, 0 checksum means 'no checksum' */
  udphdr->chksum = 0x0000;

  /* Multicast Loop? */
#if LWIP_MULTICAST_TX_OPTIONS
  .............jnbkjbkjbk....................
  if (((pcb->flags & UDP_FLAGS_MULTICAST_LOOP) != 0) && ip_addr_ismulticast(dst_ip)) {
    q->flags |= PBUF_FLAG_MCASTLOOP;
  }
#endif

  LWIP_DEBUGF(UDP_DEBUG, ("udp_send: sending datagram of length %"U16_F"\n", q->tot_len));

  if (1)
  {      /* UDP */
    LWIP_DEBUGF(UDP_DEBUG, ("udp_send: UDP packet length %"U16_F"\n", q->tot_len));
    udphdr->len = lwip_htons(q->tot_len);
    /* calculate checksum */
#if CHECKSUM_GEN_UDP
    IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_UDP) {
      /* Checksum is mandatory over IPv6. */
      if (IP_IS_V6(dst_ip) || (pcb->flags & UDP_FLAGS_NOCHKSUM) == 0) {
        u16_t udpchksum;
        {
          udpchksum = ip_chksum_pseudo(q, IP_PROTO_UDP, q->tot_len,
                                       src_ip, dst_ip);
        }

        /* chksum zero must become 0xffff, as zero means 'no checksum' */
        if (udpchksum == 0x0000) {
          udpchksum = 0xffff;
        }
        udphdr->chksum = udpchksum;
      }
    }
#endif /* CHECKSUM_GEN_UDP */
    //.ip_proto = IP_PROTO_UDP;
  }

  /* Determine TTL to use */
  //.ttl = pcb->ttl;
	
  return udp_sendto_if_src_local(pcb, q, dst_ip, dst_port, netif, src_ip);
}

//
// ip4.c
//

err_t
ip4_output_if_src(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
                  u8_t ttl, u8_t tos,
                  u8_t proto, struct netif *netif)
{
#if 1
  //return netif->output(netif, p, dest);
	return netif->output(netif, p, dest);
#endif
}

//
// pbuf.c
//

char head_and_dhcp_msg[SIZEOF_ETH_HDR+sizeof(struct dhcp_msg)]; //struct dhcp_msg dhcp_msg_buff;

/*static u8_t pbuf_add_header_impl(struct pbuf *p, size_t header_size_increment, u8_t force)
{
  void *payload;
  u16_t increment_magnitude;
	
  increment_magnitude = (u16_t)header_size_increment;

  // modify pbuf fields
  payload = &head_and_dhcp_msg[0];
  memcpy((u8_t *)payload + increment_magnitude, p->payload, p->len);
	
  p->payload = payload;
  p->len = (u16_t)(p->len + increment_magnitude);
  p->tot_len = (u16_t)(p->tot_len + increment_magnitude);
	
  return 0;
}*/
static u8_t
pbuf_add_header_impl(struct pbuf *p, size_t header_size_increment, u8_t force)
{
  u16_t type_internal;
  void *payload;
  u16_t increment_magnitude;

  LWIP_ASSERT("p != NULL", p != NULL);
  if ((p == NULL) || (header_size_increment > 0xFFFF)) {
    return 1;
  }
  if (header_size_increment == 0) {
    return 0;
  }

  increment_magnitude = (u16_t)header_size_increment;
  /* Do not allow tot_len to wrap as a result. */
  if ((u16_t)(increment_magnitude + p->tot_len) < increment_magnitude) {
    return 1;
  }

  type_internal = p->type_internal;

  /* pbuf types containing payloads? */
  if (type_internal & PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS) {
    /* set new payload pointer */
    payload = (u8_t *)p->payload - header_size_increment;
    /* boundary check fails? */
    if ((u8_t *)payload < (u8_t *)p + SIZEOF_STRUCT_PBUF) {
      LWIP_DEBUGF( PBUF_DEBUG | LWIP_DBG_TRACE,
                   ("pbuf_add_header: failed as %p < %p (not enough space for new header size)\n",
                    (void *)payload, (void *)((u8_t *)p + SIZEOF_STRUCT_PBUF)));
      /* bail out unsuccessfully */
      return 1;
    }
    /* pbuf types referring to external payloads? */
  } else {
    /* hide a header in the payload? */
    if (force) {
      payload = (u8_t *)p->payload - header_size_increment;
    } else {
      /* cannot expand payload to front (yet!)
       * bail out unsuccessfully */
      return 1;
    }
  }
  LWIP_DEBUGF(PBUF_DEBUG | LWIP_DBG_TRACE, ("pbuf_add_header: old %p new %p (%"U16_F")\n",
              (void *)p->payload, (void *)payload, increment_magnitude));

  /* modify pbuf fields */
  p->payload = payload;
  p->len = (u16_t)(p->len + increment_magnitude);
  p->tot_len = (u16_t)(p->tot_len + increment_magnitude);


  return 0;
}

u8_t
pbuf_add_header(struct pbuf *p, size_t header_size_increment)
{
  return pbuf_add_header_impl(p, header_size_increment, 0);
}

//
// ethernet.c
//

err_t
ethernet_output(struct netif * netif, struct pbuf * p,
                const struct eth_addr * src, const struct eth_addr * dst,
                u16_t eth_type) {
  struct eth_hdr *ethhdr;
  u16_t eth_type_be = lwip_htons(eth_type);
  err_t res;
	
  if (1)					
  {
    if (pbuf_add_header(p, SIZEOF_ETH_HDR) != 0) {
      goto pbuf_header_failed;
    }
  }

  ethhdr = (struct eth_hdr *)p->payload;
  ethhdr->type = eth_type_be;
  SMEMCPY(&ethhdr->dest, dst, ETH_HWADDR_LEN);
  SMEMCPY(&ethhdr->src,  src, ETH_HWADDR_LEN);
  
  printf("\r\n");
  printf("netif->linkoutput() -- low_level_output()\r\n");
  /* send the packet */
//printf("J(:%d): inp->flag & CFG_FLGs :ethernet_output.s %x & %x\r\n", LWIP_ETHERNET, netif->flags, (NETIF_FLAG_ETHARP | _NETIF_FLAG_ETHERNET));
  res = netif->linkoutput(netif, p);
//printf("J(:%d): inp->flag & CFG_FLGs :ethernet_output.e %x & %x\r\n", LWIP_ETHERNET, netif->flags, (NETIF_FLAG_ETHARP | _NETIF_FLAG_ETHERNET));
  return res;

pbuf_header_failed:
  LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
              ("ethernet_output: could not allocate room for header.\n"));
  printf("LINK_STATS_INC(link.lenerr);\r\n"); //LINK_STATS_INC(link.lenerr);
  return ERR_BUF;
}

//
// etharp.c
//

static const struct eth_addr ethbroadcast = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

err_t
etharp_output(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
	const struct eth_addr *dest;
	#if 1
	/* broadcast on Ethernet also */
    dest = (const struct eth_addr *)&ethbroadcast;
	#endif
	#if 0
	...........
	#endif
	return ethernet_output(netif, q, (struct eth_addr *)(netif->hwaddr), dest, ETHTYPE_IP);
}

//
// udp.c
//

struct udp_pcb udppcbbuf;

struct udp_pcb *
udp_new(void)
{
  struct udp_pcb *pcb;

  pcb = (struct udp_pcb *) &udppcbbuf; //memp_malloc(MEMP_UDP_PCB);
  /* could allocate UDP PCB? */
  if (pcb != NULL) {
    /* UDP Lite: by initializing to all zeroes, chksum_len is set to 0
     * which means checksum is generated over the whole datagram per default
     * (recommended as default by RFC 3828). */
    /* initialize PCB to all zeroes */
    memset(pcb, 0, sizeof(struct udp_pcb));
    pcb->ttl = UDP_TTL;
  }
  return pcb;
}
#endif

// ------------------------------- . ---------------------------------------

//
// pbuf.c
//

#define SIZEOF_STRUCT_PBUF        LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf))

static u8_t
pbuf_add_header_impl(struct pbuf *p, size_t header_size_increment, u8_t force)
{
  u16_t type_internal;
  void *payload;
  u16_t increment_magnitude;

  LWIP_ASSERT("p != NULL", p != NULL);
  if ((p == NULL) || (header_size_increment > 0xFFFF)) {
    return 1;
  }
  if (header_size_increment == 0) {
    return 0;
  }

  increment_magnitude = (u16_t)header_size_increment;
  /* Do not allow tot_len to wrap as a result. */
  if ((u16_t)(increment_magnitude + p->tot_len) < increment_magnitude) {
    return 1;
  }

  type_internal = p->type_internal;

  /* pbuf types containing payloads? */
  if (type_internal & PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS) {
    /* set new payload pointer */
    payload = (u8_t *)p->payload - header_size_increment;
    /* boundary check fails? */
    if ((u8_t *)payload < (u8_t *)p + SIZEOF_STRUCT_PBUF) {
      LWIP_DEBUGF( PBUF_DEBUG | LWIP_DBG_TRACE,
                   ("pbuf_add_header: failed as %p < %p (not enough space for new header size)\n",
                    (void *)payload, (void *)((u8_t *)p + SIZEOF_STRUCT_PBUF)));
      /* bail out unsuccessfully */
      return 1;
    }
    /* pbuf types referring to external payloads? */
  } else {
    /* hide a header in the payload? */
    if (force) {
      payload = (u8_t *)p->payload - header_size_increment;
    } else {
      /* cannot expand payload to front (yet!)
       * bail out unsuccessfully */
      return 1;
    }
  }
  LWIP_DEBUGF(PBUF_DEBUG | LWIP_DBG_TRACE, ("pbuf_add_header: old %p new %p (%"U16_F")\n",
              (void *)p->payload, (void *)payload, increment_magnitude));

  /* modify pbuf fields */
  p->payload = payload;
  p->len = (u16_t)(p->len + increment_magnitude);
  p->tot_len = (u16_t)(p->tot_len + increment_magnitude);


  return 0;
}

u8_t
pbuf_add_header(struct pbuf *p, size_t header_size_increment)
{
  return pbuf_add_header_impl(p, header_size_increment, 0);
}

//
// ethernet.c
//

err_t
ethernet_output(struct netif * netif, struct pbuf * p,
                const struct eth_addr * src, const struct eth_addr * dst,
                u16_t eth_type) {
  struct eth_hdr *ethhdr;
  u16_t eth_type_be = lwip_htons(eth_type);
  err_t res;
	
  if (1)					
  {
    if (pbuf_add_header(p, SIZEOF_ETH_HDR) != 0) {
      goto pbuf_header_failed;
    }
  }

  ethhdr = (struct eth_hdr *)p->payload;
  ethhdr->type = eth_type_be;
  SMEMCPY(&ethhdr->dest, dst, ETH_HWADDR_LEN);
  SMEMCPY(&ethhdr->src,  src, ETH_HWADDR_LEN);
  
  printf("\r\n");
  printf("netif->linkoutput() -- low_level_output()\r\n");
  /* send the packet */
//printf("J(:%d): inp->flag & CFG_FLGs :ethernet_output.s %x & %x\r\n", LWIP_ETHERNET, netif->flags, (NETIF_FLAG_ETHARP | _NETIF_FLAG_ETHERNET));
  res = netif->linkoutput(netif, p);
//printf("J(:%d): inp->flag & CFG_FLGs :ethernet_output.e %x & %x\r\n", LWIP_ETHERNET, netif->flags, (NETIF_FLAG_ETHARP | _NETIF_FLAG_ETHERNET));
  return res;

pbuf_header_failed:
  LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
              ("ethernet_output: could not allocate room for header.\n"));
  printf("LINK_STATS_INC(link.lenerr);\r\n"); //LINK_STATS_INC(link.lenerr);
  return ERR_BUF;
}

//
// etharp.c
//

static const struct eth_addr ethbroadcast = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

err_t
etharp_output(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
	const struct eth_addr *dest;
	#if 1
	/* broadcast on Ethernet also */
    dest = (const struct eth_addr *)&ethbroadcast;
	#endif
	#if 0
	...........
	#endif
	return ethernet_output(netif, q, (struct eth_addr *)(netif->hwaddr), dest, ETHTYPE_IP);
}

struct dhcp dhcpstructbuff;

struct dhcp *
create_netif_dhcp_data_jj(void)
{
	dhcpstructbuff.state = DHCP_STATE_INIT;
	dhcpstructbuff.xid = 0; //JJ.INIT!
	return &dhcpstructbuff;
}

//
//
// misc 2
//
//

//
// ip4.c
//

err_t
ip4_output_if_src(struct pbuf *p, const ip4_addr_t *src, const ip4_addr_t *dest,
                  u8_t ttl, u8_t tos,
                  u8_t proto, struct netif *netif)
{
#if 1
  //return netif->output(netif, p, dest);
	return netif->output(netif, p, dest);
#endif
}

//
// udp.c
//

err_t
udp_sendto_if_src_local(struct udp_pcb *pcb, struct pbuf *q,
                  const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif, const ip_addr_t *src_ip)
{
  err_t err;
	
  u8_t ip_proto = IP_PROTO_UDP; //ip_proto = IP_PROTO_UDPLITE; //or
  u8_t ttl = pcb->ttl;
	
  //struct pbuf *q; /* q will be sent down the stack */
  //q = p;
  err = ip_output_if_src(q, src_ip, dst_ip, ttl, pcb->tos, ip_proto, netif);
  return err;
}

err_t
udp_sendto_if_src(struct udp_pcb *pcb, struct pbuf *p,
                  const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif, const ip_addr_t *src_ip)
{
  //struct pbuf *q; /* q will be sent down the stack */
  //q = p;
	
  struct udp_hdr *udphdr;
  struct pbuf *q; /* q will be sent down the stack */
  //.u8_t ip_proto;
  //.u8_t ttl;

#if 0
  if (!IP_ADDR_PCB_VERSION_MATCH(pcb, src_ip) ||
      !IP_ADDR_PCB_VERSION_MATCH(pcb, dst_ip)) {
    return ERR_VAL;
  }
#endif
  
#if LWIP_IPV4 && IP_SOF_BROADCAST
  /*err_t err;
  // broadcast filter?
  if (!ip_get_option(pcb, SOF_BROADCAST) &&
      ip_addr_isbroadcast(dst_ip, netif)) {
    LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("udp_sendto_if: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
    return ERR_VAL;
  }*/
#endif /* LWIP_IPV4 && IP_SOF_BROADCAST */

#if 0
  /* if the PCB is not yet bound to a port, bind it here */
  if (pcb->local_port == 0) {
    err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
    if (err != ERR_OK) {
      return err;
    }
  }
#endif
  
#if 0
  /* packet too large to add a UDP header without causing an overflow? */
  if ((u16_t)(p->tot_len + UDP_HLEN) < p->tot_len) {
    return ERR_MEM;
  }
#endif
  
  /* not enough space to add an UDP header to first pbuf in given p chain? */
  if (pbuf_add_header(p, UDP_HLEN)) {
	  
   printf("? Here pbuf_chain() ...\r\n");
   #if 0
    /* allocate header in a separate new pbuf */
    q = pbuf_alloc(PBUF_IP, UDP_HLEN, PBUF_RAM);
    /* new header pbuf could not be allocated? */
    if (q == NULL) {
      LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("udp_send: could not allocate header\n"));
      return ERR_MEM;
    }
    if (p->tot_len != 0) {
      /* chain header q in front of given pbuf p (only if p contains data) */
      pbuf_chain(q, p);
    }
    /* first pbuf q points to header pbuf */
    LWIP_DEBUGF(UDP_DEBUG,
                ("udp_send: added header pbuf %p before given pbuf %p\n", (void *)q, (void *)p));
   #endif
  } else {
    /* adding space for header within p succeeded */
    /* first pbuf q equals given pbuf */
    q = p;
    LWIP_DEBUGF(UDP_DEBUG, ("udp_send: added header in given pbuf %p\n", (void *)p));
  }
  LWIP_ASSERT("check that first pbuf can hold struct udp_hdr",
              (q->len >= sizeof(struct udp_hdr)));

  /* q now represents the packet to be sent */
  udphdr = (struct udp_hdr *)q->payload;
  udphdr->src = lwip_htons(pcb->local_port);
  udphdr->dest = lwip_htons(dst_port);
  /* in UDP, 0 checksum means 'no checksum' */
  udphdr->chksum = 0x0000;

  /* Multicast Loop? */
#if LWIP_MULTICAST_TX_OPTIONS
  .............jnbkjbkjbk....................
  if (((pcb->flags & UDP_FLAGS_MULTICAST_LOOP) != 0) && ip_addr_ismulticast(dst_ip)) {
    q->flags |= PBUF_FLAG_MCASTLOOP;
  }
#endif

  LWIP_DEBUGF(UDP_DEBUG, ("udp_send: sending datagram of length %"U16_F"\n", q->tot_len));

  if (1)
  {      /* UDP */
    LWIP_DEBUGF(UDP_DEBUG, ("udp_send: UDP packet length %"U16_F"\n", q->tot_len));
    udphdr->len = lwip_htons(q->tot_len);
    /* calculate checksum */
#if CHECKSUM_GEN_UDP
    IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_UDP) {
      /* Checksum is mandatory over IPv6. */
      if (IP_IS_V6(dst_ip) || (pcb->flags & UDP_FLAGS_NOCHKSUM) == 0) {
        u16_t udpchksum;
        {
          udpchksum = ip_chksum_pseudo(q, IP_PROTO_UDP, q->tot_len,
                                       src_ip, dst_ip);
        }

        /* chksum zero must become 0xffff, as zero means 'no checksum' */
        if (udpchksum == 0x0000) {
          udpchksum = 0xffff;
        }
        udphdr->chksum = udpchksum;
      }
    }
#endif /* CHECKSUM_GEN_UDP */
    //.ip_proto = IP_PROTO_UDP;
  }

  /* Determine TTL to use */
  //.ttl = pcb->ttl;
	
  return udp_sendto_if_src_local(pcb, q, dst_ip, dst_port, netif, src_ip);
}

//
// udp.c
//

struct udp_pcb udppcbbuf;

struct udp_pcb *
udp_new(void)
{
  struct udp_pcb *pcb;

  pcb = (struct udp_pcb *) &udppcbbuf; //memp_malloc(MEMP_UDP_PCB);
  /* could allocate UDP PCB? */
  if (pcb != NULL) {
    /* UDP Lite: by initializing to all zeroes, chksum_len is set to 0
     * which means checksum is generated over the whole datagram per default
     * (recommended as default by RFC 3828). */
    /* initialize PCB to all zeroes */
    memset(pcb, 0, sizeof(struct udp_pcb));
    pcb->ttl = UDP_TTL;
  }
  return pcb;
}

//
//
// misc 3
//
//

/* Initialize members of struct pbuf after allocation */
static void
pbuf_init_alloced_pbuf(struct pbuf *p, void *payload, u16_t tot_len, u16_t len, pbuf_type type, u8_t flags)
{
  p->next = NULL;
  p->payload = payload;
  p->tot_len = tot_len;
  p->len = len;
  p->type_internal = (u8_t)type;
  p->flags = flags;
  p->ref = 1;
  p->if_idx = NETIF_NO_INDEX;
  printf("pbuf_alloc1 pbuf_init_alloced_pbuf, p->tot_len = %u, p->len = %u\r\n", tot_len, len);
}

#define EST_PAYLOAD_LEN			  120 //Joseph's estimated reserved
char pbuf_buff[LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF)+EST_PAYLOAD_LEN];
struct dhcp_msg dhcp_msg_buff;

struct pbuf *
pbuf_alloc1(pbuf_layer layer, u16_t length, pbuf_type type)
{
  //struct pbuf *p = &dhcp_msg_buff;
  struct pbuf *p;
  u16_t offset = (u16_t)layer;
	
	//PBUF_TRANSPORT/ PBUF_RAM
  switch (type) {
	  case PBUF_RAM: {
		u16_t payload_len = (u16_t)(LWIP_MEM_ALIGN_SIZE(offset) + LWIP_MEM_ALIGN_SIZE(length));
		mem_size_t alloc_len = (mem_size_t)(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF) + payload_len);

		/* bug #50040: Check for integer overflow when calculating alloc_len */
		if ((payload_len < LWIP_MEM_ALIGN_SIZE(length)) ||
		  (alloc_len < LWIP_MEM_ALIGN_SIZE(length))) {
			return NULL;
		}

		/* If pbuf is to be allocated in RAM, allocate memory for it. */
		p = (struct pbuf *)pbuf_buff; //= mem_malloc(alloc_len);
		if (p == NULL) {
		return NULL;
		}
		printf("pbuf_alloc1 need buffsize %u, prepare-size %d\r\n", length, sizeof(struct dhcp_msg));
		pbuf_init_alloced_pbuf(p, /*LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset)),*/ &dhcp_msg_buff,
							 length, length, type, 0);
        break;
      }
  }
  return p;
}
void
pbuf_realloc(struct pbuf *p, u16_t new_len)
{
  struct pbuf *q;
  u16_t rem_len; /* remaining length */
	
  rem_len = new_len;
	
  q = p;
	
  q->len = rem_len;
  q->tot_len = q->len;
	
  /* q is last packet in chain */
  q->next = NULL;
}

u8_t
pbuf_free(struct pbuf *p)
{
	return 0;
}


#if 0
//
// ip4_addr4.c
//

/* used by IP4_ADDR_ANY and IP_ADDR_BROADCAST in ip_addr.h */
const ip_addr_t ip_addr_any = IPADDR4_INIT(IPADDR_ANY);
const ip_addr_t ip_addr_broadcast = IPADDR4_INIT(IPADDR_BROADCAST);
#endif
