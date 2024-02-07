/**
 * @file
 * Ethernet Interface for standalone applications (without RTOS) - works only for 
 * ethernet polling mode (polling for ethernet frame reception)
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

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "netif/etharp.h"

#include "dm9051_lw.h"
//#include "dm9051_at32_decl.h" //#include "_dm9051_at32_conf.h" //#include "dm9051_env.h" 
#include "dm9051_lw_sync.h"
//#include "dm9051_debug_lw.h"
//#include "..\..\..\lwip_dm9051_driver2303v3\debug.h" //..#include "FreeRTOS.h"
#include "ethernetif.h"
#include "developer_conf.h"
#include <string.h>

/* Network interface name */
#define IFNAME0 'd'
#define IFNAME1 'm'

#define OVERSIZE_LEN	PBUF_POOL_BUFSIZE //#define _PBUF_POOL_BUFSIZE 1514 //defined in "lwipopts.h" (JJ20201006)
#define RXBUFF_OVERSIZE_LEN		(OVERSIZE_LEN+2)
union {
	uint8_t rx;
	uint8_t tx;
} EthBuff[RXBUFF_OVERSIZE_LEN]; //[Single Task project.] not occupied by concurrently used.
uint8_t MACaddr[6];

//u8 *get_eth_buff(void)
//{
//	return &EthBuff[0].tx;
//}
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
}

void lwip_get_mac(char *adr)
{
  memcpy(adr, MACaddr, 6);
}

void dm9051_init_dual(uint8_t *adr)
{
	uint16_t id;
#if (APP_ETH_COUNT == 1)
	dm9051_poweron_rst();
	delay_ms(1);
	id = dm9051_init(adr);
	chipid_on_pin_code("dm9051_init", id);
#elif (APP_ETH_COUNT >= 2)
	int i;
	printf(".dm9051_init_dual().s\r\n");
	for (i = 0; i < APP_ETH_COUNT; i++) { //get_eth_interfaces()
		mstep_set_net_index(i); //set_pin_code(i);
		dm9051_poweron_rst();
		delay_ms(1);
		id = dm9051_init(adr);
		chipid_on_pin_code("dm9051_init", id);
	}
	mstep_set_net_index(0);
	printf(".dm9051_init_dual().e\r\n");
#endif

#if 0
	do {
	int pnc, j, k;
	for (pnc = 0; pnc < get_eth_interfaces(); pnc++) {
		uint16_t id;
		set_pin_code(pnc);
		chipid_on_pin_code("mux-testing...", 0);	
		for (i = 0; i < 16; i++) {
			gpio_mux_sel_type muxtbl[16] = {
				GPIO_MUX_0,
				GPIO_MUX_1,
				GPIO_MUX_2,
				GPIO_MUX_3,
				GPIO_MUX_4,
				GPIO_MUX_5,
				GPIO_MUX_6,
				GPIO_MUX_7,
				GPIO_MUX_8,
				GPIO_MUX_9,
				GPIO_MUX_10,
				GPIO_MUX_11,
				GPIO_MUX_12,
				GPIO_MUX_13,
				GPIO_MUX_14,
				GPIO_MUX_15,
			};
			gpio_spi_clk_mux(muxtbl[i]);
			
			printf("  sck Mux %x, %s\r\n", i, get_spi_name(pnc));
			
			//([.])
			for (j = 0; j < 16; j++) {
				printf("  Mux %x miso, Chip ID mosi:", j);
				gpio_spi_mi_mux(muxtbl[j]);
				for (k = 0; k < 16; k++) {
					gpio_spi_mo_mux(muxtbl[k]);
					id = read_chip_id();
					printf("  %4x", id);
					if (id == 0x9051) {
						printf("\r\n");
						printf("\r\n");
						goto cont;
					}
				}
				printf("\r\n");
			}
			  
			if (!((i + 1)%4)) {
			  printf("\r\n");
			}
		}
	cont:
		id = 0; //dummy-line for compiler.
	} //for
	} while(0); //do
#endif
}

