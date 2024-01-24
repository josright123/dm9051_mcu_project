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
 */

/* SPI master control is essential,
 * Do define spi specific definition, depend on your CPU's board support package library.
 * Here we had the definition header file with : "at32f415_spi.h"
 * for AT32F415 cpu.
 */
#include "dm9051_lw_conf.h"
#include "dm9051_lw.h"
//#include "testproc/testproc_lw.h" //#include "testproc/dm9051_lw_testproc.h"
//#include "netconf.h"
//#include "testproc/testproc_lw.h"

static void dm9051_core_reset(void);
static void dm9051_set_par(const uint8_t *macadd);
static void dm9051_set_mar(void);
static void dm9051_set_recv(void);

#if 0 //[Since lwip_rx_hdlr()/lwip_rx_loop_handler() can do check the link-state in advance, so this logic can then discard!]
static u8 lw_flag[ETHERNET_COUNT];
static u16 unlink_count[ETHERNET_COUNT], unlink_stamp[ETHERNET_COUNT];
#endif

#define UNLINK_COUNT_RESET	10000 //30000 //60000

#if 0
static void DM9051_Poweron_Reset(void)
{
	cpin_poweron_reset();
}
static uint8_t DM9051_Read_Reg(uint8_t reg) //static (todo)
{	
	return cspi_read_reg(reg);
	/*spi_write_mem(u8 *buf, u16 len);
	
	union {
	uint8_t rxpad;
	uint8_t val;
	} rd;
	dm9051_if.cs_low();
	dm9051_spi_command_write(reg | OPC_REG_R);
	rd.val = dm9051_spi_dummy_read();
	dm9051_if.cs_high();
	return rd.val;*/
}
static void DM9051_Write_Reg(uint8_t reg, uint8_t val)
{
	cspi_write_reg(reg, val);
	/*dm9051_if.cs_low();
	dm9051_spi_command_write(reg | OPC_REG_W);
	dm9051_spi_command_write(val);
	dm9051_if.cs_high();*/
}
static uint8_t DM9051_Read_Mem2X(void)
{
	return cspi_read_mem2x();
	/*int i;
	union {
	uint8_t un;
	uint8_t rxb;
	} rd;
	dm9051_if.cs_low();
	dm9051_spi_command_write(DM9051_MRCMDX | OPC_REG_R);
	for(i=0; i<2; i++)
		rd.rxb = dm9051_spi_dummy_read();
	dm9051_if.cs_high();
	return rd.rxb;*/
}
static void DM9051_Read_Mem(u8 *buf, u16 len)
{
	cspi_read_mem(buf, len);
	/*int i;
	dm9051_if.cs_low();
	dm9051_spi_command_write(DM9051_MRCMD | OPC_REG_R);
	if (len & 1)
		len++;
	for(i=0; i<len; i++)
		buf[i] = dm9051_spi_dummy_read();
	dm9051_if.cs_high();*/
}
static void DM9051_Write_Mem(u8 *buf, u16 len)
{
	cspi_write_mem(buf, len);
	/*int i;
	dm9051_if.cs_low();
	dm9051_spi_command_write(DM9051_MWCMD | OPC_REG_W);
	if (len & 1)
		len++;
	for(i=0; i<len; i++)
		dm9051_spi_command_write(buf[i]);
	dm9051_if.cs_high();*/
}
#endif

void dm9051_poweron_rst(void)
{
	DM9051_Poweron_Reset();
}

int check_chip_id(uint16_t id) {
	return (id == (DM9051_ID >> 16) || id == (DM9052_ID >> 16)) ? 1 : 0;
}

uint16_t read_chip_id(void) {
	u8 buff[2];
	//uint16_t id;
	
	//if (dm9051opts_csmode())
	//u8 buff[2]; _cspi_read_regsS(DM9051_PIDL, buff, 2);
	cspi_read_regs(DM9051_PIDL, buff, 2, CS_EACH);
	return buff[0] | buff[1] << 8;
	
	/*do {
		u8 val;
		val = DM9051_Read_Reg(DM9051_PIDH);
		id = val << 8;
		val = DM9051_Read_Reg(DM9051_PIDL);
		id |= val;
	} while(0);
	return id;*/
}

