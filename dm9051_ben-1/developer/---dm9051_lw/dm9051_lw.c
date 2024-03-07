/*
 * Copyright (c) 2023-2025 Davicom Semiconductor, Inc.
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
 * This DM9051 Driver for LWIP tcp/ip stack
 * First veryification with : AT32F415
 *
 * Author: Joseph CHANG <joseph_chang@davicom.com.tw>
 * Date: 20230411
 * Date: 20230428 (V3)
 * Date: 20240215 (Versions log please refer to 'dm9051_lw/info.txt')
 */

/* SPI master control is essential,
 * Do define spi specific definition, depend on your CPU's board support package library.
 * Here we had the definition header file with : "at32f415_spi.h"
 * for AT32F415 cpu.
 */

#include "dm9051_lw_conf.h"
#include "dm9051_lw.h"
#include "dm9051_lw_log_inc.h"

static void dm9051_phycore_on(uint16_t nms);
static void dm9051_core_reset(void);
static void dm9051_set_par(const uint8_t *macadd);
static void dm9051_set_mar(void);
static void dm9051_set_recv(void);

#if 0 //[Since lwip_rx_hdlr()/lwip_rx_loop_handler() can do check the link-state in advance, so this logic can then discard!]
static u8 lw_flag[ETHERNET_COUNT];
static u16 unlink_count[ETHERNET_COUNT], unlink_stamp[ETHERNET_COUNT];
#endif

//#define UNLINK_COUNT_RESET	60000

void dm9051_poweron_rst(void)
{
	DM9051_Poweron_Reset();
}

int check_chip_id(uint16_t id) {
	int res = (id == (DM9051_ID >> 16)
			|| id == (DM9052_ID >> 16)
	#if 1
	//_CH390
			|| id == (0x91511c00 >> 16)
			|| id == (0x91001c00 >> 16) /* Test CH390 */
	#endif
		) ? 1 : 0;
	
	if (id == (0x91001c00 >> 16)) {
		printf("---------------------- \r\n");
		printf("--- warn case %04x --- \r\n", id);
		printf("---------------------- \r\n");
	}
		
	return res;
}

static uint16_t read_chip_id(void) {
	u8 buff[2];
	cspi_read_regs(DM9051_PIDL, buff, 2, CS_EACH);
	return buff[0] | buff[1] << 8;
}

static void read_chip_revision(u8 *ids, u8 *rev_ad) {
	cspi_read_regs(DM9051_VIDL, ids, 5, OPT_CS(csmode)); //dm9051opts_csmode_tcsmode()
	cspi_read_regs(0x5C, rev_ad, 1, OPT_CS(csmode)); //dm9051opts_csmode_tcsmode()
}

uint16_t eeprom_read(uint16_t wordnum)
{
	u16 uData;
	do {
		int w = 0;
		DM9051_Write_Reg(DM9051_EPAR, wordnum);
		DM9051_Write_Reg(DM9051_EPCR, 0x4); // chip is reading
		dm_delay_us(1);
		while(DM9051_Read_Reg(DM9051_EPCR) & 0x1) {
			dm_delay_us(1);
			if (++w >= 500) //5
				break;
		} //Wait complete
		DM9051_Write_Reg(DM9051_EPCR, 0x0);
		uData = (DM9051_Read_Reg(DM9051_EPDRH) << 8) | DM9051_Read_Reg(DM9051_EPDRL);
	} while(0);
	return uData;
}

static uint16_t phy_read(uint16_t uReg)
{
	int w = 0;
	u16 uData;

	#if 1
	//_CH390
	//if (uReg == PHY_STATUS_REG)
	//	dm9051_phycore_on(0);
	#endif
	
	DM9051_Write_Reg(DM9051_EPAR, DM9051_PHY | uReg);
	DM9051_Write_Reg(DM9051_EPCR, 0xc);
	dm_delay_us(1);
	while(DM9051_Read_Reg(DM9051_EPCR) & 0x1) {
		dm_delay_us(1);
		if (++w >= 500) //5
			break;
	} //Wait complete
	
	DM9051_Write_Reg(DM9051_EPCR, 0x0);
	uData = (DM9051_Read_Reg(DM9051_EPDRH) << 8) | DM9051_Read_Reg(DM9051_EPDRL);
	
	#if 0
	if (uReg == PHY_STATUS_REG) {
		if (uData  & PHY_LINKED_BIT)
			dm9051_set_flags(lw_flag, DM9051_FLAG_LINK_UP);
		else
			dm9051_clear_flags(lw_flag, DM9051_FLAG_LINK_UP);
	}
	#endif
		
	return uData;
}