uint16_t dm9051_link_update_dual(void)
{
	uint16_t updwn;
#if (APP_ETH_COUNT > 1) 
	int i;
	for (i = 0; i < APP_ETH_COUNT; i++) { //get_eth_interfaces()
		mstep_set_net_index(i); //set_pin_code(i);
		updwn = dm9051_link_update();
		if (updwn)
			break;
	}
#else
	updwn = dm9051_link_update();
#endif
	return updwn;
}
void dm9051_tx_dual(uint8_t *buf, uint16_t len)
{
#if (APP_ETH_COUNT > 1) 
	int i;
	for (i = 0; i < APP_ETH_COUNT; i++) { //get_eth_interfaces()
		mstep_set_net_index(i); //set_pin_code(i);
		dm9051_tx(buf, len); //on_pin_code("dm9051_tx", 0);
	}
#else
	dm9051_tx(buf, len);
#endif
}
uint16_t dm9051_rx_dual(uint8_t *buff)
{
	u16 len;
#if (APP_ETH_COUNT > 1) 
	int i;
	for (i = 0; i < APP_ETH_COUNT; i++) { //get_eth_interfaces()
		mstep_set_net_index(i); //set_pin_code(i);
		len = dm9051_rx(buff);
		if (len)
			break;
	}
#else
	len = dm9051_rx(buff);
#endif
	return len;
}

#if (APP_ETH_COUNT > 1) 
void bench_test_tx_and_rx(void)
{
	int i;
	uint16_t len;
	uint8_t rcr[APP_ETH_COUNT], rcrn[APP_ETH_COUNT];
	uint8_t zbuff[72];
	char disp[72];
	
	for (i = 0; i < APP_ETH_COUNT; i++) {
		mstep_set_net_index(i);
		rcr[i] = spi_read_reg(DM9051_RCR);
		spi_write_reg(DM9051_RCR, 0x33);
		rcrn[i] = spi_read_reg(DM9051_RCR);
		printf("%d. bench_test.s rcr %02x to %02x\r\n", i, rcr[i], rcrn[i]);
	}
	
	mstep_set_net_index(0);
	dm9051_tx("abcdefghijkl........abcdefghijkl........abcdefghijkl.....xxx", 60);
	printf("%s, send %s len %d\r\n", mstep_spi_conf_name(), 
		"abcdefghijkl........abcdefghijkl........abcdefghijkl.....xxx",
		strlen("abcdefghijkl........abcdefghijkl........abcdefghijkl.....xxx"));
	function_monitor_tx("abcdefghijkl........abcdefghijkl........abcdefghijkl.....xxx",
		strlen("abcdefghijkl........abcdefghijkl........abcdefghijkl.....xxx"));
	
#if 0
	//for (i = 0; i < APP_ETH_COUNT; i++) { //get_eth_interfaces()
		//mstep_set_net_index(i); //set_pin_code(i);
		//len = dm9051_rx(zbuff);
		//if (len) {
		//	printf("%s, recv %s\r\n", mstep_spi_conf_name(), zbuff);
		//	break;
		//}
	//}
#endif
	
	for (i = 0; i < APP_ETH_COUNT; i++) { //get_eth_interfaces()
		mstep_set_net_index(i); //set_pin_code(i);
		len = dm9051_rx(zbuff);
		if (len) {
			memset(disp, 0, sizeof(disp));
			memcpy(disp, zbuff, len - 4); //sizeof(zbuff)
			printf("%s, recv %s len %u\r\n", mstep_spi_conf_name(), disp, len - 4); //sizeof(zbuff)
			function_monitor_rx(zbuff, len - 4); //function_monitor_rx_all(zbuff, len - 4);
			//break;
		}
	}
	
#if 1
	mstep_set_net_index(1);
	dm9051_tx("play-game", 9);
	printf("%s, send %s len %d\r\n", mstep_spi_conf_name(), 
		"play-game", 
		strlen("play-game"));
	function_monitor_tx("play-game",
		strlen("play-game"));
#endif
	
	for (i = 0; i < APP_ETH_COUNT; i++) { //get_eth_interfaces()
		mstep_set_net_index(i); //set_pin_code(i);
		len = dm9051_rx(zbuff);
		if (len) {
			memset(disp, 0, sizeof(disp));
			memcpy(disp, zbuff, len - 4); //sizeof(zbuff)
			printf("%s, recv %s len %u\r\n", mstep_spi_conf_name(), disp, len - 4); //sizeof(zbuff)
			function_monitor_rx(zbuff, len - 4); //function_monitor_rx_all(zbuff, len - 4);
			//break;
		}
	}
	
	for (i = 0; i < APP_ETH_COUNT; i++) {
		mstep_set_net_index(i);
		spi_write_reg(DM9051_RCR, rcr[i]);
		rcr[i] = spi_read_reg(DM9051_RCR);
		printf("%d. bench_test.e rcr %02x to %02x\r\n", i, rcrn[i], rcr[i]);
	}
}
#endif

