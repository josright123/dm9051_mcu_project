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

void rx_hdlr(void);
err_t dhcp_discover(struct netif *netif);

int test_line7_enter_check_setup = 0; //.

/*void testproc_net_test(void)
{
#if (ETHERNET_COUNT >= 2)
  //
  // JJ.Test Multi-SPI.
  //
  bench_test_tx_and_rx(); //test_ethernetif_txrx();
  printf(": test end\r\n"); //printf(": while(1);\r\n");
#elif (ETHERNET_COUNT == 1)
  //
  // my debug test!
  //
	
	#if 0
	  //RXEN
	  cspi_write_reg(DM9051_RCR, 0x31);

	  //only unicast for me is allowed.
	  for (int i=0; i<8; i++)
		cspi_write_reg(DM9051_MAR+i, 0x00); // not accept broadcast, only unicast for me is allowed.
	#endif
  
	#if 0
	  if (in_time_link_up_found())
		  self_test_tx_and_rx(); //test...
	#endif
	//.in_time_link_up_found();
	  
	#if 0  
	  printf(": test end\r\n");
	  //printf(": while(1);\r\n");
	#endif

#endif
}*/

//
// "sys_timeout.c" (timeouts.c)
//

#define MAC_ADDR_LENGTH                  			(6)

#define IP3RD	37

#define MAC_5TH	IP3RD
uint8_t mac_addresse[MAC_ADDR_LENGTH] 				= {0, 0x60, 0x6e, 0x00, 0x01, MAC_5TH};

struct netif netif;

uint8_t *lwip_get_mac_addresse1(void){
	return mac_addresse;
}

void lwip_set_mac(uint8_t* macadd)
{
  //.MACaddr[0] = macadd[0];
  //.MACaddr[1] = macadd[1];
  //.MACaddr[2] = macadd[2];
  //.MACaddr[3] = macadd[3];
  //.MACaddr[4] = macadd[4];
  //.MACaddr[5] = macadd[5];
}

void
lwip_init(void)
{
  //...

  //. netif_init();

  //...
}

//
// netconf.c-ethernetif.c
//

#define IPCLO	0x00 //IP checksum
#define IPCHO	0x00 //IP checksum
#define UDCLO	0x00 //UDP checksum
#define UDCHO	0x00 //UDP checksum

#define IPCLx	0x03 //IP checksum
#define IPCHx	0x03 //IP checksum
#define UDCLx	0x03 //UDP checksum
#define UDCHx	0x03 //UDP checksum


//[Test2 TCP]
#define TCP_IPCL	0x34 //or IPCx
#define TCP_IPCH	0x73 // IPCx

#define TCP_CL	0x24 //or UDCx
#define TCP_CH	0x16 // UDCx

//[Test1 UDP]
#define UDP_IPCL	0x34 //or IPCx
#define UDP_IPCH	0x73 // IPCx

#define UDP_CL	0x24 //or UDCx
#define UDP_CH	0x16 // UDCx

const uint8_t buffer_tcp[] = {
	
//[TestX now]
#define IPCL	TCP_IPCL
#define IPCH	TCP_IPCH

#define TCCL	TCP_CL
#define TCCH	TCP_CH
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x08, 0x00, 0x45, 0x00, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x08, 0x00, 0x45, 0x00, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x08, 0x00, 0x45, 0x00, 
};