//static ; function "phy_write" was declared but never referenced.
void phy_write(uint16_t reg, uint16_t value)
{
	int w = 0;

	DM9051_Write_Reg(DM9051_EPAR, DM9051_PHY | reg);
	DM9051_Write_Reg(DM9051_EPDRL, (value & 0xff));
	DM9051_Write_Reg(DM9051_EPDRH, ((value >> 8) & 0xff));
	/* Issue phyxcer write command */
	DM9051_Write_Reg(DM9051_EPCR, 0xa);
	dm_delay_us(1);
	while(DM9051_Read_Reg(DM9051_EPCR) & 0x1){
		dm_delay_us(1);
		if (++w >= 500) //5
			break;
	} //Wait complete

	DM9051_Write_Reg(DM9051_EPCR, 0x0);    
}

void test_plan_mbndry(void)
{
	uint8_t isr0, isr1, mbndry0, mbndry1;
	char *str0, *str1;

	isr0 = DM9051_Read_Reg(DM9051_ISR);

	mbndry0 = OPT_U8(iomode);
	str0 = (mbndry0 & MBNDRY_BYTE) ? "(8-bit mode)" : "(16-bit mode)";
	printf("  ACTION: Start.s Write the MBNDRY %02x %s\r\n", mbndry0, str0);

	DM9051_Write_Reg(DM9051_MBNDRY, mbndry0);

		mbndry1 = DM9051_Read_Reg(DM9051_MBNDRY);
		str1 = (mbndry1 & MBNDRY_BYTE) ? "(8-bit mode)" : "(16-bit mode)";
		if ((mbndry0 & MBNDRY_BYTE) == (mbndry1 & MBNDRY_BYTE))
			printf("  ACTION: !And.e Read check MBNDRY %02x %s\r\n", mbndry1, str1);
		else
			printf("  ACTION: !But.e Read check MBNDRY %02x %s \"( read different )\"\r\n",
				mbndry1, str1); //"(read diff, be an elder revision chip bit7 can't write)"

	isr1 = DM9051_Read_Reg(DM9051_ISR);

	printf("  RESET: ISR.read.s %02x %s\r\n", isr0, isr0 & 0x80 ? "(8-bit mode)" : "(16-bit mode)");
	printf("  RESET: ISR.read.e %02x %s\r\n", isr1, isr1 & 0x80 ? "(8-bit mode)" : "(16-bit mode)");
}

/*void test_plan_100mf(ncrmode_t ncrmode, u8 first_log)
{
	//
	// explicity, phy write bmcr, 0x3300, 
	// Since test mode may stop in 10M Half
	// We nee Full duplex for using a loopback terminator 
	// for self xmit/receive in starting test item.
	// observ, AN did take some more time to link up state
	// compare to non AN.
	//
	//if (_dm9051opts_ncrforcemode()) {
	  #if 1
		//[dm9052]
		if (ncrmode == NCR_FORCE_100MF) {
			phy_write(27, 0xe000);
			phy_write(0, 0x2100);
			if (first_log) {
				uint16_t bmcr= phy_read(0);
				printf("  RESET: REG27 write : %04x\r\n", 0xe000);
				printf("  RESET: BMCR write/read : %04x/%04x\r\n", 0x2100, bmcr);
				printf("  RESET: _core_reset [set link parameters, Force mode for 100M Full]\r\n");
			}
		}
		//[dm9051c]
		else if (ncrmode == NCR_AUTO_NEG) {
			phy_write(0, 0x3300);
			if (first_log) {
				uint16_t bmcr= phy_read(0);
				printf("  RESET: BMCR write/read : %04x/%04x\r\n", 0x3300, bmcr);
				printf("  RESET: _core_reset [set link parameters, A.N. for 100M Full]\r\n");
			}
		}
	  #endif
	//}
}*/

/*void test_plan_rx_checksum_enable(void)
{
	DM9051_Write_Reg(DM9051_RCSSR, RCSSR_RCSEN);
}*/

static void dm9051_delay_in_core_process(uint16_t nms, char *zhead) //finally, dm9051_lw.c
{
	printf(": dm9051_driver[%d] ::: %s delay %u ms.. : \r\n", mstep_get_net_index(), zhead, nms);
	if (nms)
	  dm_delay_ms(nms); //_delay_ms(nms); //from James' advice! to be determined with a reproduced test cases!!
}

static void dm9051_phycore_on(uint16_t nms) {
	  DM9051_Write_Reg(DM9051_GPR, 0x00);		//Power on PHY
	  dm9051_delay_in_core_process(nms, "_phycore_on<>");
}

static void dm9051_ncr_reset(uint16_t nms) {
	  DM9051_Write_Reg(DM9051_NCR, DM9051_REG_RESET); //iow(DM9051_NCR, NCR_RST);
	  dm9051_delay_in_core_process(nms, "_core_reset<>"); //dm_delay_ms(250); //CH390-Est-Extra
}

