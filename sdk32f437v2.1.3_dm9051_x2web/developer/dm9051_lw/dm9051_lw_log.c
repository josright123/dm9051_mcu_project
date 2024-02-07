//#include "stdint.h"
//#include "dm9051_at32_decl.h"
//#include "dm9051_debug_lw.h"
#include <stdlib.h>
#include "dm9051_lw_conf.h"
#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_log_types.h"
#include "dm9051_lw_log_inc.h"
#include "main_malloc.h"

#if !DM9051_DEBUG_ENABLE
#error "Davicom sanity, Please don't add this implement C file, because you did NOT enable debug packets dump in the 'dm9051opts.h',\
 Or to HAVE packets dump functions you can define DM9051_DEBUG_ENABLE to 1 in the 'dm9051opts.h'"
#endif

TX_MODLE_DECLARATION;
tx_monitor_t tx_modle_keep = {
	0,
};
tx_monitor_t tx_all_modle_keep = {
	0,
};

static const struct uip_eth_addr log_ethbroadcast = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
#define	IsBroadcast(add)	(memcmp(add, log_ethbroadcast.addr, sizeof(struct uip_eth_addr))==0)
	
#define	IsMulticast(buf)	(((uint8_t *)(buf))[0] & 0x01)

static void sprint_hex_dump(const char *prefix_str, int rowsize, const void *buf, int seg_start, size_t len) { } //large program code.
static void sprint_arp_field_parse(const void *buf, int segpos, int segsize) { } // complexity program code
static void arp_counting_to_safty_tx(int to_must_safty) { }
	//=if (cb.tmp.arp_we_trans_count > 2 || to_must_safty) {
	//	cb.tmp.arp_we_trans_count = 0;
	//	cb.tmp.flg_wait_uni_arp_finish = 1; //=(cb.tmp.flg_has_arp_unicast_enter = 0;) //back-to-search-a-one.
	//}

void arp_unicast_safty_tx(const uint8_t *buf, size_t len) //(struct net_device *ndev, struct sk_buff *skb)
{	//const u8 *buf = skb->data;
	/*static uint16_t unila = 0xffff;
	uint16_t la = BUF->dipaddr[1] >> 8;*/
	//int local_ttx = 0;
	//int local_ttx_reach = 1; 

	//cb.ram.cas3++;
	//cb.tmp.arp_safty_cas3_tx_uni++;

	//if (cdef.cons_t.local_dip_ttx) {
		/*if (la != unila)
		{
			DM_save_first(unila, unilac, la);
			if (cb.tx_total.val > cb.ram.tx_total_next)
			  printk("TTX %2d TX .%d Brd %d/%d --- dm9 %d %d ARP.%d, no-reach %d (safty Tx uni to %d.%d.%d.%d) sklen %d\n",
				cb.tx_total.val,
				cb.tmp.arp_safty_cas3_tx_uni, cb.tmp.arp_wait_cas1_tx_brdcst + cb.tmp.arp_safty_cas1_tx_brdcst,
				cb.arp_total_tx.val, cb.ram.cas1, cb.ram.cas2, cb.ram.cas3, cb.tmp.arp_we_trans_count,
				BUF->dipaddr[0] & 0xff, BUF->dipaddr[0] >> 8, BUF->dipaddr[1] & 0xff, BUF->dipaddr[1] >> 8,
				skb->len);
		}
		else
		{
			if (cb.tx_total.val > cb.ram.tx_total_next)
			  printk("TTX %2d TX .%d\n", cb.tx_total.val, cb.tmp.arp_safty_cas3_tx_uni);
		}*/
		sprint_hex_dump("CAS3 ", 32, buf, 0, len - 0); // print HEX
		sprint_arp_field_parse(buf, SEG_QUAN, len - SEG_QUAN);
	//}
}

