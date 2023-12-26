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
#include "dm9051_lw.h"
#include "dm9051_lw_conf.h"
#include "testproc/testproc_lw.h" //#include "testproc/dm9051_lw_testproc.h"

static void dm9051_core_reset(void);
static void dm9051_set_par(const uint8_t *macadd);
static void dm9051_set_mar(void);
static void dm9051_set_recv(void);

static u8 lw_flag;
static u16 unlink_count;

#define UNLINK_COUNT_RESET	60000

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

//void test_mem_mode_read_reg(uint8_t reg, u8 *buf, u16 len);

void dm9051_poweron_rst(void)
{
	DM9051_Poweron_Reset();
}

int check_chip_id(uint16_t id) {
	return (id == (DM9051_ID >> 16) || id == (DM9052_ID >> 16)) ? 1 : 0;
}

uint16_t read_chip_id(void) {
	uint16_t id;
#if 1
	u8 buff[2];
	cspi_read_regsS(DM9051_PIDL, buff, 2);
	id = buff[1];
	id <<= 8;
	id |= buff[0];
#else	
	do {
		u8 val;
		val = DM9051_Read_Reg(DM9051_PIDH);
		id = val << 8;
		val = DM9051_Read_Reg(DM9051_PIDL);
		id |= val;
	} while(0);
#endif
	return id;
}

void read_chip_revision(u8 *ids, u8 *rev_ad) {
#if 1
	cspi_read_regsS(DM9051_VIDL, ids, 5);
	cspi_read_regsS(0x5C, rev_ad, 1);
#else
	ids[0] = DM9051_Read_Reg(DM9051_VIDL); //(0x2C)
	ids[1] = DM9051_Read_Reg(DM9051_VIDH);
	ids[2] = DM9051_Read_Reg(DM9051_PIDL);
	ids[3] = DM9051_Read_Reg(DM9051_PIDH);
	ids[4] = DM9051_Read_Reg(DM9051_CHIPR);
	*rev_ad = DM9051_Read_Reg(0x5C); //(0x5C)
#endif
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
	
	if (uReg == PHY_STATUS_REG) {
		if (uData  & PHY_LINKED_BIT)
			dm9051_set_flags(lw_flag, DM9051_FLAG_LINK_UP);
		else
			dm9051_clear_flags(lw_flag, DM9051_FLAG_LINK_UP);
	}
		
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
		uint8_t mbndry;
		char *str;

		mbndry = MBNDRY_DEFAULT;
		str = (mbndry & MBNDRY_BYTE) ? "8-bit" : "16-bit";
		printf("RESET: Startup DM9051_MBNDRY %02x %s mode\r\n", mbndry, str);
		
		mbndry = DM9051_Read_Reg(DM9051_MBNDRY);
		str = (mbndry & MBNDRY_BYTE) ? "8-bit" : "16-bit";
		printf("RESET: ChkRead DM9051_MBNDRY %02x %s mode\r\n", mbndry, str);
	} while(0);
}

void test_plan_100mf(void)
{
	//
	// explicity, phy write bmcr, 0x3300, 
	// Since test mode may stop in 10M Half
	// We nee Full duplex for using a loopback terminator 
	// for self xmit/receive in starting test item.
	// observ, AN did take some more time to link up state
	// compare to non AN.
	//
  #if 1
	//[dm9052]
	do {
		uint16_t bmcr;
		phy_write(27, 0xe000);
		phy_write(0, 0x2100);
		bmcr= phy_read(0);
		bannerline();
		printf("RESET: REG27 write : %04x\r\n", 0xe000);
		printf("RESET: BMCR write/read : %04x/%04x\r\n", 0x2100, bmcr);
		printf("RESET: dm9051_core_reset [set link parameters, Force mode for 100M Full]\r\n");
	} while(0);
  #else
	//[dm9051c]
	do {
		uint16_t bmcr;
		phy_write(0, 0x3300);
		bmcr= phy_read(0);
		bannerline();
		printf("RESET: BMCR write/read : %04x/%04x\r\n", 0x3300, bmcr);
		printf("RESET: dm9051_core_reset [set link parameters, A.N. for 100M Full]\r\n");
	} while(0);
  #endif
}