static void dm9051_core_reset(void)
{
	if (OPT_CONFIRM(generic_core_rst)){
		
		#if 0
		printf("-----------------------------------------------------------------------------------------\r\n");
		printf("--------------------- write a long delay type procedure ,for core reset -----------------\r\n");
		printf("-----------------------------------------------------------------------------------------\r\n");
		#endif

		//CH390
		  //printf("  _core_reset[%d]()\r\n", mstep_get_net_index());

		  dm9051_ncr_reset(OPT_U8(hdir_x2ms)*2);
		  dm9051_phycore_on(250);
		
		  DM9051_Write_Reg(DM9051_MBNDRY, OPT_U8(iomode));
		  DM9051_Write_Reg(DM9051_PPCR, PPCR_PAUSE_COUNT); //iow(DM9051_PPCR, PPCR_SETTING); //#define PPCR_SETTING 0x08
		  DM9051_Write_Reg(DM9051_LMCR, LMCR_MODE1);
		  DM9051_Write_Reg(DM9051_INTR, INTR_ACTIVE_LOW); // interrupt active low
	} else {
		  int i = mstep_get_net_index();
		  u8 if_log = first_log_get(i); //+first_log_clear(i);

		  //u8 gpcr = DM9051_Read_Reg(DM9051_GPCR);
		  //DM9051_Write_Reg(DM9051_GPCR, gpcr | 0x01); //GPC0(reserved), bit-0

		  #if 0
		  dm9051_clear_flags(lw_flag, DM9051_FLAG_LINK_UP);
		  unlink_count = unlink_stamp = 0;
		  #endif
		
	#if 0
		//DAVICOM
		  printf("  _core_reset[%d]()\r\n", i);
		  dm9051_phycore_on(5);

		  DM9051_Write_Reg(DM9051_NCR, DM9051_REG_RESET); //iow(DM9051_NCR, NCR_RST);
		  dm_delay_ms(0);
	#else
		//CH390
		  printf("  _core_reset[%d]()\r\n", i);
		  DM9051_Write_Reg(DM9051_NCR, DM9051_REG_RESET); //iow(DM9051_NCR, NCR_RST);
		  dm_delay_ms(2); //CH390-Est-Extra
		  
		  dm9051_phycore_on(5);
	#endif

		  do {
			if (if_log) {
			  test_plan_mbndry();
			} else
			  DM9051_Write_Reg(DM9051_MBNDRY, OPT_U8(iomode));
		  } while(0);

		  DM9051_Write_Reg(DM9051_PPCR, PPCR_PAUSE_COUNT); //iow(DM9051_PPCR, PPCR_SETTING); //#define PPCR_SETTING 0x08
		  DM9051_Write_Reg(DM9051_LMCR, LMCR_MODE1);
		  #if 1
		  DM9051_Write_Reg(DM9051_INTR, INTR_ACTIVE_LOW); // interrupt active low
		  #endif
		
		#if TEST_PLAN_MODE
		if (OPT_CONFIRM(rxmode_checksum_offload))
			DM9051_Write_Reg(DM9051_RCSSR, RCSSR_RCSEN);
		#else
		/* if (isrxmode_checksum_offload())
				DM9051_Write_Reg(DM9051_RCSSR, RCSSR_RCSEN); */
		#endif

		/* flow_control_config_init */
		if (OPT_CONFIRM(flowcontrolmode)) {
		  #if TEST_PLAN_MODE == 0
			printf("  core_reset: %s, fcr value %02x\r\n", dm9051opts_descflowcontrolmode(), FCR_DEFAULT_CONF);
		  #endif
			DM9051_Write_Reg(DM9051_FCR, FCR_DEFAULT_CONF);
		}

		#if TEST_PLAN_MODE
		test_plan_100mf(OPT_NCR(ncrmode), if_log); //= dm9051opts_ncrmode_tncrmode()
		#endif
	}
}

static void dm9051_show_rxbstatistic(u8 *htc, int n)
{
	int i, j;
	
	//dm9051_show_id();
	printf("SHW rxbStatistic, 254 wrngs\r\n");
	for (i = 0 ; i < (n+2); i++) {
		if (!(i%32) && i) printf("\r\n");
		if (!(i%32) || !(i%16)) printf("%02x:", i);
		if (!(i%8)) printf(" ");
		if (i==0 || i==1) {
			printf("  ");
			continue;
		}
		j = i - 2;
		printf("%d ", htc[j]);
	}
	printf("\r\n");
}

void dm9051_mac_adr(const uint8_t *macadd)
{
	dm9051_set_par(macadd);
	//show_par();
}
void dm9051_rx_mode(void)
{	
	dm9051_set_mar();
	//show_mar();
	
	dm9051_set_recv();
}

