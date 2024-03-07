//[dummy-virtual]
/*---------------------------------------------------------------------------------------------------------*/
/* SPI Setting                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
  eDRVSPI_PORT0         = 0,
  eDRVSPI_PORT1         = 1
} E_DRVSPI_PORT;

//[redefine, solution!]
#define DM9000_REG_NCR        DM9051_NCR
#define DM9000_REG_NSR        DM9051_NSR
#define DM9000_REG_TCR        DM9051_TCR
#define DM9000_REG_RCR        DM9051_RCR
#define DM9000_REG_FCR        DM9051_FCR
#define DM9000_REG_EPCR       DM9051_EPCR
#define DM9000_REG_EPAR       DM9051_EPAR
#define DM9000_REG_EPDRL      DM9051_EPDRL
#define DM9000_REG_EPDRH      DM9051_EPDRH
#define DM9000_REG_WAR        DM9051_WCR	//0x0F
#define DM9000_REG_PAR        DM9051_PAR	//0x10
#define DM9000_REG_MAR		  DM9051_MAR

#define DM9000_REG_TCSCR      DM9051_CSCR	//0x31
#define DM9000_REG_INT_CON    DM9051_INTR	//0x39
#define DM9051_REG_PPCSR      DM9051_PPCR	//0x3D

#define DM9051_REG_MRCMDX     DM9051_MRCMDX	//0x70
#define DM9051_REG_MRRL       DM9051_MRRL	//0x74
#define DM9051_REG_MRRH       DM9051_MRRH	//0x75
#define DM9051_REG_MWRL       DM9051_MWRL	//0x7A
#define DM9051_REG_MWRH       DM9051_MWRH	//0x7B
//#define DM9051_REG_TXPLL    0x7C
//#define DM9051_REG_TXPLH    0x7D
#define DM9051_REG_ISR        DM9051_ISR
#define DM9051_REG_IMR        DM9051_IMR

//[redefine, solution!]
//#undef DM9051_Read_Reg
//#undef DM9051_Write_Reg

#define DM9051_Read_Register(spiport,reg,pdata)	*pdata = cspi_read_reg(reg)
#define DM9051_Write_Register(spiport,reg,pdata)	cspi_write_reg(reg,*pdata)

#undef DM9051_Read_Mem
#undef DM9051_Write_Mem
#define DM9051_Read_Mem(port,buf,len)		cspi_read_mem(buf,len)
#define DM9051_Write_Mem(port,buf,len)		cspi_write_mem(buf,len)

void iow(UINT8 reg, UINT8 writedata)
{
}

#define etherdev_read()	dm9051_rx(SPI_Data_Array)

#define etherdev_send	dm9051_tx
//void etherdev_send(uint8_t* pu8data, uint32_t u32datalen)
//{
//}

#define DM9051_Read_Phy		dm9051_phy_read
#define DM9051_Write_Phy	dm9051_phy_write
#define dm9k_udelay			dm_delay_us

/*************************************************************************/
/*      wait end of PHY operation                                                                                  */
/*************************************************************************/
void wait_phy_end(void)
{
int loop;
unsigned long tst_ctr;
        unsigned char reg;
        //unsigned short length;
        
        uint32_t nread;

    tst_ctr=0;
    loop=1;
    while(loop)
    {
        reg = 0x0b; 
		//length=1;
        //nread = dm_read(usb_handle, reg, length, buf);
	    DM9051_Read_Register(eDRVSPI_PORT1,reg,&nread);	// DM9051_Read_Reg(eDRVSPI_PORT1, sel, &Spi_Data);
        if(nread & 0x01)
        {
            // printf("EEP status=%x\n",buf[0]);
            tst_ctr++;
            if(tst_ctr > 0x7fff)
            {
                printf("wait_phy_end: TIMEOUT!!!\r\n\r\n");
                break;
            }
        }
        else
        {
            // printf("EEP status=%x\n",buf[0]);
            loop=0;
        }
    }
}

int ether_crc(int length, unsigned char *data)
{
	int crc = -1;
	static unsigned const ethernet_polynomial = 0x04c11db7U;
	while (--length >= 0) {
		unsigned char current_octet = *data++;
		int bit;
		for (bit = 0; bit < 8; bit++, current_octet >>= 1) {
			crc = (crc << 1) ^
				((crc < 0) ^ (current_octet & 1) ?
				 ethernet_polynomial : 0);
		}
	}
	return crc;
}

void dm9k_hash_table(void)
{
	UINT8 i;
	
#ifdef Fix_Note_Address

	DEF_MAC_ADDR[0] = 0x00;
	DEF_MAC_ADDR[1] = 0x60;
	DEF_MAC_ADDR[2] = 0x6e;
	DEF_MAC_ADDR[3] = 0x90;
	DEF_MAC_ADDR[4] = 0x00;
	DEF_MAC_ADDR[5] = 0xae;
	*/
#endif
	
	/* 設置 網卡 MAC 位置，來自於 MyHardware */

//	for(i = 0; i < 6; i++)
	//	iow(DM9000_REG_PAR + i, DEF_MAC_ADDR[i]);
	iow(DM9000_REG_EPCR,0x20);  //  reload eeprom
	
	for(i = 0; i < 8; i++) 								 /* 清除 網卡多播設置 */
		iow(DM9000_REG_MAR + i, (7 == i) ? 0x80 : 0x00); /* 速設置 廣播包 設置 */
}

#define MAX_BUFFER_SIZE    1600           			// Maximum buffer Master will send
uint8_t SPI_Data_Array[MAX_BUFFER_SIZE];			//Packet or memory share buffer
uint8_t fifo_over_err;
uint32_t crc_err;

int dm9051_test_spi_eth(void);
void  Interrupt_initial_setting(void);
void  Interrupt_disable_setting(void);