void read_chip_revision(u8 *ids, u8 *rev_ad) {
	cspi_read_regs(DM9051_VIDL, ids, 5, csmode()); //dm9051opts_csmode_tcsmode()
	cspi_read_regs(0x5C, rev_ad, 1, csmode()); //dm9051opts_csmode_tcsmode()
#if 1
	/*if (dm9051opts_csmode()) {
	cspi_read_regsS(DM9051_VIDL, ids, 5);
	cspi_read_regsS(0x5C, rev_ad, 1);
	} else {
	ids[0] = DM9051_Read_Reg(DM9051_VIDL); //(0x2C)
	ids[1] = DM9051_Read_Reg(DM9051_VIDH);
	ids[2] = DM9051_Read_Reg(DM9051_PIDL);
	ids[3] = DM9051_Read_Reg(DM9051_PIDH);
	ids[4] = DM9051_Read_Reg(DM9051_CHIPR);
	*rev_ad = DM9051_Read_Reg(0x5C); //(0x5C)
	}*/
#endif
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

//static 
uint16_t phy_read(uint16_t uReg)
{
	int w = 0;
	u16 uData;
	
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
	do {
		uint8_t isr0, isr1, mbndry0, mbndry1;
		char *str0, *str1;
		//int startup, chkread;
		
		//.printf(": usage with iomode :      %s\r\n", dm9051opts_desciomode());
		
		isr0 = DM9051_Read_Reg(DM9051_ISR);
		
		mbndry0 = iomode(); //dm9051opts_iomode(), MBNDRY_DEFAULT;
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
	} while(0);
}

void test_plan_100mf(ncrmode_t ncrmode, u8 first_log)
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
				printf("  RESET: dm9051_core_reset [set link parameters, Force mode for 100M Full]\r\n");
			}
		}
		//[dm9051c]
		else if (ncrmode == NCR_AUTO_NEG) {
			phy_write(0, 0x3300);
			if (first_log) {
				uint16_t bmcr= phy_read(0);
				printf("  RESET: BMCR write/read : %04x/%04x\r\n", 0x3300, bmcr);
				printf("  RESET: dm9051_core_reset [set link parameters, A.N. for 100M Full]\r\n");
			}
		}
	  #endif
	//}
}

#if 0
void test_plan_rx_checksum_enable(void) //(u8 first_log)
{
	/*
	//printf(": test add tx_checksum offload\r\n");
	if (first_log)
		printf("  RESET: test rx_checksum offload enable (write_reg, 0x32, (1 << 1))\r\n");
	//printf(": test rx_checksum offload enable (checksumchecking RCSSR_UDPS/RCSSR_TCPS/RCSSR_IPS\r\n");
	//printf("\r\n");
	*/
	DM9051_Write_Reg(DM9051_RCSSR, RCSSR_RCSEN);
}
#endif

static void dm9051_core_reset(void)
{
	  int i = mstep_get_net_index();
	  u8 if_log = first_log_get(i); //+first_log_clear(i);
	  //#if TEST_PLAN_MODE
	  //#endif

	  //u8 gpcr = DM9051_Read_Reg(DM9051_GPCR);
	  //DM9051_Write_Reg(DM9051_GPCR, gpcr | 0x01); //bit-0
	  printf("  dm9051_core_reset[%d]()\r\n", i);
	  DM9051_Write_Reg(DM9051_GPR, 0x00);		//Power on PHY
	  dm_delay_ms(1);

	#if 0
	  dm9051_clear_flags(lw_flag, DM9051_FLAG_LINK_UP);
	  unlink_count = unlink_stamp = 0;
	#endif

	  DM9051_Write_Reg(DM9051_NCR, DM9051_REG_RESET); //iow(DM9051_NCR, NCR_RST);

	  do {
		if (if_log) {
		  test_plan_mbndry();
		} else
		  DM9051_Write_Reg(DM9051_MBNDRY, iomode());
	  } while(0);
//#if TEST_PLAN_MODE
//#else
//	  DM9051_Write_Reg(DM9051_MBNDRY, iomode());
//#endif

	  DM9051_Write_Reg(DM9051_PPCR, PPCR_PAUSE_COUNT); //iow(DM9051_PPCR, PPCR_SETTING); //#define PPCR_SETTING 0x08
	  DM9051_Write_Reg(DM9051_LMCR, LMCR_MODE1);
	  #if 1
	  DM9051_Write_Reg(DM9051_INTR, INTR_ACTIVE_LOW); // interrupt active low
	  #endif
	
#if TEST_PLAN_MODE
	if (isrxmode_checksum_offload())
		DM9051_Write_Reg(DM9051_RCSSR, RCSSR_RCSEN);
#else
	/* if (isrxmode_checksum_offload())
			DM9051_Write_Reg(DM9051_RCSSR, RCSSR_RCSEN); */
#endif

	/* flow_control_config_init */
	if (isflowcontrolmode()) {
	  #if TEST_PLAN_MODE == 0
		printf("  core_reset: %s, fcr value %02x\r\n", dm9051opts_descflowcontrolmode(), FCR_DEFAULT_CONF);
	  #endif
		DM9051_Write_Reg(DM9051_FCR, FCR_DEFAULT_CONF);
	}

#if TEST_PLAN_MODE
	test_plan_100mf(ncrmode(), if_log); //= dm9051opts_ncrmode_tncrmode()
#endif
}