/*void dm9051_rx_mode_subprocess(void) {
	u8 buff[8];
	cspi_read_regs(DM9051_MAR, buff, 8, CS_EACH);
	printf("dm9051_rx_mode_subprocess MAR %02x %02x %02x %02x  %02x %02x %02x %02x\r\n",
			buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7]);
	cspi_read_regs(DM9051_PAR, buff, 6, CS_EACH);
	printf("dm9051_rx_mode_subprocess PAR %02x %02x %02x %02x %02x %02x\r\n",
			buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]);
	dm9051_set_recv();
}*/

static void dm9051_set_par(const u8 *macadd)
{
	int i;
	for (i=0; i<6; i++)
		DM9051_Write_Reg(DM9051_PAR+i, macadd[i]);
}
static void dm9051_set_mar(void)
{
	int i;
	for (i=0; i<8; i++)
		DM9051_Write_Reg(DM9051_MAR+i, (i == 7) ? 0x80 : 0x00);
}

static void dm9051_set_recv(void)
{
	#if 0
	DM9051_Write_Reg(_DM9051_FCR, _FCR_DEFAULT_CONF); Located in 'dm9051 core reset'!
	phy_write _04, _flow
	#endif

	DM9051_Write_Reg(DM9051_IMR, IMR_PAR | IMR_PRM); //(IMR_PAR | IMR_PTM | IMR_PRM);
	
	//#if _TEST_PLAN_MODE //#else //#endif
	if (OPT_U8(promismode)) {
		printf("SETRECV: ::: test rx_promiscous (write_reg, 0x05, (1 << 2))\r\n");
		DM9051_Write_Reg(DM9051_RCR, RCR_DEFAULT | RCR_PRMSC | RCR_RXEN); //promiscous
	}
	else
		DM9051_Write_Reg(DM9051_RCR, RCR_DEFAULT | RCR_RXEN); //dm9051_fifo_RX_enable();
}

char *display_mac_bannerline_defaultN = ": Display Bare device";

char *display_identity_bannerline_title = NULL;
char *display_identity_bannerline_default =  ": Read device";

uint16_t psh_id[ETHERNET_COUNT];
uint8_t psh_ids[ETHERNET_COUNT][5], psh_id_adv[ETHERNET_COUNT];

static int display_identity(char *spiname, uint16_t id, uint8_t *ids, uint8_t id_adv) {
	if (ids) {
		psh_id[mstep_get_net_index()] = id;
		memcpy(&psh_ids[mstep_get_net_index()][0], ids, 5);
		psh_id_adv[mstep_get_net_index()] = id_adv;
	} else {
		 id = psh_id[mstep_get_net_index()];
		 memcpy(ids, &psh_ids[mstep_get_net_index()][0], 5);
		 id_adv = psh_id_adv[mstep_get_net_index()];
	}

	printf("%s[%d] ::: ids %02x %02x %02x %02x %02x (%s) chip rev %02x, Chip ID %02x (CS_EACH_MODE)%s\r\n",
		display_identity_bannerline_title ? display_identity_bannerline_title : display_identity_bannerline_default,
		mstep_get_net_index(), ids[0], ids[1], ids[2], ids[3], ids[4], dm9051opts_desccsmode(), id_adv, id,
		ids ? "" : ".(Rst.process)");
	return 0;
}

static void display_mac_action(char *head, const uint8_t *adr) {
	display_mac_bannerline_defaultN = head; // ": rd-bare device";/ ": wr-bare device";
	printf("%s[%d] ::: chip-mac %02x%02x%02x%02x%02x%02x\r\n",
				display_mac_bannerline_defaultN ? display_identity_bannerline_title : display_mac_bannerline_defaultN,
				mstep_get_net_index(), adr[0], adr[1], adr[2], adr[3], adr[4], adr[5]);
}

static void display_baremac(void) {
	uint8_t buf[6];

	cspi_read_regs(DM9051_PAR, buf, 6, OPT_CS(csmode)); //dm9051opts_csmode_tcsmode()
	display_mac_action(": rd-bare device", buf);
}

#define	DM9051_Read_Rxb	DM9051_Read_Mem2X

#define	TIMES_TO_RST	10

u8 ret_fire_time(u8 *histc, int csize, int i, u8 rxb)
{
	printf(" _dm9051f rxb %02x (times %2d)%c\r\n", rxb, histc[i], (histc[i]==2) ? '*': ' ');
	if (histc[i] >= TIMES_TO_RST) {
		dm9051_show_rxbstatistic(histc, csize);
		histc[i] = 1;
		return TIMES_TO_RST;
	}
	return 0;
}