#if 1
const uint8_t buffer_discover[384] = {

#undef IPCL
#undef IPCH
//[TestX now]
#define IPCL	0x34 //or IPCLx
#define IPCH	0x73 // IPCHx

#define UDCL	0x24 //or UDCLx
#define UDCH	0x16 // UDCHx
/*0000   .ff .ff .ff .ff .ff ff 00 c0 ca 34 01 f4 08 00 45 .00
0010   .01 .72 .45 .09 .00 00 40 11 34 73 00 00 00 00 ff .ff,
0020   .ff .ff .00 .44 .00 43 01 5e 24 16 01 01 06 00 85 .eb,
0030   .7c .86 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
0040   .00 .00 .00 .00 .00 00 00 c0 ca 34 01 f4 00 00 00 .00,
0050   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
0060   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
0070   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
0080   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
0090   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
00a0   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
00b0   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
00c0   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
00d0   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
00e0   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
00f0   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
0100   .00 .00 .00 .00 .00 00 00 00 00 00 00 00 00 00 00 .00,
0110   .00 .00 .00 .00 .00 00 63 82 53 63 35 01 01 3d 07 .01,
0120   .00 .c0 .ca .34 .01 f4 50 00 74 01 01 39 02 05 c0 .3c,
0130   .2e .64 .68 .63 .70 63 64 2d 38 2e 31 2e 32 3a 4c .69,
0140   .6e .75 .78 .2d .35 2e 31 35 2e 33 34 2d 76 37 6c .2b,
0150   .3a .61 .72 .6d .76 37 6c 3a 42 43 4d 32 37 31 31 .0c,
0160   .0b .72 .61 .73 .70 62 65 72 72 79 70 69 91 01 01 .37,
0170   .0e .01 .79 .21 .03 06 0c 0f 1a 1c 33 36 3a 3b 77 .ff,*/
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x08, 0x00, 0x45, 0x00, \
0x01, 0x72, 0x45, 0x09, 0x00, 0x00, 0x40, 0x11, IPCL, IPCH, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, /* 0x34, 0x73*/ \
0xff, 0xff, 0x00, 0x44, 0x00, 0x43, 0x01, 0x5e, UDCL, UDCH, 0x01, 0x01, 0x06, 0x00, 0x85, 0xeb, /* 0x24, 0x16*/ \
0x7c, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x82, 0x53, 0x63, 0x35, 0x01, 0x01, 0x3d, 0x07, 0x01, \
0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x50, 0x00, 0x74, 0x01, 0x01, 0x39, 0x02, 0x05, 0xc0, 0x3c, \
0x2e, 0x64, 0x68, 0x63, 0x70, 0x63, 0x64, 0x2d, 0x38, 0x2e, 0x31, 0x2e, 0x32, 0x3a, 0x4c, 0x69, \
0x6e, 0x75, 0x78, 0x2d, 0x35, 0x2e, 0x31, 0x35, 0x2e, 0x33, 0x34, 0x2d, 0x76, 0x37, 0x6c, 0x2b, \
0x3a, 0x61, 0x72, 0x6d, 0x76, 0x37, 0x6c, 0x3a, 0x42, 0x43, 0x4d, 0x32, 0x37, 0x31, 0x31, 0x0c, \
0x0b, 0x72, 0x61, 0x73, 0x70, 0x62, 0x65, 0x72, 0x72, 0x79, 0x70, 0x69, 0x91, 0x01, 0x01, 0x37, \
0x0e, 0x01, 0x79, 0x21, 0x03, 0x06, 0x0c, 0x0f, 0x1a, 0x1c, 0x33, 0x36, 0x3a, 0x3b, 0x77, 0xff,
};
const uint8_t buffer_discovercc[384] = {
#undef IPCL
#undef IPCH
#undef UDCL
#undef UDCH
//[TestX now]
#define IPCL	0xcc
#define IPCH	0xcc

#define UDCL	0xcc
#define UDCH	0xcc
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x08, 0x00, 0x45, 0x00, \
0x01, 0x72, 0x45, 0x09, 0x00, 0x00, 0x40, 0x11, IPCL, IPCH, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, /* 0x34, 0x73*/ \
0xff, 0xff, 0x00, 0x44, 0x00, 0x43, 0x01, 0x5e, UDCL, UDCH, 0x01, 0x01, 0x06, 0x00, 0x85, 0xeb, /* 0x24, 0x16*/ \
0x7c, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x82, 0x53, 0x63, 0x35, 0x01, 0x01, 0x3d, 0x07, 0x01, \
0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x50, 0x00, 0x74, 0x01, 0x01, 0x39, 0x02, 0x05, 0xc0, 0x3c, \
0x2e, 0x64, 0x68, 0x63, 0x70, 0x63, 0x64, 0x2d, 0x38, 0x2e, 0x31, 0x2e, 0x32, 0x3a, 0x4c, 0x69, \
0x6e, 0x75, 0x78, 0x2d, 0x35, 0x2e, 0x31, 0x35, 0x2e, 0x33, 0x34, 0x2d, 0x76, 0x37, 0x6c, 0x2b, \
0x3a, 0x61, 0x72, 0x6d, 0x76, 0x37, 0x6c, 0x3a, 0x42, 0x43, 0x4d, 0x32, 0x37, 0x31, 0x31, 0x0c, \
0x0b, 0x72, 0x61, 0x73, 0x70, 0x62, 0x65, 0x72, 0x72, 0x79, 0x70, 0x69, 0x91, 0x01, 0x01, 0x37, \
0x0e, 0x01, 0x79, 0x21, 0x03, 0x06, 0x0c, 0x0f, 0x1a, 0x1c, 0x33, 0x36, 0x3a, 0x3b, 0x77, 0xff,
};
const uint8_t buffer_discover00[384] = {
#undef IPCL
#undef IPCH
#undef UDCL
#undef UDCH
//[TestX now]
#define IPCL	0x00
#define IPCH	0x00

#define UDCL	0x00
#define UDCH	0x00
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x08, 0x00, 0x45, 0x00, \
0x01, 0x72, 0x45, 0x09, 0x00, 0x00, 0x40, 0x11, IPCL, IPCH, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, /* 0x34, 0x73*/ \
0xff, 0xff, 0x00, 0x44, 0x00, 0x43, 0x01, 0x5e, UDCL, UDCH, 0x01, 0x01, 0x06, 0x00, 0x85, 0xeb, /* 0x24, 0x16*/ \
0x7c, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x82, 0x53, 0x63, 0x35, 0x01, 0x01, 0x3d, 0x07, 0x01, \
0x00, 0xc0, 0xca, 0x34, 0x01, 0xf4, 0x50, 0x00, 0x74, 0x01, 0x01, 0x39, 0x02, 0x05, 0xc0, 0x3c, \
0x2e, 0x64, 0x68, 0x63, 0x70, 0x63, 0x64, 0x2d, 0x38, 0x2e, 0x31, 0x2e, 0x32, 0x3a, 0x4c, 0x69, \
0x6e, 0x75, 0x78, 0x2d, 0x35, 0x2e, 0x31, 0x35, 0x2e, 0x33, 0x34, 0x2d, 0x76, 0x37, 0x6c, 0x2b, \
0x3a, 0x61, 0x72, 0x6d, 0x76, 0x37, 0x6c, 0x3a, 0x42, 0x43, 0x4d, 0x32, 0x37, 0x31, 0x31, 0x0c, \
0x0b, 0x72, 0x61, 0x73, 0x70, 0x62, 0x65, 0x72, 0x72, 0x79, 0x70, 0x69, 0x91, 0x01, 0x01, 0x37, \
0x0e, 0x01, 0x79, 0x21, 0x03, 0x06, 0x0c, 0x0f, 0x1a, 0x1c, 0x33, 0x36, 0x3a, 0x3b, 0x77, 0xff,
};
#endif

