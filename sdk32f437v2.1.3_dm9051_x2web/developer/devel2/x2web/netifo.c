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

#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "testproc/testproc_lw.h" //#include "dm9051_lw_testproc.h"
#include "testproc/ethernetif.h"
*/

#include "lwip/opt.h"

//#if LWIP_IPV4 && LWIP_DHCP /* don't build if not configured for use in lwipopts.h */
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

#include <string.h>
//#endif //LWIP_IPV4 && LWIP_DHCP

#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "dm9051_lw.h" //#include "dm9051opts.h"

#include "ethernetif.h"

#include "netconf.h"
#include "testproc/testproc_lw.h"

int testing_tx_count = 0;

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
	
	//dm9051_txlog_monitor_tx_all(2, buffer, l);
	dm9051_txlog_monitor_tx(2, buffer, l);
	//dm9051_txlog_monitor_tx_all(2, buffer, l); //HEAD_SPC
	  
	  dm9051_tx(buffer, (uint16_t) l); //= dm9051_tx_dual(buffer, (uint16_t) l); //dm9051_tx(.);

	 testing_tx_count++;
	 bannerline();
	 txlen_show_rxwp(l);
	 bannerline();
		
	#if 0
	  dm9051_txlog_disp(buffer, l); //= process_txlog_debug(buffer, l);
	#endif

	return ERR_OK;
}