/*static*/ void roll_next_tx(int total_now)
{
	/*if (cdef.cons_t.local_dip_ttx_reach) {
	if (cdef.cons_t.local_gap_reach_ttx) {
		if (cb.tx_total.val > cb.ram.tx_total_next) {
		  cb.ram.tx_reachfor_disp ++;
		  if (cb.ram.tx_reachfor_disp < GAP_START_FOR) //=5
		    cb.ram.tx_total_next = cb.tx_total.val + GAP_START_FOR; //=5
		  else if (cb.ram.tx_reachfor_disp < (GAP_START_FOR+3)) { //=5+3 (8)
		    cb.ram.tx_total_next /= GAP_REACH_FOR_TX; //100000;
		    cb.ram.tx_total_next *= GAP_REACH_FOR_TX; //100000;
		    cb.ram.tx_total_next += GAP_REACH_FOR_TX; //100000;
		  } else {
		    cb.ram.tx_total_next /= GAP_REACH_FOR_TX_II;
		    cb.ram.tx_total_next *= GAP_REACH_FOR_TX_II;
		    cb.ram.tx_total_next += GAP_REACH_FOR_TX_II;
		  }

		  printk(" (tx reach %2d).s (%7d next >  ttx %7d)(tx next reach %2d).e\n", reachfor_now, total_now, cb.ram.tx_total_next, cb.ram.tx_reachfor_disp);
		}
	}
	}*/
}

//static 
void arp_counting_reach_disp_tx(const void *buf, size_t len)
{
	//if (cb.tmp.arp_we_trans_count > 2) {
		//printk("TX %d Brd %d/%d (more safty uni-ARP)---> dm9 (This reach 'flg_wait_uni_arp_finish' ) next could-touch, sklen %d\n",
		//	cb.tmp.arp_safty_cas3_tx_uni, cb.tmp.arp_wait_cas1_tx_brdcst + cb.tmp.arp_safty_cas1_tx_brdcst, cb.arp_total_tx.val,
		//	skb->len);
		size_t x = (size_t) kkmin(32, (int) len);
		sprint_hex_dump("CAS0", 32, buf, 0, x); // a "CAS-no_unicast_arp_enter"
		sprint_arp_field_parse(buf, SEG_QUAN, len - SEG_QUAN);
	//}
}

static void arp_tx(const uint8_t *buf, size_t len, int fullcheck)
{
	//if (cb.tmp.flg_wait_uni_arp_finish == 0) {
		if (!IsBroadcast(buf)) {
			/* [unicast-] */
			printf("  Below is arp_unicast_safty_tx [CASE3 ]\r\n");
			//cb.arp_uni_out_tx.val++;
			//if (cb.arp_uni_out_tx.enable) {
				//cb.tmp.arp_we_trans_count++; //inc 1
				arp_unicast_safty_tx(buf, len); //(ndev, skb);
				if (fullcheck) 
					roll_next_tx(0); //cb.tx_total.val
			//}
		}
		
		if (fullcheck) {
			/* [full-display-] */
			//.printf("arp_counting_reach_disp_tx [CAS2 ]\r\n");
			//[Transmiting...2/2]
			//if (cb.tmp.arp_we_trans_count > 2) { //counting reach expire
				arp_counting_reach_disp_tx(buf, len); //counting reach expire
				arp_counting_to_safty_tx(0); //counting reach expire, and rst 0
			//}
		} else {
			/* [partial-display-] */
			printf("  arp_counting_to_safty_tx [CAS1 ]\r\n");
			//part of= [Transmiting...2/2]
			//if (cb.tmp.arp_we_trans_count > 2) //counting reach expire
				arp_counting_to_safty_tx(0); //counting reach expire, and rst 0
		}
	//}
}

static void dhcp_tx(const uint8_t *buf)
{	//const void *buf = skb->data;
	// temp empty
}