void test_plan_rx_checksum_enable(void)
{
	//printf(": test add tx_checksum offload\r\n");
	printf("RESET: test rx_checksum offload enable (write_reg, 0x32, (1 << 1))\r\n");
	//printf(": test rx_checksum offload enable (checksumchecking RCSSR_UDPS/RCSSR_TCPS/RCSSR_IPS\r\n");
	//printf("\r\n");
	DM9051_Write_Reg(DM9051_RCSSR, RCSSR_RCSEN);
}
	
static void dm9051_core_reset(void)
{
//u8 gpcr = DM9051_Read_Reg(DM9051_GPCR);
//DM9051_Write_Reg(DM9051_GPCR, gpcr | 0x01); //bit-0
	  banner("=== [Core Reset] ===");
	
	  DM9051_Write_Reg(DM9051_GPR, 0x00);		//Power on PHY
	  dm_delay_ms(1);
	  dm9051_clear_flags(lw_flag, DM9051_FLAG_LINK_UP);
	  unlink_count = 0;

	  DM9051_Write_Reg(DM9051_NCR, DM9051_REG_RESET); //iow(DM9051_NCR, NCR_RST);
	  //printf("DM9051_MBNDRY: MBNDRY_WORD\r\n");
	  DM9051_Write_Reg(DM9051_MBNDRY, MBNDRY_DEFAULT); /* MemBound MBNDRY_WORD/MBNDRY_BYTE*/
	  DM9051_Write_Reg(DM9051_PPCR, PPCR_PAUSE_COUNT); //iow(DM9051_PPCR, PPCR_SETTING); //#define PPCR_SETTING 0x08
	  DM9051_Write_Reg(DM9051_LMCR, LMCR_MODE1);
#if 1
	  DM9051_Write_Reg(DM9051_INTR, INTR_ACTIVE_LOW); // interrupt active low
#endif
#if TEST_PLAN_MODE //TestMode
	test_plan_mbndry();
	test_plan_100mf();
#endif
#if TEST_PLAN_MODE //TestMode
	test_plan_rx_checksum_enable();
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
	DM9051_Write_Reg(DM9051_FCR, FCR_DEFAULT); //iow(DM9051_FCR, FCR_FLOW_ENABLE);
	phy_write 04, flow
#endif
	#if 1
	DM9051_Write_Reg(DM9051_IMR, IMR_PAR | IMR_PRM); //iow(DM9051_IMR, IMR_PAR | IMR_PTM | IMR_PRM);
	#if TEST_PLAN_MODE //TestPlan
	printf("SETRECV: test rx_promiscous (write_reg, 0x05, (1 << 2))\r\n");
	DM9051_Write_Reg(DM9051_RCR, RCR_DEFAULT | RCR_PRMSC | RCR_RXEN); //promiscous
	#else
	DM9051_Write_Reg(DM9051_RCR, RCR_DEFAULT | RCR_RXEN); //dm9051_fifo_RX_enable();
	#endif
	#endif
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
	dm9051_set_recv(); //of _dm9051_rx_mode();
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

/* if "expression" is true, then execute "handler" expression */
#define DM9051_RX_RERXB(expression, handler) do { if ((expression)) { \
  handler;}} while(0)
#define DM9051_RX_BREAK(expression, handler) do { if ((expression)) { \
  handler;}} while(0)
#define DM9051_TX_DELAY(expression, handler) do { if ((expression)) { \
  handler;}} while(0)

#if TEST_PLAN_MODE //TestMode.RX
u8 checksum_re_rxb(u8 rxbyte) {
	rxbyte &= 0x03;
	return rxbyte;
}
#endif

uint16_t dm9051_rx_dump(uint8_t *buff)
{
#if 0
	//.
	//. Target dm9051a IS only 8-bit mode (working), TRIED!TRIED!
	//.
	int i;
	for (i = 0; i < 15; i++)
		dm9051_rx(buff);
	
	return dm9051_rx(buff);
#endif
	
#if 1
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
	
	DM9051_RX_BREAK((rx_status & 0xbf), return err_hdlr("_dm9051f rx_status error : 0x%02x\r\n", rx_status, 0));
	DM9051_RX_BREAK((rx_len > RX_POOL_BUFSIZE), return err_hdlr("_dm9051f rx_len error : %u\r\n", rx_len, 0));

	DM9051_Read_Mem(buff, rx_len);
	DM9051_Write_Reg(DM9051_ISR, 0x80);
	return rx_len;
#endif
}

