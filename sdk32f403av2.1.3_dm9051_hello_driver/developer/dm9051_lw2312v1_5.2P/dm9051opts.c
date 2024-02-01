//
// dm9051opts.c
//
#include "dm9051_lw_conf.h"
//#include "dm9051_lw.h"

u8 gfirst_log[ETHERNET_COUNT];

#if HELLO_DRIVER_OPTS_DISPLAY_API
	void GpioDisplay(void) {
	  int i;
	  for (i = 0; i < ETHERNET_COUNT; i++) {
		mstep_set_net_index(i);
		printf("@%s, %s, %s\r\n", mstep_conf_info(), mstep_conf_cpu_spi_ethernet(), mstep_conf_cpu_cs_ethernet());
	  }
	}

	void ethcnt_ifdiplay_iomode(void)
	{
		int i;
		GpioDisplay();
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("iomode[%d] %s / value %02x\r\n", mstep_get_net_index(), dm9051opts_desciomode(), iomode()); //dm9051opts_iomode()
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("csmode[%d] %s\r\n", mstep_get_net_index(), dm9051opts_desccsmode());
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("bmcrmode[%d] %s\r\n", i, dm9051opts_descncrmode());
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("rx's mode[%d] %s\r\n", i, dm9051opts_descpromismode());
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("checksum[%d] %s / value %02x %s\r\n", i, dm9051opts_descrxmode_checksum_offload(), //~dm9051opts_desc_rxchecksum_offload(), 
					isrxmode_checksum_offload() ? RCSSR_RCSEN : 0,
					isrxmode_checksum_offload() ? "(RCSSR_RCSEN)" : " "); //is_dm9051opts_rxmode_checksum_offload ~is_dm9051opts_rxchecksum_offload
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("fcrmode[%d] %s / value %02x\r\n", i, dm9051opts_descflowcontrolmode(), 
					isflowcontrolmode() ? FCR_DEFAULT_CONF : 0); //(_dm9051optsex[mstep_get_net_index()]._flowcontrolmode)
		}
		
	#if TO_ADD_CODE_LATER_BACK
		for (i = 0; i< ETHERNET_COUNT; i++) {
			uint8_t *macaddr;
			mstep_set_net_index(i);
			macaddr = lwip_get_mac_addresse1();
			printf("config tobe mac[%d] %02x%02x%02x%02x%02x%02x\r\n", i, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
		}
	#endif
	#if TO_ADD_CODE_LATER_BACK
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			EepromDisplay(i);
		}
	#endif
	#if TO_ADD_CODE_LATER_BACK
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("ip[%d] %"U16_F".%"U16_F".%"U16_F".%"U16_F"\r\n", i,
			  ip4_addr1_16(netif_ip4_addr(&xnetif[i])), 
			  ip4_addr2_16(netif_ip4_addr(&xnetif[i])),
			  ip4_addr3_16(netif_ip4_addr(&xnetif[i])), 
			  ip4_addr4_16(netif_ip4_addr(&xnetif[i])));
		}
	#endif
	}

	void ethcnt_ifdiplay(void)
	{
	#if 0
		int i;
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			NetifDisplay(i);
			EepromDisplay(i);
		}
	#endif
	}
#endif

void first_log_init(void)
{
	int i;
	for (i = 0; i < ETHERNET_COUNT; i++)
		gfirst_log[i] = 1; //Can know the first time reset the dm9051 chip!
}

u8 first_log_get(int i)
{
	u8 if_log = 
		gfirst_log[i];
	gfirst_log[i] = 0; //first_log_clear(i);
	return if_log;
}

//void first_log_clear(int i)
//{
//	gfirst_log[i] = 0;
//}

void TRANS_CONN(trans_t trans_func, uint8_t trans_type)
{
	if (trans_type == UNIT_TRANS) {
		mstep_set_net_index(0); //+
		trans_func();
		return;
	}
	
	if (trans_type == ENUM_TRANS) {
	  int i;
	  for (i = 0; i < ETHERNET_COUNT; i++) {
		mstep_set_net_index(i); //+
		trans_func();; //dm9051_init(mstep_eth_mac());
	  }
	}
}