static void protocol_tx(const uint8_t *buf, size_t len, int fullcheck)
{	//const u8 *buf = skb->data; 
	if (IS_ICMP) {
		/*if (cb.tmp.msg_print_icmp) {
			cb.tmp.last_print_icmp = cb.tmp.msg_print_icmp;
			cb.tmp.msg_print_icmp = pprint_icmp(skb, ndev, DGROUP_TX,
							    cb.tmp.last_print_icmp, false);
			if (cb.tmp.last_print_icmp)
				printk("[ttx-icmp seqNO-%3d, id %d (%d %d) msg check now %3d from last %3d]\n",
					HTONS(ICMPBUF->seqno), HTONS(ICMPBUF->id), ICMPBUF->type, ICMPBUF->icode,
					cb.tmp.msg_print_icmp, cb.tmp.last_print_icmp);
		}*/
		return;
	}

	if (fullcheck) {
		if (IsMulticast(buf)) {
			if (IS_UDP) {
				//if (cdef.cons_t.msg_print_tx_multi_udp)
				//	printk("TTX %2d dm9 tx misc %3d multicast udp\n", cb.tx_total.val, cb.misc_total_tx.val);
			} else if (IPBUF->proto == UIP_PROTO_IGMP_JJ)
				; //printk("TTX %2d dm9 tx misc %3d multicast igmp\n", cb.tx_total.val, cb.misc_total_tx.val);
			else {
				//printk("TTX %2d dm9 tx misc %3d multicast-xx-else\n", cb.tx_total.val, cb.misc_total_tx.val);
				sprint_hex_dump("MISC", 32, buf, 0, 14+20+20);
			}
		} else {
			if (IPBUF->proto == UIP_PROTO_TCP) {
				//ec = &eclass[16];
				//printk("TTX %2d dm9 tx misc %3d uni-tcp\n", cb.tx_total.val, cb.misc_total_tx.val);
				sprint_hex_dump("MISC", 32, buf, 0, 14+20+20);
			} else if (IS_UDP) {
				//if (cdef.cons_t.msg_print_tx_uni_udp) {
					//ec = &eclass[17];
					//printk("TTX %2d dm9 tx misc %3d uni-udp\n", cb.tx_total.val, cb.misc_total_tx.val);
					sprint_hex_dump("MISC", 32, buf, 0, 14+20+20);
				//}
			} else {
				//ec = &eclass[18];
				//printk("TTX %2d dm9 tx misc %3d uni-xx-else\n", cb.tx_total.val, cb.misc_total_tx.val);
				sprint_hex_dump("MISC", 32, buf, 0, 14+20+20);
			}
		}
	}
}

void sprint_hex_dump0(int head_space, int titledn, char *prefix_str,
		    size_t tlen, int rowsize, const void *buf, int seg_start, size_t len, /*int useflg*/ int cast_lf) //, int must, int dgroup
{
#if 1
	//if (useflg) {
		int si, se, titlec = 0;
		int hs, i, linelen, remaining = len; //const eth_class_t *ec = &eclass[10];

		si = seg_start;
		se = seg_start + len;
		for (i = si; i < se; i += rowsize) {
			//unsigned 
			char linebuf[(12 * 3) + (3 * 16) + 1 + 32]; //here!

			linelen = kkmin(remaining, rowsize);
			remaining -= rowsize;
			do {
				const uint8_t *ptr = buf;
				int j;
				//unsigned char *
				int nb = 0;
				for (j=0; j< linelen && (size_t) nb < sizeof(linebuf); j++) {
					if (j && !(j%8))
						nb += snprintf(linebuf+nb, sizeof(linebuf)-nb, " ");
					if (((rowsize>>1)!=8) && !(j%(rowsize>>1)))
						nb += snprintf(linebuf+nb, sizeof(linebuf)-nb, " ");

					nb += snprintf(linebuf+nb, sizeof(linebuf)-nb, "%02x ", *(ptr + i + j));
				}
			} while(0);
			
			hs = head_space;
			while(hs--)
				printf(" ");;
			
			if (prefix_str) {
				printf("(%s) %.8x: %s", prefix_str, i, linebuf); //"%s", ec->str //CHECK (XXX >> )
				while (titledn) {
					titledn--;
					prefix_str[titlec++] = ' ';
				}
			} else {
				printf("(dm9 xfer) %.8x: %s", i, linebuf); //"%s", ec->str
			}
			
			if ((i+rowsize) < se)
				printf("\r\n");
			else {
				if (cast_lf)
					printf("\r\n");
				
				if (IS_UDP) {
					//ptr
					#if 0
					size_t ulen = tlen; // larger for with 4-bytes CRC
					ulen = UIP_LLH_LEN;
					ulen += HTONS(UDPBUF->udplen) - 8;
					ulen += sizeof(struct uip_udpip_hdr); // correct for without 4-bytes CRC (htons)
					
					if (cast_lf)
						printf("\r\n");
				
					printf(" ..SrcIP %d.%d.%d.%d", (IPBUF->srcipaddr[0] >> 0) & 0xff, (IPBUF->srcipaddr[0] >> 8),
						(IPBUF->srcipaddr[1] >> 0) & 0xff, (IPBUF->srcipaddr[1] >> 8));
					printf("  DestIP %d.%d.%d.%d", (IPBUF->destipaddr[0] >> 0) & 0xff, (IPBUF->destipaddr[0] >> 8),
						(IPBUF->destipaddr[1] >> 0) & 0xff, (IPBUF->destipaddr[1] >> 8));
					printf("  Len %d", ulen);
					printf("  (%5d -> %d Len %d)", UDPBUF->srcport, UDPBUF->destport, HTONS(UDPBUF->udplen) - 8);
					printf("\r\n");
					#endif
				} 
				//else
				//	printf("\r\n");
			}
		}
	//}
#endif
}