const uint8_t buffer_sync[66] = {
#undef IPCL
#undef IPCH
#undef TCCL
#undef TCCH
//[TestX now]
#define IPCL	0x68
#define IPCH	0x73
#define TCCL	0xfd
#define TCCH	0xa2
0xdc, 0xa6, 0x32, 0x22, 0xff, 0xb3, 0x08, 0x97, 0x98, 0xc3, 0xb9, 0xa2, 0x08, 0x00, 0x45, 0x00, \
0x00, 0x34, 0x04, 0xfa, 0x40, 0x00, 0x80, 0x06, IPCL, IPCH, 0xc0, 0xa8, 0x06, 0x04, 0xc0, 0xa8, \
0x06, 0x02, 0xdc, 0x7e, 0x00, 0x50, 0x4d, 0x8d, 0xbe, 0xc9, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, \
0xfa, 0xf0, TCCL, TCCH, 0x00, 0x00, 0x02, 0x04, 0x05, 0xb4, 0x01, 0x03, 0x03, 0x08, 0x01, 0x01, \
0x04, 0x02
};

const uint8_t buffer_syncc[66] = {
#undef IPCL
#undef IPCH
#undef TCCL
#undef TCCH
//[TestX now]
#define IPCL	0xcc
#define IPCH	0xcc
#define TCCL	0xcc
#define TCCH	0xcc
0xdc, 0xa6, 0x32, 0x22, 0xff, 0xb3, 0x08, 0x97, 0x98, 0xc3, 0xb9, 0xa2, 0x08, 0x00, 0x45, 0x00, \
0x00, 0x34, 0x04, 0xfa, 0x40, 0x00, 0x80, 0x06, IPCL, IPCH, 0xc0, 0xa8, 0x06, 0x04, 0xc0, 0xa8, \
0x06, 0x02, 0xdc, 0x7e, 0x00, 0x50, 0x4d, 0x8d, 0xbe, 0xc9, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, \
0xfa, 0xf0, TCCL, TCCH, 0x00, 0x00, 0x02, 0x04, 0x05, 0xb4, 0x01, 0x03, 0x03, 0x08, 0x01, 0x01, \
0x04, 0x02
};

const uint8_t buffer_syn00[66] = {
#undef IPCL
#undef IPCH
#undef TCCL
#undef TCCH
//[TestX now]
#define IPCL	0x00
#define IPCH	0x00
#define TCCL	0x00
#define TCCH	0x00
0xdc, 0xa6, 0x32, 0x22, 0xff, 0xb3, 0x08, 0x97, 0x98, 0xc3, 0xb9, 0xa2, 0x08, 0x00, 0x45, 0x00, \
0x00, 0x34, 0x04, 0xfa, 0x40, 0x00, 0x80, 0x06, IPCL, IPCH, 0xc0, 0xa8, 0x06, 0x04, 0xc0, 0xa8, \
0x06, 0x02, 0xdc, 0x7e, 0x00, 0x50, 0x4d, 0x8d, 0xbe, 0xc9, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, \
0xfa, 0xf0, TCCL, TCCH, 0x00, 0x00, 0x02, 0x04, 0x05, 0xb4, 0x01, 0x03, 0x03, 0x08, 0x01, 0x01, \
0x04, 0x02
};

//Logic 
//  0. start with a loopback terminator connected
//  1. fcr -- flow control mode
//  2. bp -- back pressurr mode
enum {
	TST_START_CONNECT_LOOPBK_HEAD = 0,
	TST_LINKUP_FOUND,
	TST_SEND_DONE,
	TST_CHANG_CONNECT_PC,
	TST_FCR_INGRESS, //TST_FCR_RECEIVING,
	TST_FCR_DUMPING,
	TST_BP_INGRESS, //TST_BP_RECEIVING,
	TST_BP_DUMPING,
	TST_END,
};
//0: start_run and wait until link_up, 
//   link_up does an action:
//1: lnkup found, checksum_test_send, checksum_test_send_done, so assigned to be 1 
//   checksum_test_send_done, does an action, receiveing sent packets. 
//2: while receiving is all tx packets be received, so so assigned to be 2
//   this action as fcr-receiving.
static int testproc_stage = TST_START_CONNECT_LOOPBK_HEAD; 
//int _sending_done_flag = 0; //static int fcr_stage_flag = 0;

static int test_stage_flag = 0, rxtest_stage_flag = 0;
int testing_rx_count = 0;
int testing_tx_count = 0;

int get_testing_rx_count(void)
{
	return testing_rx_count;
}

//
// netconf.c
//
#define DUMP_FREQ_MS 25 //250 //1500 note: more fast is better, and it can continuous input packets recyclely.
static uint32_t local_time;