void hdlr_reset_process(enable_t cfgrcv)
{
	//printf(".(Rst.process[%d])\r\n", mstep_get_net_index());
	display_identity(mstep_spi_conf_name(), 0, NULL, 0);

	dm9051_core_reset(); //As: printf("rstc %d ,because rxb %02x (is %d times)\r\n", rstc, rxbyte, times);

  #if 0
	do { //[want more]
		//void tcpip_set_mac_address(void); //#include "netconf.h" (- For power on/off DM9051 demo-board, can restored working, Keep Lwip all no change.)
		//tcpip_set_mac_address(); //more for if user power-off the DM9051 chip, so instead of dm9051_set_recv();
		.void lwip_get_mac(char *adr);
		char macadd[6];
		.lwip_get_mac(macadd);
		.dm9051_mac_adr(macadd);
	} while(0);
  #endif

  #if 0
	/*dm9051_rx_mode_subprocess(); //of _dm9051_set_recv(); //of _dm9051_rx_mode();
	//dm9051_link_show();//Add: show
	*/
  #else

    #if 0 //or [A].AS In _dm9051_init_setup(), no call-
	//=dm9051_start(adr);
	dm9051_mac_adr(adr);
	dm9051_rx_mode();
    #endif
    #if 1 //or [B].AS dm9051_init's whole operations. Only for _CH390

	if (cfgrcv) {
		//CH390
		exint_menable(NVIC_PRIORITY_GROUP_0); //dm9051_board_irq_enable(); 
		dm9051_start(mstep_eth_mac());
	}
    #endif
  #endif
}

static u16 err_hdlr(char *errstr, u32 valuecode, u8 zerochk)
{
	if (zerochk && valuecode == 0)
		return 0; //.printf(": NoError as %u\r\n", valuecode);
		
	printf(errstr, valuecode); //or "0x%02x"

	hdlr_reset_process(OPT_CONFIRM(hdlr_confrecv));
	return 0;
}

static u16 ev_rxb(uint8_t rxb)
{
	int i;
	static u8 histc[254] = { 0 }; //static int rff_c = 0 ...;
	u8 times = 1;
	
	for (i = 0 ; i < sizeof(histc); i++) {
		if (rxb == (i+2)) {
			histc[i]++;
			times = ret_fire_time(histc, sizeof(histc), i, rxb);
			err_hdlr("_dm9051f rxb error times : %u\r\n", times, 1); //As: Hdlr (times : TIMES_TO_RST or 0)
		}
	}
	return err_hdlr("_dm9051f rxb error times : %u\r\n", times, 0); //As: Hdlr (times : 1)
}

static u16 ev_status(uint8_t rx_status)
{
	bannerline_log();
	printf(".(Err.status%2x) _dm9051f:", rx_status);
	if (rx_status & RSR_RF) printf(" runt-frame");
	
	if (rx_status & RSR_LCS) printf(" late-collision");
	if (rx_status & RSR_RWTO) printf(" watchdog-timeout");
	if (rx_status & RSR_PLE) printf(" physical-layer-err");
	if (rx_status & RSR_AE) printf(" alignment-err");
	if (rx_status & RSR_CE) printf(" crc-err");
	if (rx_status & RSR_FOE) printf(" rx-memory-overflow-err");
	bannerline_log();
	return err_hdlr("_dm9051f rx_status error : 0x%02x\r\n", rx_status, 0);
}

/* if "expression" is true, then execute "handler" expression */
#define DM9051_RX_RERXB(expression, handler) do { if ((expression)) { \
  handler;}} while(0)
#define DM9051_RX_BREAK(expression, handler) do { if ((expression)) { \
  handler;}} while(0)
#define DM9051_TX_DELAY(expression, handler) do { if ((expression)) { \
  handler;}} while(0)