//static
void dm_check_tx(const uint8_t *buf, size_t len)
{
#if 1
	if (IS_ARP) { //cb.arp_total_tx.val++;
		arp_tx(buf, len, DEF_MONITOR_NTX); //1 or 0
	}
	else if (IS_DHCPTX) { //cb.dhcp_total_tx.val++; //'~tx_total'
		dhcp_tx(buf);
	} else { //cb.misc_total_tx.val++;
		protocol_tx(buf, len, DEF_MONITOR_NTX);
	}
#endif
}

//static void dm_check_tx(const uint8_t *buf, size_t len);
//static void _arp_tx(const uint8_t *buf, size_t len, int fullcheck);
//static void _dhcp_tx(const uint8_t *buf);
//static void _protocol_tx(const uint8_t *buf, size_t len, int fullcheck);
//static void _arp_unicast_safty_tx(const uint8_t *buf, size_t len);
//static void _arp_counting_reach_disp_tx(const void *buf, size_t len);
//static void _arp_counting_to_safty_tx(int to_must_safty);
//static void _roll_next_tx(int total_now);

int DBG_IS_ARP(void *dataload) {
	const uint8_t *buf = dataload;
	return IS_ARP;
}
int DBG_IS_TCP(void *dataload) {
	const uint8_t *buf = dataload;
	return IS_TCP;
}

int master_UDP_unknow = 0;
void debug_tx(const uint8_t *buf, uint16_t len)
{
  static int count = 0;
#if 0
  static int icmp_count = 0;
  static int arp_count = 0;
  static int udp_count = 0;
#endif
  count++;
	
  if (IS_ICMP) {
#if 0
	if (icmp_count < 2) {
		icmp_count++;
		//.printf("---------------Sending ICMP(%d/2) total_tx(%d)\r\n", icmp_count, count);
		//.function_monitor_tx(buf, len);
	}
#endif
	return;
  } 

  if (IS_ARP) {
#if 0
	//if (arp_count < 2) {
	//	arp_count++;
	//	printf("---------------Sending ARP(%d/2) total_tx(%d)\r\n", arp_count, count);
	//	function_monitor_tx(buf, len);
	//	printf("\r\n");
	//}
#endif
	return;
  } 
  
  if (IS_UDP) {
	  if (IS_DHCPTX) {
		printf("---------------Sending DHCP total_tx(%d) Len %u\r\n", count, len);
		function_monitor_tx_all(HEAD_SPC, 0, NULL, buf, len);
	  }
#if 0
	  //else {
	//	  if (udp_count < 3) {
	//		udp_count++;
	//		printf("---------------Sending packet(%d/3) total_tx(%d)\r\n", udp_count, count);
	//		function_monitor_tx(buf, len);
	//	  }
	  //}
#endif
	  return;
  }
  
#if 0
  if (len > 70) { //add.
    printf("---------------Sending total_tx(%d)\r\n", count);
    function_monitor_tx_all(HEAD_SPC, 0, NULL, buf, len);
  }
#endif
}