//static void moni_show_rxrp(void) {
//}

//static int moni_read_rxrp(void)
//{
//}

static void rxrp_dump_print_init_show(void) { //init show
	uint16_t rxrp;
	//printf("%s: Update rxrp - - - debug (txc %d rxc %d) (checkrxbcount %d)\r\n",
	//	"init", testing_tx_count, testing_rx_count, check_get()); //" _sending_done_flag %d", sending_done_flag
	
	//=moni_show_rxrp(); //~_moni_read_rxrp();
	rxrp = cspi_read_reg(DM9051_MRRL);
	rxrp |= cspi_read_reg(DM9051_MRRH) << 8;
	printf(" %4x", rxrp);
	
	local_time = sys_now() + DUMP_FREQ_MS; //since 'fcr_stage_flag' true
}
static int rxrp_read_print_diff_show(void) { //diff show
	//=return moni_read_rxrp();
	static uint16_t moni_rxrp = 0;
	uint16_t rxrp;
	rxrp = cspi_read_reg(DM9051_MRRL);
	rxrp |= cspi_read_reg(DM9051_MRRH) << 8;
	if (rxrp != moni_rxrp) {
		moni_rxrp = rxrp;
		printf(" %4x", moni_rxrp);
		return 1;
	}
	return 0;
}

//
// netconf.c
//

static void moni_show_rxwp(void) { //.init
	uint16_t rxwp;
	rxwp = cspi_read_reg(DM9051_RWPAL);
	rxwp |= cspi_read_reg(DM9051_RWPAH) << 8;
	printf(" %4x", rxwp);
}

static int moni_read_rxwp(void) { //.diff
	static uint16_t moni_rxwp = 0;
	uint16_t rxwp;
	rxwp = cspi_read_reg(DM9051_RWPAL);
	rxwp |= cspi_read_reg(DM9051_RWPAH) << 8;
	if (rxwp != moni_rxwp) {
		moni_rxwp = rxwp;
		printf(" %4x", moni_rxwp);
		return 1;
	}
	return 0;
}

static void rxwp_read_print_init(void) { //init show
	//printf("%s: Update rxwp - - - debug (txc %d rxc %d) (checkrxbcount %d)\r\n",
		//"init", testing_tx_count, testing_rx_count, check_get()); //"sending_done_flag %d", sending_done_flag
	moni_show_rxwp(); //_moni_read_rxwp("init");
}
static int rxwp_read_print_diff(void) { //diff show
	return moni_read_rxwp();
}


uint8_t save_fcr, on_fcr;

void threads_support(void)
{
	sys_check_timeouts();
}

void testproc_board_initialize(void)
{
  //printf("\r\n");
  //printf("- dm9051_board_initialize [%d spi board(s), 'devconf' as %s]\r\n", mstep_conf_spi_count(), mstep_conf_type());
  //printf("- dm9051_board_initialize [%d eth device(s)]\r\n", ETHERNET_COUNT);
  bannerline();
  banner("=== [Power On DM9051/DM9052] ===");
  dm9051_board_initialize(); //netif_create(&ethernetif_create); //at32_dm9051_init_configuration_all(); //env_dm9051f_system_init();
  //printf("\r\n");
}

uint16_t testproc_drv_initialize(void)
{
#if (ETHERNET_COUNT >= 2)
	//=ethernetif_dm9051_init_dual(MACaddr);
	int i;
	const uint8_t addr_meanless[6] = { 0 }; //no exactly need.

	for (i = 0; i < ETHERNET_COUNT; i++) { //get_eth_interfaces()
		mstep_set_net_index(i); //set_pin_code(i);
		dm9051_poweron_rst();
		dm_delay_ms(1);
		dm9051_init(addr_meanless);
	}
	mstep_set_net_index(0);
	return 0;
#elif (ETHERNET_COUNT == 1) 
	//
	// test init dual only need while only 1 ethernet port exist.
	//
	
	//=test_dm9051_init_dual(MACaddr);
	uint16_t id;

	dm9051_poweron_rst();
	dm_delay_ms(1);

	//printf("................................ dm9051 init\r\n");
	id = dm9051_init(MACaddr);
	
	#if NON_CHIPID_STOP == 0 //test version, itdelf
	if (!check_chip_id(id)) {
	  printf("Chip ID wrong! Check the device board!\r\n");
	  printf(": test end\r\n");
	  printf(": while(1);\r\n");
	  while(1) ;
	}
	#endif

	if (is_dm9051_board_irq()) {
		#if 1
		test_line7_enter_check_setup = 1;
		printf("................................ dm9051 board_irq_enable, test_line7_enter_check_setup %d\r\n", test_line7_enter_check_setup);
		//.display_verify_chipid("dm9051_init", mstep_spi_conf_name(), id);
		#endif
	}
	return id;
#endif
}

void link_display(char *str, char *speed, char *duplex, char *up)
{
  //char *up, *speed, *duplex;
  uint16_t bmsr = dm9051_bmsr_update();
  uint16_t bmcr = dm9051_bmcr_update();
  
  up = (bmsr & 0x0004) ? "up" : "down";
  speed = (bmcr & 0x2000) ? "100M" : "10M";
  duplex = (bmcr & 0x0100) ? "Full" : "Half";
  
  printf(str, speed, duplex, up); //.print_lnkcb_up()
}