uint16_t dm9051_rx(uint8_t *buff)
{
	u8 rxbyte, rxbyte_flg, rx_status;
	u8 ReceiveData[4];
	u16 rx_len;
	int ipf = 0, udpf = 0, tcpf = 0;
	
	if (!dm9051_is_flag_set(lw_flag, DM9051_FLAG_LINK_UP)) {
		if (unlink_count < UNLINK_COUNT_RESET) {
			unlink_count++;
			//if (unlink_count < 3U) {
			//	printf("(dm9051f chklink %d..)\r\n", unlink_count);
			//}
		} 
		//else {
		//	DM9051_RX_BREAK((unlink_count >= UNLINK_COUNT_RESET), return err_hdlr("_dm9051f rx_link_down count : %u\r\n", unlink_count, 0));
		//}
		phy_read(PHY_STATUS_REG);
		return 0;
	}
	
	rxbyte = DM9051_Read_Rxb(); //DM9051_Read_Reg(DM9051_MRCMDX);
	//DM9051_RXB_Basic(rxbyte); //(todo) Need sevice case.

#if TEST_PLAN_MODE //TestMode.RX
	if (check_get() && !check_get_check_done()) { //(checkrxbcount > 1)
		if (rxbyte & RXB_ERR) { //(checksum_re_rxb(rxbyte) != 0x01 && checksum_re_rxb(rxbyte) != 0)
				check_decr_to_done();
				printf(".(Err.rxb%2x)", rxbyte);
				if (check_get_check_done()) {
					printf("\r\n");
					printf("---------------------- on (Err.rxb == %2x) ,check_get_check_done() ----------------------\r\n", rxbyte);
					printf("-------------------------- DM9051_RX_RERXB will process follow -------------------------\r\n");
				}
		}
		else {
			if (rxbyte == 0) {
			#if 1 //need ?
			  check_decr_to_done();
			#endif
			  printf(".(rxb%2x)", rxbyte);
			  if (check_get_check_done()) {
				printf("\r\n");
				printf("-------------------------- (rxb == 0) ,check_get_check_done() --------------------------\r\n");
				printf("-------------------------- On ChecksumOffload, DM9051_RX_RERXB will process follow -------------------------\r\n");
			  }
			}
			//else
			
			if (checksum_re_rxb(rxbyte) == DM9051_PKT_RDY)
				printf("[rx %d]\r\n", get_testing_rx_count()); //testing_rx_count
			
			if (rxbyte != 0x01 && rxbyte != 0) {
				if (rxbyte == 0x40)
					;
				else {
					#if 0 //Normal- case
					printf(".ReceivePacket: rxb=%2x (processed) to %02x\r\n", rxbyte, rxbyte & 0x03);
					#endif
					#if 1 //Extra- work around
					if (rxbyte & RCSSR_TCPS) {
						if (!(rxbyte & RCSSR_TCPP)) {
							rxbyte &= ~RCSSR_TCPS;
							printf(".ReceivePacket: rxb=%2x (step1) to %02x\r\n", rxbyte, rxbyte);
						}
					}
					#endif
				}
				
				if (rxbyte & RCSSR_IPP) {
					ipf = 1;
					printf("DM9RX: found IP, checksum %s\r\n", (rxbyte & RCSSR_IPS) ? "fail" : "ok");
				}
				if (rxbyte & RCSSR_UDPP) {
					udpf = 1;
					printf("DM9RX: found UDP, checksum %s\r\n", (rxbyte & RCSSR_UDPS) ? "fail" : "ok");
				}
				if (rxbyte & RCSSR_TCPP) {
					tcpf = 1;
					printf("DM9RX: found TCP, checksum %s\r\n", (rxbyte & RCSSR_TCPS) ? "fail" : "ok");
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
	
#if 0
if (rx_len != LEN64)
	printf(":drv.rx %02x %02x %02x %02x (len %u)\r\n",
		ReceiveData[0], ReceiveData[1], ReceiveData[2], ReceiveData[3], rx_len);
#endif
	
	DM9051_RX_BREAK((rx_status & 0xbf), return err_hdlr("_dm9051f rx_status error : 0x%02x\r\n", rx_status, 0));
	DM9051_RX_BREAK((rx_len > RX_POOL_BUFSIZE), return err_hdlr("_dm9051f rx_len error : %u\r\n", rx_len, 0));

	DM9051_Read_Mem(buff, rx_len);
	DM9051_Write_Reg(DM9051_ISR, 0x80);

	if (ipf) 
		printf("DM9RX: IP, checksum %02x %02x\r\n", buff[24], buff[25]);
	if (udpf)
		printf("DM9RX: UDP, checksum %02x %02x\r\n", buff[40], buff[41]);
	if (tcpf)
		printf("DM9RX: TCP, checksum %02x %02x\r\n", buff[50], buff[51]); //TBD

#if 0	
	printf("debug: check_get() %d, this means checking-rxb-count %s\r\n", check_get(), check_get_check_done() ? "done" : "NOT done" );
#endif
	rxbyte_flg = ReceiveData[0] & ~(0x03);
	printf("DM9RX: drv.rx %02x %02x %02x %02x (len %u), rxb %02x | %x\r\n",
		ReceiveData[0], ReceiveData[1], ReceiveData[2], ReceiveData[3], rx_len, rxbyte_flg, rxbyte);
	
	//if (ReceiveData[0] != RXB_RDY) 
		//printf(".rxb %x &= 03\r\n", ReceiveData[0]);
	//printf(".rxb%2x\r\n", rxbyte);
	
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

uint16_t dm9051_init_setup(const uint8_t *adr)
{
	uint8_t ids[5], id_adv;
	uint16_t id = read_chip_id();
	display_verify_chipid("dm9051_init", mstep_spi_conf_name(), id);

	read_chip_revision(ids, &id_adv);
	display_ids("dm9051_init", ids);
	display_ida("dm9051_init", id_adv);
	
	if (!check_chip_id(id))
		return id;
	
	dm9051_core_reset();
#if 0
	//=dm9051_start(adr);
	dm9051_mac_adr(adr);
	dm9051_rx_mode();
#endif
	return id;
}

void dm9051_start(const uint8_t *adr)
{
	dm9051_mac_adr(adr);
	dm9051_rx_mode();
}

uint16_t dm9051_init(const uint8_t *adr)
{
	uint16_t id = dm9051_init_setup(adr);
	
	if (!check_chip_id(id))
		return id;
	
	dm9051_board_irq_enable();
	dm9051_start(adr);
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
	u16 rwpa_w, mrr_rd;

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
	mrr_rd = (uint32_t)DM9051_Read_Reg(0x74) | (uint32_t)DM9051_Read_Reg(0x75) << 8; //DM9051_MRRL;
	
	printf("(SHW timelink, 20 detects) det %d\r\n", n);
	
	/*for (i= 0; i< 16; i++)
		printf(" %s%d", (i==0) ? "check .nsr  " : (i==8) ? ".nsr.  " : "", histnsr[i]);
	printf(" %s\r\n", (bityes(histnsr) || bityes(histlnk)) ? "up" : "down");
	
	for (i= 0; i< 16; i++)
		printf(" %s%d", (i==0) ? "check .bmsr " : (i==8) ? ".bmsr. " : "", histlnk[i]);
	
	printf(" %s (rrp %x rwp %x)\r\n", (bityes(histnsr) || bityes(histlnk)) ? "up" : "down",
		mrr_rd, rwpa_w);*/
	
	for (i= 8; i< 16; i++)
		printf(" %s%d", (i==8) ? ".nsr " : "", histnsr[i]);
	for (i= 8; i< 16; i++)
		printf(" %s%d", (i==8) ? ".bmsr. " : "", histlnk[i]);
	
	printf(" (rrp %x rwp %x)\r\n", mrr_rd, rwpa_w);
	
	return bityes(histnsr) && bityes(histlnk);
}

uint16_t dm9051_link_show(void)
{
	return link_show();
}

void display_verify_chipid(char *str, char *spiname, uint16_t id) {
	chipid_on_pin_code_log_s(spiname, str);
	if (!check_chip_id(id)) { //if (id != (DM9051_ID/DM9052_ID >> 16))
	#if NON_CHIPID_STOP == 1
		chipid_on_pin_code_log_err(spiname);
		printf("\r\n");
		printf(": end\r\n");
		printf(": while(1);\r\n");
		while(1) ;
	#endif
	}
	#if NON_CHIPID_STOP == 1
	else {
		printf("Chip ID CHECK experiment! Succeed OK!!\r\n");
		printf(": end\r\n");
		printf(": while(1);\r\n");
		while(1) ; //Feature attribute experiment!!
	}
	#endif
}