int debug_rx(const uint8_t *buf, uint16_t len)
{
#if 0
  static int icmp_count_rx = 0;
  static int rx_mult_tcp_count = 0;
  static int rx_mult_udp_count = 0;
#endif
  static int master_TCP_count = 0;
	
  if (IsMulticast(buf)) { 
    if (IS_ARP)
		return 0;
	
	if (IS_DHCPRX) {
		printf("Receive DHCP pkt (%02x:%02x:%02x:%02x:%02x:%02x) len %d ---------------\r\n", 
			buf[0],buf[1],buf[2],buf[3],buf[4],buf[5], len);
#if 0
		//function_monitor_rx_all(buf, len);
#endif
	#if 1
		printf("Your client ID: %d.%d.%d.%d\r\n", buf[0x3a], buf[0x3b], buf[0x3c], buf[0x3d]); //0x3a = 58
		printf("\r\n");
	#endif
		return 0;
	}

#if 0
	if (IS_IP) {
		if (IS_TCP) {
			if (rx_mult_tcp_count < 2) {
				rx_mult_tcp_count++;
				printf("Receive Multicast and TCP(%d/2): (%02x:%02x:%02x:%02x:%02x:%02x) ? ---------------\r\n", 
					rx_mult_tcp_count, buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
				function_monitor_rx(buf, len);
			}
			return 0;
		}
		if (IS_UDP) {
			if (rx_mult_udp_count < 3) {
				rx_mult_udp_count++;
				printf("Receive Multicast(%d/3) and UDP: (%02x:%02x:%02x:%02x:%02x:%02x) Protocol: %s (%d) ---------------\r\n", 
					rx_mult_udp_count, buf[0],buf[1],buf[2],buf[3],buf[4],buf[5], "TBD", IPBUF->proto);
				function_monitor_rx(buf, len);
			}
			return 0;
		}
			
		printf("Receive Multicast and OP: (%02x:%02x:%02x:%02x:%02x:%02x) ? ---------------\r\n", 
			buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
		function_monitor_rx(buf, len);
		return 0;
	}
	
	printf("Receive Multicast and UNKNOW Multicast packet: (%02x:%02x:%02x:%02x:%02x:%02x) ---------------\r\n", 
				buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	//function_monitor_rx_all(buf, len);
#endif
	return 0;
  }
  
  if (IS_ARP)
	return 0; //uni-cast arp
 
  if (IS_ICMP) {
#if 0
		if (icmp_count_rx < 2) {
			icmp_count_rx++;
			printf("Receive icmp(%d/2) Protocol: %s (%d) ---------------\r\n", icmp_count_rx, "ICMP", IPBUF->proto);
			function_monitor_rx(buf, len);
		}
#endif
		return 0;
  }
  
  if (IS_IP) {
	if (IS_TCP) {
		if (IS_TCP) {
			master_TCP_count++;
			if (len > 70) { //add.
				printf("\r\n");
				printf("Receive unit-cast pkt. Protocol %s (%d), Listen port %d len %d (%d) ---------------\r\n", "TCP",
						IPBUF->proto, HTONS(TCPBUF->destport), len, master_TCP_count);
				function_monitor_rx(HEAD_SPC, buf, len);
			}
			return 0;
		}
	}
#if 0
	else if (IS_UDP) {
		master_UDP_unknow++;
		printf("\r\n");
		printf("Receive unit-cast pkt. Protocol: %s (%d) .%d ---------------\r\n", "UDP", IPBUF->proto, master_UDP_unknow);
	} else
		printf("Receive unit-cast pkt. Protocol: %s (%d) ---------------\r\n", "TBD", IPBUF->proto);
	function_monitor_rx(buf, len);
#endif
	return 0;
  }
  
  printf("Receive unit-cast UNKNOW pkt ---------------\r\n");
  function_monitor_rx(HEAD_SPC, buf, len); //(buffer, l);
  return 1; //err
}

//void dm9051_log_dump0(char *prefix_str, size_t tlen, const void *buf, size_t len)
//{
//	int rowsize = 32;
//	int seg_start = 0;
//	sprint_hex_dump0(HEAD_SPC, 0, prefix_str, tlen, rowsize, buf, seg_start, len, DM_FALSE);
//}

void dm9051_txlog_monitor_tx(int hdspc, const uint8_t *buffer, size_t len)
{
	//#define HEAD_LEN	125
	//char buff[36]; // note: should be longer enough!
	//char *heads = buff;
	if (tx_modle_keep.allow_num < tx_modle.allow_num) {
		tx_modle_keep.allow_num++;
#if LWIP_TESTMODE
		//......fgn l nre..........
		if (tx_modle_keep.allow_num == 1 || tx_modle_keep.allow_num == 5 || // because we do checksum offload teat, only meaning with packet1/packet9
			tx_modle_keep.allow_num == 9 || tx_modle_keep.allow_num == 13)
#endif
		do {
			#define HEAD_LEN	MMALLOC_MAX_LEN2 //3KB --> 1kb
			char *heads;
			int n;
			heads = (char *) malloc(HEAD_LEN); // note: memory allocation WITH <stdlib.h>!
			#if 1
			printf("  ### tx [ %lx = malloc( %d )  as a reference for headstr of _txlog_monitor_tx]\r\n",
				heads, HEAD_LEN);
			#endif
			  //=sprintf(heads, "%d/%d tx[%d]>>", tx_modle_keep.allow_num, tx_modle.allow_num, mstep_get_net_index());
			  n = sprintf(heads, "%d/%d", tx_modle_keep.allow_num, tx_modle.allow_num);
			  sprintf(heads, "%d/%d tx[%d]>>", tx_modle_keep.allow_num, tx_modle.allow_num, mstep_get_net_index());
			
			  function_monitor_tx(hdspc, n, /*NULL*/ heads, buffer, len);
			free(heads);
		} while(0);
	}
}

void dm9051_rxlog_monitor_rx(int hdspc, const uint8_t *buffer, size_t len)
{
	function_monitor_rx(hdspc, buffer, len);
}

void dm9051_txlog_monitor_tx_all(int hdspc, const uint8_t *buffer, size_t len)
{
	if (tx_all_modle_keep.allow_num < tx_all_modle.allow_num) {
		#define HEAD_LEN	MMALLOC_MAX_LEN2 //3KB --> 1kb
		char *heads;
		int n;
		tx_all_modle_keep.allow_num++;

		heads = (char *) malloc(HEAD_LEN); // note: memory allocation WITH <stdlib.h>!
		  n = sprintf(heads, "%d/%d", tx_all_modle_keep.allow_num, tx_all_modle.allow_num);
		  sprintf(heads, "%d/%d tx[%d]>>", tx_all_modle_keep.allow_num, tx_all_modle.allow_num, mstep_get_net_index());
		
		  bannerline_log();
		  function_monitor_tx_all(hdspc, n, heads, buffer, len);
		
		free(heads);
	}
}

uint16_t unitcast_UNKNOW_pkt = 0;

void debug_rx_inc_count(void) {
	unitcast_UNKNOW_pkt++;
}
void debug_rx_display_count(void) {
	#if 1
	printf("d (... unitcast_UNKNOW_pkt: %d / function_monitor_rx_all ...)\r\n", unitcast_UNKNOW_pkt);
	#endif
}

void dm9051_rxlog_monitor_rx_all(int hdspc, const uint8_t *buffer, size_t len)
{
	function_monitor_rx_all(hdspc, NULL, buffer, len);
	//.debug_rx_display_count(); NOT here~
}

void dm9051_txlog_disp(uint8_t *buffer, int len)
{
#if DM9051_DEBUG_ENABLE
  debug_tx(buffer, len);
#endif
}

void dm9051_rxlog_arp(void *payload, uint16_t tot_len, uint16_t len)
{
#if DM9051_DEBUG_ENABLE
  if (tot_len == LEN64) {
	if (DBG_IS_ARP(payload)) {
	
	#if 0 //tobe-used~
		printf("\r\n");
		printf("[%d]\r\n", 5); //part = 5.

		printf(" rx,(such as 'ARP')\r\n"); //no-need show. such as 'ARP'
		debug_rx_display_count(); //Put here~
		bannerline_log();
	#endif

	}
	#if 0
	else if (DBG_IS_TCP(payload)) //printf(" rx,(such as 'ACK')\r\n"); //no-need show. such as 'ACK'
		printf(" rx,(debug do process such as 'ACK')\r\n");
	else
		printf(" rx,(tiny LEN64)\r\n");
	#endif
  } //else
	//printf(" netif_input,rx (tot_len %u)\r\n", tot_len);
#endif
}

void dm9051_rxlog_ack(void *payload, uint16_t tot_len, char *result_str)
{
#if DM9051_DEBUG_ENABLE
  if (tot_len == 64) {
	  if (DBG_IS_TCP(payload)) {
		  printf(" .[debug process this 'ACK'] is %s\r\n", result_str);
	  }
  }
#endif
}
