//#include "stdint.h"
//#include "dm9051_at32_decl.h"
//#include "dm9051_debug_lw.h"
#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_log_types.h"
//#include "dm9051_lw_testproc_type.h"
//#include "dm9051_lw_log.h"

#if !DM9051_DEBUG_ENABLE
#error "Please don't add this implement C file, because you did NOT enable debug packets dump in the 'dm9051opts.h',\
 Or to HAVE packets dump functions you can define DM9051_DEBUG_ENABLE to 1 in the 'dm9051opts.h'"
#endif

#define	IsMulticast(buf)	(((uint8_t *)(buf))[0] & 0x01)

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
	if (arp_count < 2) {
		arp_count++;
		printf("---------------Sending ARP(%d/2) total_tx(%d)\r\n", arp_count, count);
		function_monitor_tx(buf, len);
		printf("\r\n");
	}
#endif
	return;
  } 
  
  if (IS_UDP) {
	  if (IS_DHCPTX) {
		printf("---------------Sending DHCP total_tx(%d) Len %u\r\n", count, len);
		function_monitor_tx_all(HEAD_SPC, buf, len);
	  }
#if 0
	  else {
		  if (udp_count < 3) {
			udp_count++;
			printf("---------------Sending packet(%d/3) total_tx(%d)\r\n", udp_count, count);
			function_monitor_tx(buf, len);
		  }
	  }
#endif
	  return;
  }
  
  if (len > 70) { //add.
    printf("---------------Sending total_tx(%d)\r\n", count);
    function_monitor_tx_all(HEAD_SPC, buf, len);
  }
}

void debug_rx(const uint8_t *buf, uint16_t len)
{
#if 0
  static int icmp_count_rx = 0;
  static int rx_mult_tcp_count = 0;
  static int rx_mult_udp_count = 0;
#endif
  static int master_TCP_count = 0;
	
  if (IsMulticast(buf)) { 
    if (IS_ARP)
		return;
	
	if (IS_DHCPRX) {
		printf("Receive DHCP pkt (%02x:%02x:%02x:%02x:%02x:%02x) len %d ---------------\r\n", 
			buf[0],buf[1],buf[2],buf[3],buf[4],buf[5], len);
#if 0
		function_monitor_rx_all(buf, len);
#endif
	#if 1
		printf("Your client ID: %d.%d.%d.%d\r\n", buf[0x3a], buf[0x3b], buf[0x3c], buf[0x3d]); //0x3a = 58
		printf("\r\n");
	#endif
		return;
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
			return;
		}
		if (IS_UDP) {
			if (rx_mult_udp_count < 3) {
				rx_mult_udp_count++;
				printf("Receive Multicast(%d/3) and UDP: (%02x:%02x:%02x:%02x:%02x:%02x) Protocol: %s (%d) ---------------\r\n", 
					rx_mult_udp_count, buf[0],buf[1],buf[2],buf[3],buf[4],buf[5], "TBD", IPBUF->proto);
				function_monitor_rx(buf, len);
			}
			return;
		}
			
		printf("Receive Multicast and OP: (%02x:%02x:%02x:%02x:%02x:%02x) ? ---------------\r\n", 
			buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
		function_monitor_rx(buf, len);
		return;
	}
	
	printf("Receive Multicast and UNKNOW Multicast packet: (%02x:%02x:%02x:%02x:%02x:%02x) ---------------\r\n", 
				buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	function_monitor_rx_all(buf, len);
#endif
	return;
  }
  
  if (IS_ARP)
	return; //uni-cast arp
 
  if (IS_ICMP) {
#if 0
		if (icmp_count_rx < 2) {
			icmp_count_rx++;
			printf("Receive icmp(%d/2) Protocol: %s (%d) ---------------\r\n", icmp_count_rx, "ICMP", IPBUF->proto);
			function_monitor_rx(buf, len);
		}
#endif
		return;
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
			return;
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
	return;
  }
  
  printf("Receive unit-cast UNKNOW pkt ---------------\r\n");
  function_monitor_rx(HEAD_SPC, buf, len); //(buffer, l);
}