u8 checksum_re_rxb(u8 rxbyte) {
	if (OPT_CONFIRM(rxmode_checksum_offload))
		rxbyte &= 0x03;
	return rxbyte;
}
uint16_t dm9051_rx_dump(uint8_t *buff)
{
	// Because we encounter 16-bit mode fail, so went to try above!
	// Since target dm9051a IS only 8-bit mode (working)
	u8 rxbyte, rx_status;
	u8 ReceiveData[4];
	u16 rx_len;
	rxbyte = DM9051_Read_Rxb();
	rxbyte = checksum_re_rxb(rxbyte); //This is because checksum-offload enable

	DM9051_RX_BREAK((rxbyte != 0x01 && rxbyte != 0), return ev_rxb(rxbyte));
	DM9051_RX_BREAK((rxbyte == 0), return 0);
		
	DM9051_Read_Mem(ReceiveData, 4);
	DM9051_Write_Reg(DM9051_ISR, 0x80);
	
	rx_status = ReceiveData[1];
	rx_len = ReceiveData[2] + (ReceiveData[3] << 8);
	
	DM9051_RX_BREAK((rx_status & 0xbf), return ev_status(rx_status)); //_err_hdlr("_dm9051f rx_status error : 0x%02x\r\n", rx_status, 0)
	DM9051_RX_BREAK((rx_len > RX_POOL_BUFSIZE), return err_hdlr("_dm9051f rx_len error : %u\r\n", rx_len, 0));

	DM9051_Read_Mem(buff, rx_len);
	DM9051_Write_Reg(DM9051_ISR, 0x80);
	return rx_len;
}

 #if TEST_PLAN_MODE
 //.#include "test/test_state.h"
 #define check_get()				1
 #define check_get_check_done()	0
 #define	check_decr_to_done()
 #define	check_set_done()
 #define	check_set_new()

 //#ifndef tp_all_done
 #define tp_all_done()	1
 //#endif

 //#ifndef rxrp_dump_print_init_show
 static void rxrp_dump_print_init_show(void) {
	uint16_t rxrp;
	rxrp = cspi_read_reg(DM9051_MRRL);
	rxrp |= cspi_read_reg(DM9051_MRRH) << 8;
	printf(" %4x", rxrp);
 }
 //#endif
 #endif

