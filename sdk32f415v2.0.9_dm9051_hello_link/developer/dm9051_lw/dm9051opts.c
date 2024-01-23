//
// dm9051opts.c
//
#include "dm9051_lw.h"
#include "dm9051_lw_conf.h"
//#include "developer_conf.h" 
//#include "netconf.h"

#if HELLO_DRIVER_OPTS_DISPLAY_API
//.#include "testproc/testproc_lw.h"
#endif

#if TEST_PLAN_MODE //TestMode
u8 gfirst_log[ETHERNET_COUNT];
#endif

#if HELLO_DRIVER_OPTS_DISPLAY_API
	void GpioDisplay(void) {
	  int i;
	  for (i = 0; i < ETHERNET_COUNT; i++) {
		mstep_set_net_index(i);
		printf("@ %s, %s, %s\r\n", mstep_conf_info(), mstep_conf_cpu_spi_ethernet(), mstep_conf_cpu_cs_ethernet());
	  }
	}
	
//Other project, in LOG.c
void EepromDisplay(int pin)
{
	int i;
	printf("@ --EEPROM[%d] word", pin);
	for (i = 0; i < 9; i++) {
		uint16_t value;
		value = eeprom_read(i);
		printf("%s%04x", 
			!(i % 4) ? "  " : " ",
			value);
	}
	bannerline_log();
}
//Other project, in developer_conf.c
static uint8_t MACaddr[6] = {0x00, 0x60, 0x6e, 0x00, 0x01, 0x25};
uint8_t mac_addresse[ETHERNET_COUNT][6] = { /* [ETHERNET_COUNT] */ \
	{0, 0x60, 0x6e, 0x00, 0x01, 0x25}, \
};
//my definition
//as lwip.
/** This is the aligned version of ip4_addr_t,
   used as local variable, on the stack, etc. */
struct ip4_addr {
  uint32_t addr;
};
/* Get one byte from the 4-byte address */
#define ip4_addr_get_byte(ipaddr, idx) (((const uint8_t*)(&(ipaddr)->addr))[idx])
#define ip4_addr1(ipaddr) ip4_addr_get_byte(ipaddr, 0)
#define ip4_addr2(ipaddr) ip4_addr_get_byte(ipaddr, 1)
#define ip4_addr3(ipaddr) ip4_addr_get_byte(ipaddr, 2)
#define ip4_addr4(ipaddr) ip4_addr_get_byte(ipaddr, 3)
/* These are cast to u16_t, with the intent that they are often arguments
 * to printf using the U16_F format from cc.h. */
#define ip4_addr1_16(ipaddr) ((uint16_t)ip4_addr1(ipaddr))
#define ip4_addr2_16(ipaddr) ((uint16_t)ip4_addr2(ipaddr))
#define ip4_addr3_16(ipaddr) ((uint16_t)ip4_addr3(ipaddr))
#define ip4_addr4_16(ipaddr) ((uint16_t)ip4_addr4(ipaddr))
struct ip4_addr myip = { 
	8 << 24 | 6 << 16 | 168 << 8 | 192, //= 0x0806a8c0, 
	//(192 << 24) | (168 << 16) | (6 << 8) | 8,
};

uint8_t *lwip_get_mac_addresse1(void){
	#if 0
	//int i = mstep_get_net_index();
	uint8_t *p = &mac_addresse[mstep_get_net_index()][0];
	printf("lwip_get_mac_addresse1 -> mac_addresse[%d][0]\r\n", mstep_get_net_index());
	printf("ptr is %02x%02x%02x%02x%02x%02x, ", *p++, *p++,*p++,*p++,*p++,*p++);
	printf("array is %02x%02x%02x%02x%02x%02x\r\n", mac_addresse[mstep_get_net_index()][0],
		mac_addresse[mstep_get_net_index()][1],mac_addresse[mstep_get_net_index()][2],
		mac_addresse[mstep_get_net_index()][3],mac_addresse[mstep_get_net_index()][4],
		mac_addresse[mstep_get_net_index()][5]);
	#endif
	
	return MACaddr; //&mac_addresse[mstep_get_net_index()][0];
}

	void ethcnt_ifdiplay_chipmac(void)
	{
		int i;
		//.bannerline_log();
		for (i = 0; i< ETHERNET_COUNT; i++) {
			uint8_t buf[6];
			mstep_set_net_index(i);
		
			//part par
			cspi_read_regs(DM9051_PAR, buf, 6, csmode()); //dm9051opts_csmode_tcsmode()
			printf("@ chip-bare-mac[%d] %02x%02x%02x%02x%02x%02x\r\n", i, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
		}
	}

	void ethcnt_ifdiplay_iomode(void)
	{
		int i;
		GpioDisplay();
		ethcnt_ifdiplay_chipmac();
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("@ iomode[%d] %s / value %02x\r\n", mstep_get_net_index(), dm9051opts_desciomode(), iomode()); //dm9051opts_iomode()
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("@ csmode[%d] %s\r\n", mstep_get_net_index(), dm9051opts_desccsmode());
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("@ bmcrmode[%d] %s\r\n", i, dm9051opts_descncrmode());
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("@ rx's mode[%d] %s\r\n", i, dm9051opts_descpromismode());
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			if (isrxmode_checksum_offload())
				printf("@ checksum[%d] %s / value %02x %s\r\n", i, dm9051opts_descrxmode_checksum_offload(), //~dm9051opts_desc_rxchecksum_offload(), 
					isrxmode_checksum_offload() ? RCSSR_RCSEN : 0,
					isrxmode_checksum_offload() ? "(RCSSR_RCSEN)" : " "); //is_dm9051opts_rxmode_checksum_offload ~is_dm9051opts_rxchecksum_offload
			else
				printf("@ checksum[%d] %s\r\n", i, dm9051opts_descrxmode_checksum_offload());
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("@ fcrmode[%d] %s / value %02x\r\n", i, dm9051opts_descflowcontrolmode(), 
					isflowcontrolmode() ? FCR_DEFAULT_CONF : 0); //(_dm9051optsex[mstep_get_net_index()]._flowcontrolmode)
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			uint8_t *macaddr;
			mstep_set_net_index(i);
			macaddr = lwip_get_mac_addresse1();
			printf("@ config tobe mac[%d] %02x%02x%02x%02x%02x%02x\r\n", i, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
		}

		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			EepromDisplay(i);
		}
		
		for (i = 0; i< ETHERNET_COUNT; i++) {
			mstep_set_net_index(i);
			printf("@ ip[%d] %d.%d.%d.%d\r\n", i, //%"U16_F".%"U16_F".%"U16_F".%"U16_F"
				ip4_addr1_16(&myip),
				ip4_addr2_16(&myip),
				ip4_addr3_16(&myip),
				ip4_addr4_16(&myip)
				);
			  //ip4_addr1_16(netif_ip4_addr(&xnetif[i])), 
			  //ip4_addr2_16(netif_ip4_addr(&xnetif[i])),
			  //ip4_addr3_16(netif_ip4_addr(&xnetif[i])), 
			  //ip4_addr4_16(netif_ip4_addr(&xnetif[i])));
		}
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
#if TEST_PLAN_MODE
	int i;
	for (i = 0; i < ETHERNET_COUNT; i++)
		gfirst_log[i] = 1; //Can know the first time reset the dm9051 chip!
#endif
}

#if TEST_PLAN_MODE
u8 first_log_get(int i)
{
	u8 if_log = 
		gfirst_log[i];
	gfirst_log[i] = 0; //first_log_clear(i);
	return if_log;
}
#endif

//void first_log_clear(int i)
//{
//	gfirst_log[i] = 0;
//}