void down_display(char *str)
{
	if (testproc_stage == TST_CHANG_CONNECT_PC)
	    printf("\r\non: fcr wait connect to a PC which doese keep sending packets\r\n");
	if (testproc_stage == TST_FCR_INGRESS)  {
		bannerline();
		printf("\r\non: fcr receiving\r\n");
	}
	if (testproc_stage == TST_FCR_DUMPING)
		printf("\r\non: fcr dumping\r\n");
	if (testproc_stage == TST_BP_INGRESS) {
		bannerline();
		printf("\r\non: back presure ingress\r\n");
	}
		
	printf(str);
		
	if (testproc_stage == TST_CHANG_CONNECT_PC)
	    printf("to: dm9051 will ingress fcr packets intto rx memory\r\n"); //printf("to: if connect to PC will fcr enter packets to rx memory ...\r\n");
	if (testproc_stage == TST_FCR_INGRESS) 
		printf("to: next will fcr-dumping ...\r\n"); //while next link-up
	if (testproc_stage == TST_FCR_DUMPING) 
		printf("to: next will back-pressure-ingressing\r\n");
	if (testproc_stage == TST_BP_INGRESS)
		printf("to: next will back-pressure-dumping ...\r\n"); //while next link-up
#if 1
	rxwp_read_print_diff(); //_rxwp_read_print_init();
	printf("\r\n");
#endif
}

void macro_lnkcb_up(char *str, char *speed, char *duplex, char *up)
{
  if (testproc_stage == TST_FCR_INGRESS) 
    printf("on: for fcr-dumping\r\n");
  else if (testproc_stage == TST_BP_INGRESS)
	printf("on: for back-pressure-dumping\r\n");
  else if (testproc_stage == TST_BP_DUMPING)
	printf("on: Last TST_BP_DUMPING, link-up to recycle\r\n");
  else
    printf("on: link-up\r\n");
	
  link_display(str, speed, duplex, up);
		
  if (testproc_stage == TST_CHANG_CONNECT_PC)
	;
  if (testproc_stage == TST_FCR_INGRESS) 
	printf("to: fcr-dumping the rx packets in rx memory\r\n");
  if (testproc_stage == TST_FCR_DUMPING) 
	printf("to: fcr-receiving\r\n");
  if (testproc_stage == TST_BP_INGRESS)
	printf("to: back-pressure-dumping the rx packets in rx memory\r\n");
  
  //if (!test_stage_flag)
  linkchangeup_sending();
}

void macro_lnkcb_down(void)
{
  uint16_t bmcr;
  char *down_disp_head;
  if (testproc_stage == TST_FCR_DUMPING) {
	phy_write(0, 0x0000);
	bmcr= phy_read(0);
	bannerline();
	bannerline();
	printf("BMCR write/read : %04x/%04x\r\n", 0x0000, bmcr);
	down_disp_head = "tmr: dm9051 link down [set link parameters, for 10M Half]\r\n";
	  //_down_display();
  }
  else if (testproc_stage == TST_BP_INGRESS) {
	bannerline();
	down_disp_head = "tmr: dm9051 link down [while 10M Half]\r\n";
	//_down_display();
  }
  else if (testproc_stage == TST_BP_DUMPING) {
	phy_write(0, 0x3300);
	bmcr= phy_read(0);
	bannerline();
	bannerline();
	banner("=== [Recycle] ===");
	printf("BMCR write/read : %04x/%04x\r\n", 0x3300, bmcr);
	down_disp_head = "tmr: dm9051 link down [set link parameters, A.N. for 100M Full]\r\n";
	//_down_display();
  }
  else
	down_disp_head = "tmr: dm9051 link down\r\n";
	//_down_display();
  
  down_display(down_disp_head);
}

void linkup_cb(net_t *net, void *arg, uint8_t status) {
  if (status == NET_LINK_UP) {
	  char *up, *speed, *duplex;
	  netif_set_link_up1(); //netif_set_link_up_INITAndTimer(&netif);
	  macro_lnkcb_up("tmr: dm9051 link up (%s %s %s)\r\n", speed, duplex, up);

	//#if LWIP_NETIF_LINK_CALLBACK == 0
	//	env_ethernetif_update_config_cb(&netif);
	//#endif
  }
  if (status == NET_LINK_DOWN) {
    
	netif_set_link_down1(); //netif_set_link_down_Timer(&netif);
	macro_lnkcb_down();
  }
}

int in_time_link_up_found(void)
{
  int i = 0;

  printf("\r\n");
  printf("in_time_link: ");
  printf("\r\n");
  while(i < 20) {
	uint16_t bmsr;
	
	dm_delay_ms(100);
	bmsr = dm9051_bmsr_update(); //dm9051_link_update(); //nsr = spi_read_reg(DM9051_NSR);
	if (bmsr & PHY_LINKED_BIT) { //(nsr & NSR_LINKST)
		printf(".%04x link is up\r\n", bmsr);
	  //printf(". link is up\r\n");
		netif_set_link_up1();
		linkchangeup_sending(); //,netif_set_link_up_INITAndTimer(&netif);
		return 1;
	}

 #if 0
	if (i == 0)
		printf("  ");
 #endif
	i++;
	
	if (i < 20)
		printf(".%04x\r\n", bmsr);
	else
		printf(".%04x link is down\r\n", bmsr);
	//	printf(".%04x \r\n.  link is down\r\n", bmsr);
	//printf("%s", (i < 20) ? "." : ". \r\n.  link is down\r\n");
	
  }
  netif_set_link_down1(); //netif_set_link_down_Timer(&netif);
  return 0;
}