uint16_t dm9051_rx(uint8_t *buff)
{
	u8 rxbyte, rx_status;
	u8 ReceiveData[4];
	u16 rx_len;
	
	#if TEST_PLAN_MODE
	u8 rxbyte_flg;
	int ipf = 0, udpf = 0, tcpf = 0;
	#endif
	
	#if 0
	/*if (!dm9051_is_flag_set(lw_flag, DM9051_FLAG_LINK_UP)) {
		if (unlink_count < UNLINK_COUNT_RESET) {
			unlink_count++;
		}
		if (unlink_count >= unlink_stamp) {*/
			#if 0
			/*uint16_t bmsr = dm9051_bmsr_update();
			printf("dm9051rx dm9051[%d](while bmsr %04x %s) step counter %04u\r\n", 
				mstep_get_net_index(),
				bmsr, !dm9051_is_flag_set(lw_flag, DM9051_FLAG_LINK_UP) ? "PHY DOWN" : "PHY UP",
				unlink_stamp);*/
			#endif
			/*unlink_stamp = unlink_count + 1000;
		}
		return 0;
	}*/
	#endif
	
	rxbyte = DM9051_Read_Rxb(); //DM9051_Read_Reg(DM9051_MRCMDX);
	//DM9051_RXB_Basic(rxbyte); //(todo) Need sevice case.

	#if TEST_PLAN_MODE //TestMode.RX
	if (check_get() && !check_get_check_done()) { //(checkrxbcount > 1)
		if (rxbyte & RXB_ERR) { //(checksum_re_rxb(rxbyte) != 0x01 && checksum_re_rxb(rxbyte) != 0)
				check_decr_to_done();
				bannerline_log();
				printf(".(Err.rxb%2x)", rxbyte);
				if (check_get_check_done()) {
					printf("\r\n");
					printf("---------------------- on (Err.rxb == %2x) ,check_get_check_done() ----------------------\r\n", rxbyte);
					printf("-------------------------- DM9051_RX_RERXB will process follow -------------------------\r\n");
				}
		}
		else {
			if (rxbyte == 0) {

			  check_decr_to_done(); //need ?
			#if 0
			  printf(".(rxb%2x)", rxbyte);
			#endif
			  if (check_get_check_done()) {
				bannerline_log();
				printf("-------------------------- (rxb == 0) ,check_get_check_done() --------------------------\r\n");
				printf("-------------------------- On ChecksumOffload, DM9051_RX_RERXB will process follow -------------------------\r\n");
			  }
			}
			//else
			
			/*if (checksum_re_rxb(rxbyte) == DM9051_PKT_RDY)
			{
				//.printf("[rx %d]\r\n", get_testing_rx_count()); //testing_rx_count
				if (!tp_all_done()) {
					printf("[rx %d]\r\n", get_testing_rx_count());
					display_state();
				}
			}*/
			
			if (rxbyte != 0x01 && rxbyte != 0) {
				if (rxbyte == 0x40)
					;
				else {
					//Normal- case
					//printf(".ReceivePacket: rxb=%2x (processed) to %02x\r\n", rxbyte, rxbyte & 0x03);

					#if 1 //Extra- work around
					if (rxbyte & RCSSR_TCPS) {
						if (!(rxbyte & RCSSR_TCPP)) {
							rxbyte &= ~RCSSR_TCPS;
							bannerline_log();
							printf("Warn.case.ReceivePacket: rxb=%2x (step1) to %02x, mdra_ingress", rxbyte, rxbyte);
							rxrp_dump_print_init_show();
							bannerline_log();
						}
					}
					#endif
				}
								
				if (get_testplanlog(test_plan_log)) { //get_dm9051opts_testplanlog()
					if ((rxbyte & RCSSR_IPP)) {
						ipf = 1;
						if (!tp_all_done())
							printf("DM9RX: found IP, checksum %s\r\n", (rxbyte & RCSSR_IPS) ? "fail" : "ok");
					}
					if ((rxbyte & RCSSR_UDPP)) {
						udpf = 1;
						if (!tp_all_done())
							printf("DM9RX: found UDP, checksum %s\r\n", (rxbyte & RCSSR_UDPS) ? "fail" : "ok");
					}
					if ((rxbyte & RCSSR_TCPP)) {
						tcpf = 1;
						if (!tp_all_done())
							printf("DM9RX: found TCP, checksum %s\r\n", (rxbyte & RCSSR_TCPS) ? "fail" : "ok");
					}
				}
				rxbyte = checksum_re_rxb(rxbyte);
			}

			//.if (rxbyte != 0x01 && rxbyte != 0) {
			//.} else 
			if (rxbyte == 1) {
			 #if 1
			  check_set_done(); //checkrxbcount = 1;
			  check_set_new(); //SET NEW AGAIN, we do continue testing...
			 #endif
			  //POST output, printf(".rxb%2x\r\n", rxbyte);
			}
		}
	}
	else
		DM9051_RX_RERXB((rxbyte != 0x01 && rxbyte != 0), rxbyte = checksum_re_rxb(rxbyte));
	#endif
	
	DM9051_RX_BREAK((rxbyte != 0x01 && rxbyte != 0), return ev_rxb(rxbyte));
	DM9051_RX_BREAK((rxbyte == 0), return 0);
		
	DM9051_Read_Mem(ReceiveData, 4);
	DM9051_Write_Reg(DM9051_ISR, 0x80);
	
	rx_status = ReceiveData[1];
	rx_len = ReceiveData[2] + (ReceiveData[3] << 8);
	
  //printf(" :drv.rx %02x %02x %02x %02x (len %u)\r\n",
  //		ReceiveData[0], ReceiveData[1], ReceiveData[2], ReceiveData[3], rx_len);
	
	#if 0
	if (rx_len != LEN64)
		printf(":drv.rx %02x %02x %02x %02x (len %u)\r\n",
			ReceiveData[0], ReceiveData[1], ReceiveData[2], ReceiveData[3], rx_len);
	#endif
	
	#if 0
	do {
		static int dump_odd_c = 0;
		if (rx_len & 1) {
		  printf("\r\n+ drv +oddfound %d (len %d)\r\n", ++dump_odd_c, rx_len);
		}
	} while(0);
	#endif
	
	DM9051_RX_BREAK((rx_status & 0xbf), return ev_status(rx_status)); //_err_hdlr("_dm9051f rx_status error : 0x%02x\r\n", rx_status, 0)
	DM9051_RX_BREAK((rx_len > RX_POOL_BUFSIZE), return err_hdlr("_dm9051f rx_len error : %u\r\n", rx_len, 0));

	DM9051_Read_Mem(buff, rx_len);
	DM9051_Write_Reg(DM9051_ISR, 0x80);
	
	if (!OPT_CONFIRM(tx_endbit))
		dm_delay_ms(1);

	#if TEST_PLAN_MODE
	if (get_testplanlog(test_plan_log)) { //get_dm9051opts_testplanlog()
		if (ipf && !tp_all_done()) 
			printf("DM9RX: IP, checksum %02x %02x\r\n", buff[24], buff[25]);
		if (udpf && !tp_all_done())
			printf("DM9RX: UDP, checksum %02x %02x\r\n", buff[40], buff[41]);
		if (tcpf && !tp_all_done())
			printf("DM9RX: TCP, checksum %02x %02x\r\n", buff[50], buff[51]); //TBD

		rxbyte_flg = ReceiveData[0] & ~(0x03);

		if (!tp_all_done())
			printf("DM9RX[%d]: drv.rx %02x %02x %02x %02x (len %u), rxb %02x | %x\r\n",
				mstep_get_net_index(), ReceiveData[0], ReceiveData[1], ReceiveData[2], ReceiveData[3], rx_len, rxbyte_flg, rxbyte);
	}
	#endif
	
	/* An assurence */
	if (debug_rx(buff, rx_len)) { //ok. only 1st-pbuf
		debug_rx_inc_count();
		hdlr_reset_process(OPT_CONFIRM(hdlr_confrecv)); //~return ev_rxb(rxbyte);
		return 0;
	}
	return rx_len;
}