static void dm9051_show_rxbstatistic(u8 *htc, int n)
{
	int i, j;
	
	//dm9051_show_id(); //Also [test][test][test].init
	printf("SHW rxbStatistic, 254 wrngs\r\n");
	for (i = 0 ; i < (n+2); i++) {
		if (!(i%32) && i) printf("\r\n");
		if (!(i%32) || !(i%16)) printf("%02x:", i);
		//if (!(i%16)) printf(" ");
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
void dm9051_rx_mode_subprocess(void)
{
	u8 buff[8];
	cspi_read_regs(DM9051_MAR, buff, 8, CS_EACH);
	printf("dm9051_rx_mode_subprocess MAR %02x %02x %02x %02x  %02x %02x %02x %02x\r\n",
			buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7]);
	cspi_read_regs(DM9051_PAR, buff, 6, CS_EACH);
	printf("dm9051_rx_mode_subprocess PAR %02x %02x %02x %02x %02x %02x\r\n",
			buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]);
	dm9051_set_recv();
}

static void dm9051_set_par(const u8 *macadd)
{
	int i;
	for (i=0; i<6; i++) {
		DM9051_Write_Reg(DM9051_PAR+i, macadd[i]);
			printf("write reg %02x = %02x\r\n", DM9051_PAR+i, macadd[i]);
	}
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

	DM9051_Write_Reg(DM9051_IMR, IMR_PAR | IMR_PRM); //iow(DM9051_IMR, IMR_PAR | IMR_PTM | IMR_PRM);
	
	//#if _TEST_PLAN_MODE //TestPlan
	if (promismode()) { //dm9051opts_promismode()
		printf("SETRECV: test rx_promiscous (write_reg, 0x05, (1 << 2))\r\n");
		DM9051_Write_Reg(DM9051_RCR, RCR_DEFAULT | RCR_PRMSC | RCR_RXEN); //promiscous
	}
	//#else
	else
		DM9051_Write_Reg(DM9051_RCR, RCR_DEFAULT | RCR_RXEN); //dm9051_fifo_RX_enable();
	//#endif
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

static void dm9051_delay_in_core_process(uint16_t nms) //finally, dm9051_lw.c
{
  #if 1
	printf("dm9051_driver setup delay %u ms..\r\n", nms);
	if (nms)
	  dm_delay_ms(nms); //_delay_ms(nms); //from James' advice! to be determined with a reproduced test cases!!
  #endif
}

void dm9051_reset_process(void)
{
	printf(".(Rst.process)\r\n");
	dm9051_core_reset(); //As: printf("rstc %d ,because rxb %02x (is %d times)\r\n", rstc, rxbyte, times);
  #if 0
	printf("dm9051_core_reset: "); dm9051_delay_in_core_process(300);
  #else
	printf("dm9051_core_reset: "); dm9051_delay_in_core_process(0);
  #endif

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

	dm9051_rx_mode_subprocess(); //dm9051_set_recv(); //of _dm9051_rx_mode();
	//dm9051_link_show(); //dm9051_show_link(); //_dm9051_show_timelinkstatistic(); //Add: show
}

static u16 err_hdlr(char *errstr, u32 valuecode, u8 zerochk)
{
	if (zerochk && valuecode == 0)
		//.printf("_dm9051f %s : NoError as %u\r\n", errstr, valuecode); //or "0x%02x"
		return 0;
		
	printf(errstr, valuecode); //or "0x%02x"

	dm9051_reset_process();
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

#if TEST_PLAN_MODE
u8 checksum_re_rxb(u8 rxbyte) {
	if (isrxmode_checksum_offload())
		rxbyte &= 0x03;
	return rxbyte;
}
#endif

#if 0
uint16_t dm9051_rx_dump(uint8_t *buff)
{
	// Because we encounter 16-bit mode fail, so went to try above!
	// Since target dm9051a IS only 8-bit mode (working)
	//
	u8 rxbyte, rx_status;
	u8 ReceiveData[4];
	u16 rx_len;
	rxbyte = DM9051_Read_Rxb();
	
#if 1
	rxbyte = checksum_re_rxb(rxbyte); //This is because checksum-offload enable
#endif

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
#endif

#if 1
#define check_get()				1
#define check_get_check_done()	0
#define	check_decr_to_done()
#define	check_set_done()
#define	check_set_new()

//.#include "test/test_state.h"

 //#ifndef tp_all_done
 #define tp_all_done()	1
 //#endif

 #if TEST_PLAN_MODE
 //#ifndef rxrp_dump_print_init_show
 static void rxrp_dump_print_init_show(void) {
	uint16_t rxrp;
	rxrp = cspi_read_reg(DM9051_MRRL);
	rxrp |= cspi_read_reg(DM9051_MRRH) << 8;
	printf(" %4x", rxrp);
 }
 //#endif
 #endif
 
#endif

uint16_t dm9051_rx(uint8_t *buff)
{
	u8 rxbyte, rxbyte_flg, rx_status;
	u8 ReceiveData[4];
	u16 rx_len;
	int ipf = 0, udpf = 0, tcpf = 0;
	
#if 0
	if (!dm9051_is_flag_set(lw_flag, DM9051_FLAG_LINK_UP)) {
		if (unlink_count < UNLINK_COUNT_RESET) {
			unlink_count++;
			//if (_unlink_count < 3U)
			//	printf("(dm9051rx chklink %d..)\r\n", _unlink_count);
		}
		
		if (unlink_count >= unlink_stamp) {
			
			#if 0
			uint16_t bmsr = dm9051_bmsr_update(); //phy_read(PHY_STATUS_REG); //re-detect the phy and _set_flags
			printf("dm9051rx dm9051[%d](while bmsr %04x %s) step counter %04u\r\n", 
				mstep_get_net_index(),
				bmsr, !dm9051_is_flag_set(lw_flag, DM9051_FLAG_LINK_UP) ? "PHY DOWN" : "PHY UP",
				unlink_stamp);
			#endif
			
			unlink_stamp = unlink_count + 1000;
		}
		
		return 0;
	}
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
								
				if (get_testplanlog()) { //get_dm9051opts_testplanlog()
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
	
	DM9051_RX_BREAK((rx_status & 0xbf), return ev_status(rx_status)); //_err_hdlr("_dm9051f rx_status error : 0x%02x\r\n", rx_status, 0)
	DM9051_RX_BREAK((rx_len > RX_POOL_BUFSIZE), return err_hdlr("_dm9051f rx_len error : %u\r\n", rx_len, 0));

	DM9051_Read_Mem(buff, rx_len);
	DM9051_Write_Reg(DM9051_ISR, 0x80);

	if (get_testplanlog()) { //get_dm9051opts_testplanlog()
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
	
	return rx_len;
}

void dm9051_tx(uint8_t *buf, uint16_t len)
{
	DM9051_Write_Reg(DM9051_TXPLL, len & 0xff);
	DM9051_Write_Reg(DM9051_TXPLH, (len >> 8) & 0xff);
	DM9051_Write_Mem(buf, len);
	DM9051_Write_Reg(DM9051_TCR, TCR_TXREQ); /* Cleared after TX complete */
	DM9051_TX_DELAY((DM9051_Read_Reg(DM9051_TCR) & TCR_TXREQ), dm_delay_us(5));
}

char *display_identity_bannerline_title = NULL;
char *display_identity_bannerline_default =  ": Read the revisions";
char *display_identity_bannerline_defaultN = ": Bare bare registers";

int display_identity(char *spiname, uint16_t id, uint8_t *ids, uint8_t id_adv)
{
	printf("%s[%d] ::: ids %02x %02x %02x %02x %02x (%s) chip rev %02x, Chip ID %02x (CS_EACH_MODE)\r\n",
		display_identity_bannerline_title ? display_identity_bannerline_title : display_identity_bannerline_default,
		mstep_get_net_index(), ids[0], ids[1], ids[2], ids[3], ids[4], dm9051opts_desccsmode(), id_adv, id);
	return 0;
}

	void display_chipmac(void)
	{
		//int i;
		//.bannerline_log();
		//for (i = 0; i< ETHERNET_COUNT; i++) {
			//mstep_set_net_index(i);
			//Read par
			uint8_t buf[6];
		
			display_identity_bannerline_defaultN = ": Read bare registers";
			cspi_read_regs(DM9051_PAR, buf, 6, csmode()); //dm9051opts_csmode_tcsmode()
			printf("%s[%d] ::: chip-mac %02x%02x%02x%02x%02x%02x\r\n",
				display_identity_bannerline_title ? display_identity_bannerline_title : display_identity_bannerline_defaultN,
				mstep_get_net_index(), buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
		//}
	}

int display_verify_chipid(char *str, char *spiname, uint16_t id) {
	if (check_chip_id(id)) { //(id == (DM9051_ID/DM9052_ID >> 16))
		chipid_on_pin_code_log_s(spiname, str, id);
	//#if NON_CHIPID_STOP == 1
	//	printf("Chip ID CHECK experiment! Succeed OK!!\r\n");
	//	while(1) ; //Feature attribute experiment!!
	//#endif
		return 1;
	} //else {
		chipid_on_pin_code_log_err(spiname, str, id);
		return 0;
	//#if NON_CHIPID_STOP == 1
	//	while(1) ;
	//#endif
	//}
}

uint16_t dm9051_init_setup(const uint8_t *adr)
{
	uint16_t id;
	uint8_t ids[5], id_adv;

  #if 1
	//bannerline_log();
	//printf(": usage with csmode :      %s\r\n", dm9051opts_desccsmode());
	//printf("  ::: usage with ncrmode :     %s\r\n", dm9051opts_descncrmode());
	//printf("  ::: usage with rx's mode :   %s\r\n", dm9051opts_descpromismode());
  #endif
	id = read_chip_id();
	read_chip_revision(ids, &id_adv);
	
	display_identity(mstep_spi_conf_name(), id, ids, id_adv);
	//display_verify_chipid("dm9051_init", mstep_spi_conf_name(), id);
	//display_ids("dm9051_init", ids);
	//display_ida("dm9051_init", id_adv);
	display_chipmac();
	
	if (!check_chip_id(id))
		return id;
	
	printf(".(Rst.setup[%d])\r\n", mstep_get_net_index());
	dm9051_core_reset();
#if 0
	//=dm9051_start(adr);
	dm9051_mac_adr(adr);
	dm9051_rx_mode();
#endif
	return id;
}

//x void dm9051_board_irq_enable(void) //void net_irq_enable(void)
//x {
//x 	exint_menable(NVIC_PRIORITY_GROUP_0); //if (exint_conf_mptr()) _exint_enable(_exint_conf_ptr(), NVIC_PRIORITY_GROUP_0);
//x }

void dm9051_start(const uint8_t *adr)
{
	display_identity_bannerline_defaultN = ": write bare registers";
	printf("%s[%d] ::: chip-mac %02x%02x%02x%02x%02x%02x\r\n",
				display_identity_bannerline_title ? display_identity_bannerline_title : display_identity_bannerline_defaultN,
				mstep_get_net_index(), adr[0], adr[1], adr[2], adr[3], adr[4], adr[5]);

	dm9051_mac_adr(adr);
	dm9051_rx_mode();
	
	display_chipmac();
}

uint16_t dm9051_init(const uint8_t *adr)
{
	uint16_t id;
	
	first_log_init();

//#if 0
//	id = dm9051_init_setup(adr);
//#else
	bannerline_log();
	printf("Read %s, %s, to set mac/ %s, %02x%02x%02x%02x%02x%02x\r\n",
			mstep_conf_info(),
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
	exint_menable(NVIC_PRIORITY_GROUP_0); //dm9051_board_irq_enable(); 
	dm9051_start(adr);
//#endif
	return id;
}

/**
  * @brief  updates the link states
  * @param  none
  * @retval link state 0: disconnect, 1: connection
  */
uint16_t dm9051_bmcr_update(void) {
  return phy_read(0);
}
uint16_t dm9051_bmsr_update(void) {
  return phy_read(PHY_STATUS_REG);
}
uint16_t dm9051_link_update(void) {
  uint16_t link_data;
  //uint16_t link_state;
	
  link_data = phy_read(PHY_STATUS_REG);
  //link_state = (link_data & PHY_LINKED_BIT) >> 2;
  //return link_state;
  return link_data;
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
	
	/*for (i= 0; i< 16; i++)
		printf(" %s%d", (i==0) ? "check .nsr  " : (i==8) ? ".nsr.  " : "", histnsr[i]);
	printf(" %s\r\n", (bityes(histnsr) || bityes(histlnk)) ? "up" : "down");
	
	for (i= 0; i< 16; i++)
		printf(" %s%d", (i==0) ? "check .bmsr " : (i==8) ? ".bmsr. " : "", histlnk[i]);
	
	printf(" %s (rrp %x rwp %x)\r\n", (bityes(histnsr) || bityes(histlnk)) ? "up" : "down",
		mdra_ingress, rwpa_w);*/
	
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