//
// test main procedure
//

void testproc_run(void)
{
  netlink_init();
	
  at32_dm9051_tmr_init(); //dm9051_tmr_init(); //old: env_dm9051_tmr_init()
  #if 0
  printf(":  Connect Detecting...\r\n");
  printf("\r\n");
  #endif

  while(1)
  {
	  rx_hdlr();
	  threads_support();
  }
}

//void rx_hdlr_inc(void)
//{
//}

void rx_hdlr(void)
{ 
  if (testproc_stage == TST_FCR_DUMPING || testproc_stage == TST_BP_DUMPING) { //(fcr_stage_flag)
	  if (sys_now() > local_time) {
		  local_time += DUMP_FREQ_MS;
		  dump_rx_hdlr();
		  if (rxrp_read_print_diff_show()) {  
			rxtest_stage_flag++;
			if (!(rxtest_stage_flag % 16))
				printf("\r\n");
		  }
	  }
	  return;
  }
	
  if (test_stage_flag) {
	if (rxwp_read_print_diff()) {
	  test_stage_flag++;
	  if (!(test_stage_flag % 16))
		  printf("\r\n");
	  
	  //
	  // while first time rxwp is move, change_connect_pc update to fcr_receiving 
	  //
	  if (testproc_stage == TST_CHANG_CONNECT_PC) // note: if not, will be fcr_receiving, always ethier change_connect_pc update or fcr_receiving.
		testproc_stage = TST_FCR_INGRESS;
	}
  }
  
  if (testproc_stage == TST_SEND_DONE) { //(sending_done_flag == 1)
	  
	lwip_rx_hdlr(); //if (lwip_rx_hdlr() == ERR_OK) testing_rx_count++; //= to be, rx_hdlr_inc();
		
	if (testing_rx_count == testing_tx_count) {
		uint8_t buf[6];
	#if 1
		cspi_read_regsS(DM9051_PAR, buf, 6);
	#else
		buf[0] = cspi_read_reg(DM9051_PAR); buf[1] = cspi_read_reg(DM9051_PAR+1); buf[2] = cspi_read_reg(DM9051_PAR+2);
		buf[3] = cspi_read_reg(DM9051_PAR+3); buf[4] = cspi_read_reg(DM9051_PAR+4); buf[5] = cspi_read_reg(DM9051_PAR+5);
	#endif
		
		printf("= IP/UDP/TCP checksum offload testing done ...\r\n");
		
		banner("Flow-control INGRESS Test");
		delay_ms(1250);
			//cspi_read_reg(DM9051_PAR), cspi_read_reg(DM9051_PAR+1), cspi_read_reg(DM9051_PAR+2),
			//cspi_read_reg(DM9051_PAR+3), cspi_read_reg(DM9051_PAR+4), cspi_read_reg(DM9051_PAR+5)
		
		//cspi_write_reg() flow-control disable state
		// ...
		//cspi_write_reg() flow-control enable state (ENABLE)
		// ...
		
		//.printf("Start-enable cspi_read_reg() fcr = %02x\r\n", cspi_read_reg(DM9051_FCR));
		save_fcr = cspi_read_reg(DM9051_FCR);
		cspi_write_reg(DM9051_FCR, FCR_DEFAULT1); //FCR_DEFAULT
		//do {
		  //uint8_t value_fcr = FCR_DEFAULT;
		  //banner_arg("Start FCR %02x", value_fcr );
			
		//} while(0);
		
		//printf("Check-enable cspi_read_reg() fcr = %02x\r\n", cspi_read_reg(DM9051_FCR));
		banner_arg("Start FCR %02x", cspi_read_reg(DM9051_FCR));
		printf("Mac address: %02x %02x %02x %02x %02x %02x\r\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
		
		printf("PLEASE CONNECT TO A PC!\r\n");
		rxwp_read_print_diff(); //_rxwp_read_print_init();
		printf("\r\n");
		#if 0
		//._rxwp_read_print_init();
		//.test_stage_flag = 1; //"Receiving enable, NOT CALL lwip_rx_hdlr, for testing flow-control ..."
		#endif
		
		testproc_stage = TST_CHANG_CONNECT_PC; //_TST_FCR_RECEIVING; //sending_done_flag = 0;
	}
  }
  #if 0  
  else
  if (_test_stage_flag == 1) {
	  _rxwp_read_print_diff();
  }
  #endif
}

/*void send_udp_main(void)
{
  uint8_t *mac_address;
  ip_addr_t cfg_ipaddr;
  ip_addr_t cfg_netmask;
  ip_addr_t cfg_gw;
  
  cfg_ipaddr.addr = 0; //.addr = 0;
  cfg_netmask.addr = 0; //netmask.addr = 0;
  cfg_gw.addr = 0; //gw.addr = 0;
  
  // Initialize the LwIP stack
  printf("\r\n");
  printf("lwip_init().s\r\n");
  lwip_init();
  
  //lwip_get_mac_addresse0(mac_address);
  mac_address = lwip_get_mac_addresse1();
  lwip_set_mac(mac_address); //tcpip_set_mac_address(); //_lwip_set_mac_address(mac_address);
	  
  printf("netif_add().s\r\n");
  if (netif_add(&netif, &cfg_ipaddr, &cfg_netmask, &cfg_gw, NULL, &ethernetif_init, &netif_input) == NULL)
  {
	printf("netif_add().e\r\n");
	printf("netif_add() fail\r\n");
	while(1);
  }
  
  //printf("\r\n");
  printf("dhcp_discover().s\r\n");
  #if 0
  dhcp_start(&netif);
  #else
  dhcp_new_jj();
  dhcp_discover(&netif); //LWIP_SINGLE_NETIF, netif_list
  #endif
}*/

void bannerline(void)
{
  printf("\r\n");
}

void banner(char *str)
{
  int n =  strlen(str);
  n = (n >= 65) ? 0 : (65 - n) >> 1;
//"----------------------------------------------------------------"
  printf("\r\n");
  printf("----------------------------------------------------------------\r\n");
  while (n--) printf(" ");
  printf("%s\r\n", str);
  printf("----------------------------------------------------------------\r\n");
  //printf("\r\n");
}

#define ARG_U8	0
#define ARG_U16	1

void banner_arg1(int argtype, char *str, void *value)
{
//----------------------------------------------------------------
  char buffer[160];
  uint8_t *p8 = value;
  uint16_t *p16 = value;
  int n;
  
  //strcpy(buffer, str);
  //if (argtype == ARG_U8)
	//printf(buffer, *p8);
  //if (argtype == ARG_U16)
	//printf(buffer, *p16);
  //printf("\r\n");

  if (argtype == ARG_U8)
	  sprintf(buffer, str, *p8);
  else if (argtype == ARG_U16)
	  sprintf(buffer, str, *p16);
  else
	  sprintf(buffer, str, *p16);
  
  n = strlen(buffer);
  n = (n >= 65) ? 0 : (65 - n) >> 1;
  
  printf("\r\n");
  printf("----------------------------------------------------------------\r\n");
  while (n--) printf(" ");
  printf("%s\r\n", buffer);
  printf("----------------------------------------------------------------\r\n");
  printf("\r\n");
}

void banner_arg(char *str, uint8_t value)
{
  //uint8_t val = value;
  banner_arg1(ARG_U8, str, &value);
}

void linkchangeup_sending(void)
{	
  //printf("Test program send DHCP discover (UDP) packet:\r\n");
  //printf("Test no checksum value (observ with the partner peer's wireshark ...)\r\n");

#if TEST_PLAN_MODE //TestMode

  if (testproc_stage == TST_START_CONNECT_LOOPBK_HEAD) {
	  uint8_t *buffer = get_tx_buffer(); //&EthBuff[0].tx; //Tx_Buff;
	  int l = 0;
	  
	  banner("UDP checksum offload");
	  //Send_UDP();

	  out_packet(OUT_TEST_UDP, buffer, l, buffer_discovercc, sizeof(buffer_discovercc));
	  out_packet(OUT_TEST_UDP, buffer, l, buffer_discover00, sizeof(buffer_discover00));
	#if TEST_PLAN_MODE
	//.Write to [TCSCR_UDPCS_ENABLE/TCSCR_TCPCS_ENABLE/TCSCR_IPCS_ENABLE]
	  //.printf("Start-generation cspi_read_reg() = %02x\r\n", cspi_read_reg(DM9051_CSCR));
	  cspi_write_reg(DM9051_CSCR, TCSCR_UDPCS_ENABLE | TCSCR_IPCS_ENABLE);
	  banner("cspi_write_reg DM9051_CSCR Reg31H [IPCS_ENABLE/UDPCS_ENABLE] 0x05");
	  //printf("cspi_read_reg() = %02x\r\n", cspi_read_reg(DM9051_CSCR));
	#endif
	  out_packet(OUT_TEST_UDP, buffer, l, buffer_discovercc, sizeof(buffer_discovercc));
	#if TEST_PLAN_MODE
	//.Write to [0]
	  cspi_write_reg(DM9051_CSCR, 0);
	  //banner("cspi_write_reg DM9051_CSCR 0x31 [IPCS_DISABLE/UDPCS_DISABLE] 0");
	  //printf("cspi_read_reg(DM9051_CSCR) = %02x\r\n", cspi_read_reg(DM9051_CSCR));
	#endif
	  //.send_udp_main(); //out_packet(OUT_TEST_UDP, ...);
	  out_packet(OUT_TEST_UDP, buffer, l, buffer_discover, sizeof(buffer_discover));
		
	  bannerline();
	  banner("TCP checksum offload");
	  //Send_TCP();
		
	  out_packet(OUT_TEST_TCP, buffer, l, buffer_syncc, sizeof(buffer_syncc));
	  out_packet(OUT_TEST_TCP, buffer, l, buffer_syn00, sizeof(buffer_syn00));
	#if TEST_PLAN_MODE
	//.Write to [TCSCR_UDPCS_ENABLE/TCSCR_TCPCS_ENABLE/TCSCR_IPCS_ENABLE]
	  //.printf("Start-generation cspi_read_reg() = %02x\r\n", cspi_read_reg(DM9051_CSCR));
	  cspi_write_reg(DM9051_CSCR, 0x07); //v.s. TCSCR_TCPCS_ENABLE | TCSCR_IPCS_ENABLE
	  banner("cspi_write_reg DM9051_CSCR Reg31H [IPCS_ENABLE/TCPCS_ENABLE] 0x03");
	  //printf("cspi_read_reg() = %02x\r\n", cspi_read_reg(DM9051_CSCR));
	#endif
	  out_packet(OUT_TEST_TCP, buffer, l, buffer_syn00, sizeof(buffer_syn00)); //v.s. buffer_syncc, sizeof(buffer_syncc)
	#if TEST_PLAN_MODE
	//.Write to [0]
	  cspi_write_reg(DM9051_CSCR, 0);
	  //banner("cspi_write_reg DM9051_CSCR 0x31 [IPCS_DISABLE/TCPCS_DISABLE] 0");
	  //printf("cspi_read_reg(DM9051_CSCR) = %02x\r\n", cspi_read_reg(DM9051_CSCR));
	#endif
	  out_packet(OUT_TEST_TCP, buffer, l, buffer_sync, sizeof(buffer_sync));
		
	  banner("Next, Receiving IP/UDP/TCP packetS ...");
	  delay_ms(1250);
	  
	#if 1 //STATEs
	  testproc_stage = TST_SEND_DONE;  //sending_done_flag = 1; //rx is allowed
	  check_set_new(); //logic applied start. (do when need monotor 'rxb')
	#endif //STATEs
	  return;
  }
	
//[no-need.s]
  if (testproc_stage == TST_CHANG_CONNECT_PC
		|| testproc_stage == TST_BP_INGRESS || testproc_stage == TST_FCR_INGRESS) { //.if (test_stage_flag > 0) //rx is not allowedif (test_stage_flag) 
		  test_stage_flag += 30; // each time link-up accumulated! untill goal. 
		  if (test_stage_flag > 100) {
			  
			  if (testproc_stage == TST_FCR_INGRESS) {
				  
				  testproc_stage = TST_FCR_DUMPING;			//.sending_done_flag = 0; //fcr_stage_flag = 1;
				  rxtest_stage_flag = 1; // such as increase one.
				  cspi_write_reg(DM9051_RCR, 0); // not RCR_RXEN and all bits others.
				  banner("Disable RCR_EN first, only dumping NOT receiving...");
				  
				  #if 0
				  //.cspi_write_reg(DM9051_FCR, save_fcr);
				  //.banner_arg("Set back FCR %02x", save_fcr);
				  #else
				  on_fcr = cspi_read_reg(DM9051_FCR);
				  banner_arg("View current FCR %02x -- for Flow control mode enable, dumping...", on_fcr);
				  #endif
				  
				  rxrp_dump_print_init_show(); //_moni_read_rxrp(); //"init"
			  }
			  if (testproc_stage == TST_BP_INGRESS) {
				  
				  testproc_stage = TST_BP_DUMPING;	
				  rxtest_stage_flag = 1; // such as increase one.
				  cspi_write_reg(DM9051_RCR, 0); // not RCR_RXEN and all bits others.
				  banner("Disable RCR_EN first, only dumping NOT receiving...");
				  
				  on_fcr = cspi_read_reg(DM9051_FCR);
				  banner_arg("View current FCR %02x -- for Back Pressure mode enable, dumping...", on_fcr);
				  
				  rxrp_dump_print_init_show(); //_moni_read_rxrp(); //"init"
			  }
		  } else {
			  if (testproc_stage == TST_FCR_INGRESS || testproc_stage == TST_BP_INGRESS) {
				  printf("Go DUMPING: need t-count > 100, t-count now %d\r\n", test_stage_flag);
				  printf("(Unplug and plug the ethernrt cable, Please.)\r\n");
			  }
		  }
		  return;
  }
  //.rxwp_read_print_init();
//[no-need.e]  

  if (testproc_stage == TST_FCR_DUMPING) {
	  char *up, *speed, *duplex;;
	  testproc_stage = TST_BP_INGRESS;
	  
	  banner("Ingress packets, Back Pressure mode ...");
	  link_display("TST_BP_INGRESS more check (%s %s %s)\r\n", speed, duplex, up);
	  #if 1
	  rxwp_read_print_init();
	  test_stage_flag = 1; //"Receiving enable, NOT CALL lwip_rx_hdlr, for testing flow-control ..."
	  #endif
	  cspi_write_reg(DM9051_RCR, RCR_DEFAULT | RCR_PRMSC | RCR_RXEN); //cspi_write_reg(DM9051_FCR, );
  }
  
  if (testproc_stage == TST_BP_DUMPING) {
	  char *up, *speed, *duplex;;
	  testproc_stage = TST_FCR_INGRESS;
	  
	  banner("Ingress packets, Flow Control eanble mode ...");
	  link_display("TST_FCR_INGRESS more check (%s %s %s)\r\n", speed, duplex, up);
	  #if 1
	  rxwp_read_print_init();
	  test_stage_flag = 1; //"Receiving enable, NOT CALL lwip_rx_hdlr, for testing flow-control ..."
	  #endif
	  cspi_write_reg(DM9051_RCR, RCR_DEFAULT | RCR_PRMSC | RCR_RXEN); //cspi_write_reg(DM9051_FCR, );
  }
#endif //TestMode
}