void dm9051_tx(uint8_t *buf, uint16_t len)
{
	DM9051_Write_Reg(DM9051_TXPLL, len & 0xff);
	DM9051_Write_Reg(DM9051_TXPLH, (len >> 8) & 0xff);
	DM9051_Write_Mem(buf, len);
	DM9051_Write_Reg(DM9051_TCR, TCR_TXREQ); /* Cleared after TX complete */
	
	if (OPT_CONFIRM(tx_endbit))
		DM9051_TX_DELAY((DM9051_Read_Reg(DM9051_TCR) & TCR_TXREQ), dm_delay_us(5));
	else
		dm_delay_ms(1);
}

uint16_t dm9051_init_setup(const uint8_t *adr)
{
	uint16_t id;
	uint8_t ids[5], id_adv;

	id = read_chip_id();
	read_chip_revision(ids, &id_adv);
	
	display_identity(mstep_spi_conf_name(), id, ids, id_adv);
	if (!check_chip_id(id))
		return id;
	
	return id;
}

//x void dm9051_board_irq_enable(void) //void net_irq_enable(void)
//x {
//x 	exint_menable(NVIC_PRIORITY_GROUP_0); //if (exint_conf_mptr()) _exint_enable(_exint_conf_ptr(), NVIC_PRIORITY_GROUP_0);
//x }

void dm9051_start(const uint8_t *adr)
{	
	display_baremac();
	display_mac_action(": wr-bare device", adr);

	dm9051_mac_adr(adr);
	dm9051_rx_mode();
}

uint16_t dm9051_init(const uint8_t *adr)
{
	uint16_t id;

	first_log_init();
	printf("%s, device[%d] %s, %s, to set mac/ %02x%02x%02x%02x%02x%02x\r\n",
			mstep_conf_info(),
			mstep_get_net_index(),
			mstep_conf_cpu_spi_ethernet(),
			mstep_conf_cpu_cs_ethernet(),
			adr[0],
			adr[1],
			adr[2],
			adr[3],
			adr[4],
			adr[5]);
	id = dm9051_init_setup(adr);
	if (!check_chip_id(id))
		return id;

	/*=	printf(".(Rst.setup[%d])\r\n", mstep_get_net_index());
		dm9051_core_reset();

	#if 0
		//=dm9051_start(adr);
		dm9051_mac_adr(adr);
		dm9051_rx_mode();
	#endif
		exint_menable(NVIC_PRIORITY_GROUP_0); //dm9051_board_irq_enable(); 
		dm9051_start(adr);*/

	hdlr_reset_process(DM_TRUE);
	return id;
}

uint16_t dm9051_bmcr_update(void) {
  return phy_read(PHY_CONTROL_REG);
}
uint16_t dm9051_bmsr_update(void) {
  return phy_read(PHY_STATUS_REG);
}
uint16_t dm9051_link_update(void) {
  return phy_read(PHY_STATUS_REG);
}
uint16_t dm9051_phy_read(uint32_t reg) {
  return phy_read(reg);
}
void dm9051_phy_write(uint32_t reg, uint16_t value) {
  phy_write(reg, value);
}

static uint16_t bityes(uint8_t *hist) {
	uint16_t i;
	for (i = 0; i< 16; i++)
		if (hist[i])
			return 1;
	return 0;
}

static uint16_t link_show(void) {
	u8 n = 0, i, histnsr[16] = { 0, }, histlnk[16] = { 0, };
	u8 val;
	u16 lnk;
	u16 rwpa_w, mdra_ingress;

	//.dm9051_show_id(); //Also [test][test][test].init
	do {
		n++;
		for (i= 0; i< 16; i++) {
			val = DM9051_Read_Reg(DM9051_NSR);
			lnk = phy_read(PHY_STATUS_REG);
			histnsr[i] += (val & 0x40) ? 1 : 0;
			histlnk[i] += (lnk & 0x04) ? 1 : 0;
		}
	} while(n < 20 && !bityes(histnsr) && !bityes(histlnk)); // 20 times for 2 seconds
	
	rwpa_w = (uint32_t)DM9051_Read_Reg(0x24) | (uint32_t)DM9051_Read_Reg(0x25) << 8; //DM9051_RWPAL
	mdra_ingress = (uint32_t)DM9051_Read_Reg(0x74) | (uint32_t)DM9051_Read_Reg(0x75) << 8; //DM9051_MRRL;
	
	printf("(SHW timelink, 20 detects) det %d\r\n", n);
	for (i= 8; i< 16; i++)
		printf(" %s%d", (i==8) ? ".nsr " : "", histnsr[i]);
	for (i= 8; i< 16; i++)
		printf(" %s%d", (i==8) ? ".bmsr. " : "", histlnk[i]);
	printf(" (rrp %x rwp %x)\r\n", mdra_ingress, rwpa_w);
	return bityes(histnsr) && bityes(histlnk);
}

uint16_t dm9051_link_show(void)
{
	return link_show();
}
