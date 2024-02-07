/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */
#include "at32f415_board.h" //"at32f403a_407_board.h"
#include "gadge\dm9051_debug_lw.h" //..#include "FreeRTOS.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "err.h"
#include "ethernetif.h"

#include "dm9051_at32_decl.h"

#include "dm9051_lw.h"
#include "developer_conf.h"

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#include <string.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'd'
#define IFNAME1 'm'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int unused;
};

#define OVERSIZE_LEN	PBUF_POOL_BUFSIZE //#define _PBUF_POOL_BUFSIZE 1514 //defined in "lwipopts.h" (JJ20201006)
#define RXBUFF_OVERSIZE_LEN		(OVERSIZE_LEN+2)
union {
	u8 rx;
	u8 tx;
} EthBuff[RXBUFF_OVERSIZE_LEN]; //[Single Task project.] not occupied by concurrently used.
u8 MACaddr[6];

u8 *get_eth_buff(void)
{
	return &EthBuff[0].tx;
}

void lwip_set_mac(uint8_t* macadd)
{
  MACaddr[0] = macadd[0];
  MACaddr[1] = macadd[1];
  MACaddr[2] = macadd[2];
  MACaddr[3] = macadd[3];
  MACaddr[4] = macadd[4];
  MACaddr[5] = macadd[5];
  //.dm9051_mac_adr(macadd);
}

void lwip_get_mac(char *adr)
{
  memcpy(adr, MACaddr, 6);
}

static void
low_level_init(struct netif *netif)
{
  uint16_t id;
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
  id = dm9051_init(MACaddr);
  chipid_on_pin_code("dm9051_init", id);
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int l = 0;
  u8 *buffer = &EthBuff[0].tx; //Tx_Buff;
	
  for(q = p; q != NULL; q = q->next)
  {
    memcpy((u8_t*)&buffer[l], q->payload, q->len);
    l = l + q->len;
  }
  dm9051_tx(buffer, (uint16_t) l);
  debug_tx(buffer, l);
  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  int l = 0;
  u8 *buffer = &EthBuff[0].rx; //Rx_Buff;

  len = dm9051_rx(buffer);
  if (!len)
	  return NULL;
  
	p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	if (p)
	{
		for (q = p; q != NULL; q = q->next)
		{
		  memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
		  l = l + q->len;
		  //.printf("ethernetif.c(pbuf:q->len %d)\r\n", q->len);
		}
 #if 1
		//printf("ethernetif.c(pbuf:%s Eth Buffer total len %d)\r\n", "rx", len);
 #else
		showpbuf("rx", netif, p);
 #endif
	}
  return p;
}

err_t
ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);

  /* no packet could be read, silently ignore this */
  if (p == NULL) 
	return ERR_INPROGRESS; //JJ.

#if 1
  onchange_timeouts_log("ethernetif_input() call-to low_lvl_input && netif_input,rx"); //to-has this
	
  if (p->tot_len == 64)
	; //no-need show. sucj as 'ACK'
  else {
	printf("netif_input,rx (p->tot_len %u)\r\n", p->tot_len);
	//.printf("\r\n");
  }
	
  /* To trace */
  debug_rx(p->payload, p->len); //ok. only 1st-pbuf
#endif

  err = netif->input(p, netif);
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
	return err;
  }
  return ERR_OK;
}

err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("_ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_init(netif);
  
  //.printf("init ethernetif: name = %c%c\r\n", netif->name[0], netif->name[1]);
  return ERR_OK;
}