static void
low_level_init(struct netif *netif)
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
  dm9051_init_dual(MACaddr); //dm9051_init(MACaddr);
}

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int l = 0;
  uint8_t *buffer = &EthBuff[0].tx; //Tx_Buff;
	
  for(q = p; q != NULL; q = q->next)
  {
    memcpy((u8_t*)&buffer[l], q->payload, q->len);
    l = l + q->len;
  }
  dm9051_tx_dual(buffer, (uint16_t) l); //dm9051_tx(.);
  debug_tx(buffer, l);
  return ERR_OK;
}

	static struct pbuf *
	low_level_inp(struct netif *netif)
	{
	  struct pbuf *p, *q;
	  uint16_t len;
	  int l = 0;
	  uint8_t *buffer = &EthBuff[0].rx;
		
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
		}
	  }
	  return p;
	}

	static struct pbuf *
	low_level_input(struct netif *netif)
	{
	  struct pbuf *p, *q;
	  uint16_t len;
	  int l = 0;
	  uint8_t *buffer = &EthBuff[0].rx; //Rx_Buff;

	  len = dm9051_rx_dual(buffer); //dm9051_rx(.);
		
	  if (!len)
		  return NULL;
	  
		//.printf("net,rx (len %u)\r\n", len);
	  
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
		if (p)
		{
			for (q = p; q != NULL; q = q->next)
			{
			  memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
			  l = l + q->len;
			}
			#if 1
			//printf("ethernetif.c(pbuf:%s Eth Buffer total len %d)\r\n", "rx", len);
			#else
			showpbuf("rx", netif, p);
			#endif
		}
	  return p;
	}

	
err_t ethernetif_inp(struct netif *netif)
{
	  err_t err;
	  struct pbuf *p;

	  p = low_level_inp(netif);
	  if (p == NULL) 
		return ERR_INPROGRESS; //JJ.

	  err = netif->input(p, netif);
	  if (err != ERR_OK)
	  {
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
		pbuf_free(p);
	  }
	  
	  return err;
}

err_t ethernetif_input(struct netif *netif)
{
	  err_t err;
	  struct pbuf *p;
		
#if 1
  onchange_timeouts_log("ethernetif_input() call-to low_lvl_input && netif_input,rx"); //to-has this
#endif

	  /* move received packet into a new pbuf */
	  p = low_level_input(netif);

	  /* no packet could be read, silently ignore this */
	  if (p == NULL) 
		return ERR_INPROGRESS; //JJ.

#if 1
  if (p->tot_len == LEN64) {
	if (DBG_IS_ARP(p->payload))
		printf(" rx,(such as 'ARP')\r\n"); //no-need show. such as 'ARP'
	else if (DBG_IS_TCP(p->payload))
		//printf(" rx,(such as 'ACK')\r\n"); //no-need show. such as 'ACK'
		printf(" rx,(debug do process such as 'ACK')\r\n");
	else
		printf(" rx,(tiny LEN64)\r\n");
  } else
	printf(" netif_input,rx (p->tot_len %u)\r\n", p->tot_len);
	
  /* To trace */
  debug_rx(p->payload, p->len); //ok. only 1st-pbuf
#endif

	  /* entry point to the LwIP stack */
	  err = netif->input(p, netif);
  
  if (p->tot_len == 64) {
	  if (DBG_IS_TCP(p->payload)) {
		  if (err == ERR_OK)
			printf(" .[debug process this 'ACK' is ERR_OK] = %s\r\n", lwip_strerr(err));
		  else
			printf(" .[debug process this 'ACK' is err %d] = %s\r\n", err, lwip_strerr(err));
	  }
  }
  
	  if (err != ERR_OK)
	  {
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
		pbuf_free(p);
	  }
	  return err;
}

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

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
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

  #if 0
  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);
  #endif

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

  return ERR_OK;
}
