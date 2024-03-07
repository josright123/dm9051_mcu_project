/**************************************************************************//**
 *
 * Copyright (C) 2012 Qualsilicon Technology Co., Ltd. All rights reserved.
 *  Feb24   
 *   Ver 0429  
 ******************************************************************************/
#include <stdlib.h>
#include "dm9051opts.h"
#include "dm9051_lw.h"
#include "trans_level.h"
#define bannerline()   printf("\r\n")

#define __USE_ANSI_EXAMPLE_RAND
uint8_t UART_RxBuf=0;
uint32_t rx_n, rxlen,p,y;
uint32_t host_MAC[6],local_MAC[6];
extern uint32_t crc_err;
extern uint8_t fifo_over_err;

#if 1
void sample_frame_mapping(uint8_t* frame_data, uint8_t* flag_tab, uint32_t frame_len, uint32_t frame_num)
{
	uint16_t i;
//	uint8_t flag_mask;
	unsigned int pat_base_adr,flag_base_adr;
//	unsigned char flag_data;
	//unsigned char datax, datay, dataz;
	UINT8 dataz; //datax, datay, 
	//frame_len = 0x10;
	
  //printf("in sample frame\r\n");
	//printf("length:%d\r\n",frame_len);
	
#if 0		
	if(frame_num > 1)
		{
        pat_base_adr  = frame_num-2;
		}
    	else {
        pat_base_adr = 0x2002 + frame_num;
    		}

    	if(frame_num > 3)
    	{
        flag_base_adr = 0x2001;
	flag_mask = (0x03) << ((frame_num-4)*2);
    	}
    	else
    	{
        flag_base_adr = 0x2000;
	flag_mask = (0x03) << ((frame_num-0)*2);
    	}
#endif
	//pat_base_adr = 0x0000;
	//flag_base_adr = 0x2000;
			
	//Stone add for Sample frame debug		 
	//printf("pat_base_adr=0X%x ,flag_base_adr=0X%x,  \r\n", pat_base_adr, flag_base_adr);
	//Stone add for usb_9051-1588.c sample_frame_mask()		
	printf("Clear All Memory data to 0x00\r\n");		
	for (i=0; i< 0x4000; i++)
  {
	iow(0x7a,i & 0xff);
    iow(0x7b,(i>>8) & 0xff);
    iow(0x78,0x00);
	}		
	
	printf("Test pattern 0 !\r\n");
	printf("length:%d\r\n",frame_len);
	
	pat_base_adr = 0x0000; 
  flag_base_adr = 0x2000;
  
	//Sample frame mask prepare
  for (i=0; i<frame_len; i++)
  flag_tab[i]= 0x10;
  //sample_mask[0]=0xfd; //Stone add for other sample frame pattern 11 11 11 01 
  flag_tab[frame_len-1]=0x30; 
	 
	for (i=0; i<frame_len; i++) //sample frame data patten (0~0xff)
    	frame_data[i] = i;		
	
			
//  if (frame_len == 0x10) 		//if test 16 bytes sample frame need to setup end of mask to 0x03.	
//	  flag_tab[15] = 3;		
			
	
  for(i=0;i<frame_len;i++)
    {
        // write pattern byte
        iow(0x7a,pat_base_adr & 0xff);
        iow(0x7b,(pat_base_adr>>8) & 0xff);
        iow(0x78,frame_data[i]);
	 

#if 0
        // get mask byte
        if(frame_num > 4)
	    flag_data = (flag_tab[i] & 0x03) << ((frame_num-4)*2);
        else
	    flag_data = (flag_tab[i] & 0x03) << ((frame_num-0)*2);
		
        iow(0x74,flag_base_adr & 0xff);
        iow(0x75,(flag_base_adr>>8) & 0xff);
	datax = ior(0x70);
	datax = ior(0x70);
	datay = datax & (~flag_mask); // clear mask bits
	dataz = datay | flag_data;
#endif
				dataz = flag_tab[i];
				
				
        // write mask byte
        iow(0x7a,flag_base_adr & 0xff);
        iow(0x7b,(flag_base_adr>>8) & 0xff);
        iow(0x78,dataz);
	
        if(i<10)
					printf("pat_base_adr = %x, flag_base_adr = %x + flag_tab[%d] = %x, dataz = %x, frame_data[%d] = %x \r\n",pat_base_adr, flag_base_adr, i, flag_tab[i], dataz, i, frame_data[i]);
				if (i==(frame_len-1))
          printf("pat_base_adr = %x, flag_base_adr = %x + flag_tab[%d] = %x, dataz = %x, frame_data[%d] = %x \r\n",pat_base_adr, flag_base_adr, i, flag_tab[i], dataz, i, frame_data[i]);
				   				
				
      	pat_base_adr = pat_base_adr + 4;			
	      flag_base_adr = flag_base_adr + 4;
    }
 
}

#endif 


/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
  system_clock_config();
  uart_print_init(115200);
  at32_board_init();

  bannerline();
  bannerline();
  dm9051_opts_display();
  dm9051_boards_initialize(ETHERNET_COUNT);

  bannerline();
  printf("x2web start: [BOARD_SPI COUNT] %d  /  Operating: [ETHERNET COUNT] %d\r\n", BOARD_SPI_COUNT, ETHERNET_COUNT); //(0, 0);
  bannerline();

  dm9051_test_spi_eth();
}

int dm9051_test_spi_eth(void)
{
  uint32_t g,h,i,j,k,l,tx_n,ip_len,crc32; //,tx_tmp;
  //time_t t;
//  STR_UART_T sParam;
//  int32_t i32Err;
    int r=0, sample_len;
	uint32_t sample_length;
	uint32_t sel, sel_1, pat_sel,pro_sel,mdata;
  //Stone add for SED need 802.3 test
	uint32_t packet_len;
	uint32_t Spi_Data,eerom_data;
	uint32_t  mdata_h,mdata_l,mem_loc;
	uint32_t mpktlen,min_len,max_len,delta, curlen;
	uint32_t spi_t[6];
	uint32_t  sub_sel ;
	unsigned char tabx[8]={0x00,0x04,0x02,0x06,0x01,0x05,0x03,0x07};
	uint8_t c,line_cnt,rest_cnt,m[7],hash_matrix[8][8],bit_ptr,crc6,byte_ptr; //,pat,crc_tab[8];
	uint32_t pats;
	uint8_t phy[12]={0,1,2,3,4,5,6,16,17,18,19,20};
	char *speed[4] = {"100M Full","100M Half","10M Full", "10M Half"};
	
	char *mode[2] = {"Mac Internal Loop-back","Normal"};
	char *link[2] = {"Link up", "Link fail"};
	unsigned char   wol_type, test_temp;
/* Frame (256 bytes) */
uint8_t  s1[256] = {
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, /* ........ */
0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, /* .. !"#$% *//* ........ */
0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, /* ./012345 *//* &'()*+,- */
0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, /* >?@ABCDE *//* 6789:;<= */
0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, /* NOPQRSTU *//* FGHIJKLM */
0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, /* ^_`abcde *//* VWXYZ[\] */
0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, /* nopqrstu *//* fghijklm */
0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, /* ~....... *//* vwxyz{|} */
0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, /* ........ *//* ........ */
0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, /* ........ *//* ........ */
0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, /* ........ *//* ........ */
0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, /* ........ *//* ........ */
0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, /* ........ *//* ........ */
0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, /* ........ *//* ........ */
0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, /* ........ *//* ........ */
0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};

#if 0
uint8_t  s0[64] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ........ */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ...}.... */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ........ */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ........ */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ...}.... */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ........ */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,/* ........ */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff                          /* ..0x00. */
};
uint8_t  frame_s2[60] = { /* ARP Frame (60 bytes): 192.168.1.233 ping 192.168.1.173 */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x79, 0x59, 0x95, 0xb6, 0xb2, 0x08, 0x06, 0x00, 0x01, /* Y....... */
0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0xf0, 0x79, 0x59, 0x95, 0xb6, 0xb2, 0xc0, 0xa8, 0x01, 0xad, /* Y....... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};/* 6789:;<= */


/* just care bytes: 0x0c~0x15, 0x1c~0x1f, and 0x26~0x29 as example */
uint8_t  flag_s2[60] = { /* Condition (60 bytes) */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, /* Y....... */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, /* Y....... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};

/* Frame (93 bytes) */
uint8_t s3[93] = {
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* ........ */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* .).{..E. */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* ........ */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* ........ */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* .).{..E. */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* ........ */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* ........ */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* .).{..E. */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, /* ........ */
0xAA, 0xAA, 0xAA, 0xAA, 0xAA
};

/* Frame (60 bytes) */
uint8_t s4[91] = {
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, /* .......% */
0x55, 0x55, 0x55 /* .......% */
};

/* Frame (92 bytes) */
uint8_t  s5[92] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1e, /* ........ */
0x8c, 0x85, 0x29, 0xee, 0x08, 0x00, 0x45, 0x00, /* ..)...E. */
0x00, 0x4e, 0x86, 0xcc, 0x00, 0x00, 0x80, 0x11, /* .N...... */
0x2e, 0x95, 0xc0, 0xa8, 0x01, 0xee, 0xc0, 0xa8, /* ........ */
0x02, 0xff, 0x00, 0x89, 0x00, 0x89, 0x00, 0x3a, /* .......: */
0x58, 0x4e, 0xc6, 0x6a, 0x01, 0x10, 0x00, 0x01, /* XN.j.... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x45, /* ...... E */
0x42, 0x46, 0x41, 0x46, 0x50, 0x45, 0x4e, 0x45, /* BFAFPENE */
0x48, 0x44, 0x42, 0x43, 0x41, 0x43, 0x41, 0x43, /* HDBCACAC */
0x41, 0x43, 0x41, 0x43, 0x41, 0x43, 0x41, 0x43, /* ACACACAC */
0x41, 0x43, 0x41, 0x43, 0x41, 0x41, 0x41, 0x00, /* ACACAAA. */
0x00, 0x20, 0x00, 0x01                          /* . .. */
};
#endif
	
  /* Run PLL */
//  DrvSYS_SetPLLMode(0);

  /* Set UART Pin */
//  DrvGPIO_InitFunction(E_FUNC_UART0);

  /* UART Setting */
//  sParam.u32BaudRate = 115200;
//  sParam.u8cDataBits = DRVUART_DATABITS_8;
//  sParam.u8cStopBits = DRVUART_STOPBITS_1;
//  sParam.u8cParity = DRVUART_PARITY_NONE;
//  sParam.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;

  /* Set UART Configuration */
//  DrvUART_Open(UART_PORT0, &sParam);

  /* Configure SPI1 related multi-function pins */
//  DrvGPIO_InitFunction(E_FUNC_SPI1);
  /* Configure SPI1 as a master, 8-bit transaction */
//  DrvSPI_Open(eDRVSPI_PORT1, eDRVSPI_MASTER, eDRVSPI_TYPE1, 8);
  /* Enable the automatic slave select function of SPI1 and select the slave select pin. */
  //DrvSPI_EnableAutoSS(eDRVSPI_PORT1);
//  DrvSPI_DisableAutoSS(eDRVSPI_PORT1);
  /* SPI clock rate 24MHz */
//  DrvSPI_SetClockFreq(eDRVSPI_PORT1, 4000000);

//  DrvGPIO_Open(E_PORT2, E_PIN5, E_IO_OUTPUT);

//  DrvGPIO_Open(E_PORT2, E_PIN6, E_IO_OUTPUT);

//  DrvGPIO_Open(E_PORT2, E_PIN7, E_IO_OUTPUT);

//  DrvGPIO_Open(E_PORT4, E_PIN0, E_IO_OUTPUT);

//  DrvGPIO_Open(E_PORT4, E_PIN1, E_IO_OUTPUT);

//  DrvGPIO_Open(E_PORT4, E_PIN3, E_IO_OUTPUT);

//  DrvGPIO_Open(E_PORT1, E_PIN0, E_IO_OUTPUT);

//  DrvGPIO_Open(E_PORT1, E_PIN4, E_IO_OUTPUT);

//  DrvGPIO_SetPortBits(E_PORT2, ~0xE0);

//  DrvGPIO_ClrBit(E_PORT4,E_PIN3);

//  DrvGPIO_ClrBit(E_PORT4,E_PIN1);

//  DrvGPIO_ClrBit(E_PORT4,E_PIN0);

//  DrvGPIO_SetPortBits(E_PORT1, ~0x11);

  printf("LED ON\r\n");

   for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetPortBits(E_PORT2, 0xE0);

//  DrvGPIO_SetBit(E_PORT4,E_PIN3);

//  DrvGPIO_SetBit(E_PORT4,E_PIN1);

//  DrvGPIO_SetBit(E_PORT4,E_PIN0);

//  DrvGPIO_SetPortBits(E_PORT1, 0x11);

  printf("LED OFF\r\n");

 

  for(k=0;k<1;k++)

  {

//  DrvGPIO_ClrBit(E_PORT2,E_PIN5);

     for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetBit(E_PORT2,E_PIN5);

//  DrvGPIO_ClrBit(E_PORT2,E_PIN6);

       for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetBit(E_PORT2,E_PIN6);

//  DrvGPIO_ClrBit(E_PORT2,E_PIN7);

       for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetBit(E_PORT2,E_PIN7);

//  DrvGPIO_ClrBit(E_PORT4,E_PIN0);

         for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetBit(E_PORT4,E_PIN0);

//  DrvGPIO_ClrBit(E_PORT4,E_PIN1);

         for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetBit(E_PORT4,E_PIN1);

//  DrvGPIO_ClrBit(E_PORT4,E_PIN3);

         for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetBit(E_PORT4,E_PIN3);

//  DrvGPIO_ClrBit(E_PORT1,E_PIN0);

       for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetBit(E_PORT1,E_PIN0);

//  DrvGPIO_ClrBit(E_PORT1,E_PIN4);

         for(j=0;j<10;j++)

   for(i=0;i<100000;i++);

//  DrvGPIO_SetBit(E_PORT1,E_PIN4);

                                                             }

  /* Configure SPI1 related multi-function pins */
//  DrvGPIO_InitFunction(E_FUNC_SPI1);
  /* Configure SPI1 as a slave, 16-bit transaction */
//  DrvSPI_Open(eDRVSPI_PORT1, eDRVSPI_SLAVE, eDRVSPI_TYPE1, 16);
/*
	do
	{
		DrvSYS_Delay(500);
		DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_VID_L, &Spi_Data);
		printf("VID_L = %x.\r\n", (unsigned int)Spi_Data);
	}while(0x46 != Spi_Data);
*/
	
//	etherdev_init();			//Initial DM9051
	
			sel = 0x10;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,& local_MAC[0]);
			sel = 0x11;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&local_MAC[1]);
			sel = 0x12;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&local_MAC[2]);
			sel = 0x13;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&local_MAC[3]);
			sel = 0x14;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&local_MAC[4]);
			sel = 0x15;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&local_MAC[5]);
  				//k= 0;
			rx_n = 0;
			/* software reset  */
			Spi_Data = 0x01;
      		       DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			 wait_phy_end();
			/*  rx all packet */
			Spi_Data = 0x0f;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_RCR, &Spi_Data);
			/* enable flow control */
			Spi_Data = 0x31;			
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_FCR,&Spi_Data);
			Spi_Data = 0x00;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_PPCSR,&Spi_Data);
			Spi_Data = 0x20;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_TCR,&Spi_Data);
		    /* enable pkt rx interrupt */
			/*DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data);			
			printf("read rx reg:0x7F=%x\r\n",Spi_Data);
			Spi_Data = 0x81;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);

			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data);
			printf("rx reg:0x7F :%x\r\n",Spi_Data);		   */
			Interrupt_initial_setting();
			fifo_over_err = 0;
			crc_err = 0;
				
				 	
/*					
		   while(1)
		   {
		   	              UART_RxBuf=0;
					 DrvUART_Read(UART_PORT0,&UART_RxBuf, 1);
					 		
					if(UART_RxBuf==27)
					{
					   printf("Exit RX only test item!!\r\n");  
					   UART_RxBuf=0;
					   sel=0;
					   Interrupt_disable_setting();	
					   printf("Number of pkt %ld received\r\n",rx_n);
			                 printf("FIFO Overflow Error:%d, CRC Error:%d\r\n",fifo_over_err,crc_err);				   
					   break;
					}
			}
*/
printf("size of array s1:%d\r\n", sizeof(s1));
#if 0
printf("size of array s0:%d\r\n", sizeof(s0));
printf("size of array fram_s2:%d\r\n", sizeof(frame_s2));
printf("size of array s3:%d\r\n", sizeof(s3));
printf("size of array s4:%d\r\n", sizeof(s4));
printf("size of array s5:%d\r\n", sizeof(s5));
#endif

  while (1)
  {	
	 	printf("ver 0811 (Stone 2023-12-12)\r\n");
		printf("(0)Show Status\r\n");
  		printf("(1)Read REG\r\n");
		printf("(2)Write REG\r\n");
		printf("(3)Read MEM\r\n");
		printf("(4)Write MEM\r\n");
		
		printf("(5)Tx/Rx Pkt: loop back \r\n");
		printf("(51)Rx Only by Interruption\r\n");
		printf("(52)Rx Only by Polling\r\n");
		bannerline();
		
		printf("(6)Read PHY REG\r\n");
		printf("(7)Write PHY REG\r\n");
		printf("(8)Read EEPROM\r\n");
		printf("(9)Write EEPROM\r\n");
		printf("(10) Wake up on Lan\r\n");
		printf("(11) Hash table scan\r\n");
		printf("(12) 802.3u tx test\r\n");
		printf("(13) Mac Register List\r\n");
		printf("(14) Phy Register List\r\n");
		printf("(61) SMB TX Echo by Interruption\r\n");
		printf("(62) SMB TX Echo by Polling\r\n");
		printf("-> ");
		scanf("%d", &sel);

		switch(sel) 
		{
			case 0:		// show link status
			printf("\r\n");
			printf("DM9051 Diagnostic Test Progran. Ver 0500 \r\n");
			sel = 0x29;
			DM9051_Read_Register(eDRVSPI_PORT1, sel, &spi_t[1]);
			sel = 0x28;
			DM9051_Read_Register(eDRVSPI_PORT1, sel, &spi_t[2]);
			spi_t[3] = spi_t[1]<<8 |spi_t[2];
			
			sel = 0x2B;
			DM9051_Read_Register(eDRVSPI_PORT1, sel, &spi_t[4]);
			sel = 0x2A;
			DM9051_Read_Register(eDRVSPI_PORT1, sel, &spi_t[5]);
			//spi_t[6] = spi_t[4]<<8 |spi_t[5];
			//printf("Vender ID:%04X, Product ID:%04X\r\n",spi_t[3],spi_t[6]);

			sel = 0x0;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[0]);
			if (spi_t[0] & 0x06) {
			printf("Link Mode:%s\r\n",mode[0]);
				} 
			else{
				printf("Link Mode:%s\r\n",mode[1]);
				}
			sel = 0x01;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[0]);
			if (spi_t[0] & 0x40) {
				printf("Link status: %s:", link[0]);
				sel = 0x11;
				mdata = DM9051_Read_Phy(sel);
				// printf("%04X\r\n",mdata & 0xf000);
				switch(mdata & 0xf000)	{
					case 0x8000:
						printf("%s\r\n",speed[0]);
						break;
					case 0x4000:
						printf("%s\r\n",speed[1]);
						break;
					case 0x2000:
						printf("%s\r\n",speed[2]);
						break;
					case 0x1000:
						printf("%s\r\n",speed[3]);
						break;
					default:
  					printf("No matched selection...\r\n");
  					break;
					}
			}
			else {
			printf("Link status: %s\r\n", link[1]);
			}
			sel = 0x10;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[0]);
			sel = 0x11;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[1]);
			sel = 0x12;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[2]);
			sel = 0x13;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[3]);
			sel = 0x14;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[4]);
			sel = 0x15;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[5]);
			printf("Reg10~15=%02x:%02x,%02x,%02x,%02x,%02x\r\n",spi_t[0],spi_t[1],spi_t[2],spi_t[3],spi_t[4],spi_t[5]);


			sel = 0x22;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[0]);
			sel = 0x23;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[1]);
			spi_t[2] = spi_t[1] << 8 | spi_t[0];

			sel = 0x24;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[3]);
			sel = 0x25;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[4]);
			spi_t[5] = spi_t[4] << 8 | spi_t[3];
			printf("REG23_22(TX_WR)=%04X, REG25_24(RX_WR)=%04X\r\n",spi_t[2],spi_t[5]);

			sel = 0x7A;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[0]);
			sel = 0x7B;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[1]);
			spi_t[2] = spi_t[1] << 8 | spi_t[0];

			sel = 0x74;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[3]);
			sel = 0x75;
			DM9051_Read_Register(eDRVSPI_PORT1, sel,&spi_t[4]);
			spi_t[5] = spi_t[4] << 8 | spi_t[3];
			 printf("REG7B_7A(TX_RD)=%04X, REG75_74(RX_RD)=%04X\r\n",	spi_t[2],spi_t[5]);
			break;
		
			case 1: 	// Read Register
				//while(1) {
					printf("Read REG No: 0x");
					scanf( "%x", &sel);
					if(sel > 0xff) break;		// escape this loop
					
					/* Read DM9051 Register */
					DM9051_Read_Register(eDRVSPI_PORT1, sel, &Spi_Data);	
					//Command = sel | SPI_READ;
//					SPI_Byte_Read (sel);
					//while (!NSSMD0);                    // Wait until the Read transfer has
        		                               // finished
					printf("\r\nREG_%Xh = 0x%X\r\n", sel, Spi_Data);
				//};
  			break;
  		case 2:		//Write register and then print out
  			//while(1) {
  				printf("Write REG No: 0x");
  				scanf("%x", &sel);
  				if(sel > 0xff) break;		// escape this loop
  				printf("Write REG_%Xh Data = 0x", sel);
  				scanf("%x", &Spi_Data);
  				
  				DM9051_Write_Register(eDRVSPI_PORT1, sel, &Spi_Data);
  				
  				DM9051_Read_Register(eDRVSPI_PORT1, sel, &Spi_Data);
  				
  				//Command = sel | SPI_WRITE;
  				//SPI_Data = (unsigned char)mdata;
//  				SPI_Byte_Write(sel, mdata);
  				//while (!NSSMD0);
  				
  				//Read back
  				//Command = sel | SPI_READ;
//					SPI_Byte_Read (sel);
					//while (!NSSMD0);                    // Wait until the Read transfer has
        		                               // finished
					printf("\r\nRead REG_%Xh = 0x%X\r\n", sel, Spi_Data);
  			//};	  
  			break;

  		case 3:		//Read burst memory test
  			// Disable SRAM Read/Write pointer automatic return to the start address.
  			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
  			Spi_Data &= 0x7F;
  			DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
  			
  		//	DM9051_Read_Register(DM9051_REG_IMR, &Spi_Data);
  		//	DM9051_Write_Register(DM9051_REG_IMR, (Spi_Data & 0x7F));
  		
  			
  			printf("\r\nStart address H: 0x");
  			scanf("%x", &mdata_h);
  			DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_MRRH, &mdata_h);
  			
  			printf("\r\nStart address L: 0x");
  			scanf("%x", &mdata_l);
  			
  			DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_MRRL, &mdata_l);
  			mem_loc = mdata_h << 8 | mdata_l;
  			printf("\r\nNumber of read: 0x");
  			scanf("%x", &mdata);
  			
  			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX, &Spi_Data);	//DM9051 memory dummy read command

				DM9051_Read_Mem(eDRVSPI_PORT1, SPI_Data_Array, mdata);
			/*
  			for(i=0; i<mdata; i++) {
  				printf("0x%X ", (unsigned int)SPI_Data_Array[i]);
  			}
			*/
			line_cnt= mdata / 16;
			rest_cnt = mdata % 16; 
			printf("0x%04X: ", mem_loc);
			for (i=0; i<line_cnt; i++) {
			mem_loc += 16;
			
				for (j=0;j < 16; j++) {
				printf("0x%02X ", (unsigned int)SPI_Data_Array[j+16*i]);
				
					}
				printf("\r\n");
				printf("0x%04X: ", mem_loc);
				}
			
			mem_loc += 16;
			//printf("0x%04X: ",mem_loc);
			for (j=0; j < rest_cnt; j++) {
				printf("0x%02X ", (unsigned int)SPI_Data_Array[16*line_cnt+j]);
				}
				printf("\r\n");	
  			//printf("\r\n");
  			
  			// Enable SRAM Read/Write pointer automatic return to the start address.
  			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
  			Spi_Data |= 0x80;
  			DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
  			
  			//SPI_Byte_Read(DM9051_REG_IMR);
  			//SPI_Byte_Write(DM9051_REG_IMR, (SPI_Data | 0x80));
  			//while (!NSSMD0);
  			 
  			break;

  		case 4:		//Write burst memory test
  			// Disable SRAM Read/Write pointer automatic return to the start address.
  			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
  			Spi_Data &= 0x7F;
  			DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
  			
  			printf("\r\nStart address H: 0x");
  			scanf("%x", &mdata);
  			DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_MWRH, &mdata);
  			printf("\r\nStart address L: 0x");
  			scanf("%x", &mdata);
  			DM9051_Write_Register (eDRVSPI_PORT1, DM9051_REG_MWRL, &mdata);
  			printf("\r\nNumber of write: 0x");
  			scanf("%x", &mdata);

  			//Creat buffer data 
  			for (i = 0; i < mdata; i++)
   			{
      			SPI_Data_Array[i] = i;
   			}

				DM9051_Write_Mem(eDRVSPI_PORT1, SPI_Data_Array, mdata);

        
                    // Enable SRAM Read/Write pointer automatic return to the start address.
  			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
  			Spi_Data |= 0x80;
  			DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
                               
  			break;

  		case 5:		//Tx/Rx Packets
  			
			
			/*  rx all packet */
			
			Spi_Data = 0x0f;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_RCR, &Spi_Data);
			/* single pkg mode */
			//Spi_Data = 0x10;
			//DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_TCR2, &Spi_Data);
			
			Interrupt_initial_setting();
			
			printf("Internal mac Loop-back:0, Normal mode:1, Internal PHY 100M mode digital Loop-back 2: \r\n");
			scanf("%d", &sel);
			if (sel == 0) {
			DM9051_Read_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			Spi_Data |= 0x02;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			printf("MAC internal Loop-back mode\r\n");
				} else if (sel == 1) {
			printf("Normal mode\r\n");
					}
				else if (sel == 2) {
			DM9051_Read_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			Spi_Data |= 0x04;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			printf("Internal PHY 100M mode digital Loop-back mode\r\n");
					}
				
			printf("Input number of tx packets\r\n");
			scanf("%d", &tx_n);

			printf("Input tx pkg content, 0: Inc, 1: pattern, 2: random[0x00~ff] \r\n");
			scanf("%d", &pat_sel);

			printf("Input min pkt length, 0 for random length [64,1508]\r\n");
			scanf("%d", &min_len);

			if(min_len) {
			printf("Input max pkt length\r\n");
			scanf("%d", &max_len);

			printf("Input delta length\r\n");
			scanf("%d", &delta);
				} 

			if (pat_sel == 1) {
			printf("keyin pkt pattern, 0x00: 0xff \r\n");
			scanf("%x", &pats); //&pat
				}

			printf("tx only: 0, tx with rx by interrupt: 1,tx with rx by polling: 2  \r\n");
			scanf("%d", &sel);
			
			printf("Protocol: UDP: 0, TCP/IP: 1 \r\n");
			scanf("%d", &pro_sel);
			/* enable flow control */
			/*
			Spi_Data = 0x021;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_FCR,&Spi_Data);
			*/
			
			DM9051_Read_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);
			  //printf("before tx_n loop reg:0x7F=%x\r\n",Spi_Data);
			rx_n = 0;
			fifo_over_err = 0;
			crc_err = 0;
			rxlen=0;
			c=0; //  tx high byte for unsigned long int , h
			y=0; //  rx high byte for unsigned long int , h
			g=0;  // pkt counter
			h=0;  //  accumulated tx low bytes, 32 bits
			p= 0; //  accumulated rx low bytes, 32 bits
			/* */
			if(min_len <= 60) {
				srand(min_len);
				curlen = rand() % 1508;
				if (curlen <64)
					curlen = 64;
				} else {
					curlen = min_len;
				}
				mpktlen = curlen;
//				tx_tmp=tx_n;
				//printf("tx_n %d\r\n", tx_n);		
  		while(tx_n --	)  {
		if (curlen <64) curlen = 64;	
		mpktlen = curlen;	
		  	//printf("mpktlen %d\r\n", mpktlen);
  			SPI_Data_Array[0] = 0xff;
  			SPI_Data_Array[1] = 0xff;
  			SPI_Data_Array[2] = 0xff;
  			SPI_Data_Array[3] = 0xff;
  			SPI_Data_Array[4] = 0xff;
  			SPI_Data_Array[5] = 0xff;
			//mpktlen = min_len + delta ;
  			for (i = 6; i < mpktlen; i++)
   			{
   				if(pat_sel==1) {
      			SPI_Data_Array[i] = pats;
   					}
				else if (pat_sel==0){
				SPI_Data_Array[i] = (uint8_t)i;
					}
				else {
					srand(i);
					r = rand()& 0xff;
					//SPI_Data_Array[i] = r % 256;
					//printf("random r = %d \r\n", r);	
					SPI_Data_Array[i] = r;	
					}
   			}
			//printf("SPI_Data_Array[%d] = %d\r\n", i, SPI_Data_Array[i]);
			ip_len = mpktlen - 14;
			SPI_Data_Array[12]=0x08;
        	SPI_Data_Array[13]=0x00;
			
        	SPI_Data_Array[14]=0x45;
        	SPI_Data_Array[15]=0x00;
			SPI_Data_Array[16] = (ip_len & 0xff00) >> 8;
            SPI_Data_Array[17] = (ip_len & 0x00ff);
			SPI_Data_Array[20]=0x40;
        	SPI_Data_Array[21]=0x00;
			//printf("pro_sel = %d \r\n",pro_sel);	
			if ( pro_sel)  {
        	SPI_Data_Array[23]=0x06;
		   	SPI_Data_Array[46]=0x50;
			
            SPI_Data_Array[42] = ((ip_len-20) & 0xff00) >> 8;
            SPI_Data_Array[43] = ((ip_len-20) & 0x00ff);
			 // TCP
			 } else {
			  SPI_Data_Array[23]=0x11;
			  SPI_Data_Array[38] = ((ip_len-20) & 0xff00) >> 8;
              SPI_Data_Array[39] = ((ip_len-20) & 0x00ff);
			 }
			  //printf("sel = %d \r\n",sel);
   			//Send a packet
   			//TX_Data_len = mpktlen;
   			//etherdev_send(SPI_Data_Array, mpktlen);
   			if(sel==1) {
				//printf("sel = 1 in \r\n");
			  //Interrupt_initial_setting();
				//	mpktlen = curlen;
			#if 0
			   	curlen= curlen+ delta;
				if( curlen> max_len) 
			    curlen = min_len;
			#endif	
                     #if 1
                      //Spi_Data = 0x80;
			  //DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);
			  Interrupt_disable_setting();
                      #endif
        //printf("etherdev_send == sel = 1 tx with rx by interrupt == \r\n");              
				etherdev_send(SPI_Data_Array, mpktlen);
				
			   #if 1
			 // Spi_Data = 0x81;
			  //DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);
			  Interrupt_initial_setting();
                      #endif
				 /* cal tx num and bytes */ 
				g++;
				h = h + mpktlen;
				if(h > 100000000) 
				{
					h = h % 100000000;
					c++;
				}
				if((g % 1000) == 0) {
				
					if (c>0 | y>0 ) {
					printf("tx %ld pkts, %d%ld bytse\r\n",g,c,h);
					printf("%ld pkt received, %d%ld bytes received\r\n",rx_n,y,p);
					} else {
					printf("tx %ld pkts,%ld bytse\r\n",g,h);
					printf("%ld pkt received, %ld bytes received\r\n",rx_n,p);
					}
				    printf("\r\n");
					DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data);
					printf("DM9051_REG_IMR=0x %x \r\n",Spi_Data);
					DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
					printf("DM9051_REG_ISR=0x %x \r\n",Spi_Data);
					DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_RCR,&Spi_Data);
					printf("DM9051_REG_RCR=0x %x \r\n",Spi_Data);
				}

					  
   			 } 
			 else if(sel == 2) {
				//printf("sel = 2 in \r\n"); 
			 	Interrupt_disable_setting();
				//printf("Interrupt_disable_setting end \r\n");  
				#if 0 
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
				//DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
					while(Spi_Data & 0x01) {
					//DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_ISR	 , &Spi_Data);	
   					rxlen = etherdev_read();
					//printf("rxlen:%ld\r\n",rxlen);
					if (rxlen >0) {
					p=p+rxlen;
					//printf("p:%ld\r\n",p);
					rx_n++;
						}
					if(p> 100000000) {
						p= p % 100000000;
						y++;
						}
					/*
				 	mem_loc = 0x0;
					line_cnt= rxlen / 16;
					rest_cnt = rxlen % 16; 
					printf("0x%04X: ", mem_loc);
						for (i=0; i<line_cnt; i++) {
						mem_loc += 16;
			
							for (j=0;j < 16; j++) {
							printf("0x%02X ", (unsigned int)SPI_Data_Array[j+16*i]);
				
							}
						printf("\r\n");
						printf("0x%04X: ", mem_loc);
							}
			
					mem_loc += 16;
					for (j=0; j < rest_cnt; j++) {
					printf("0x%02X ", (unsigned int)SPI_Data_Array[16*line_cnt+j]);
					}
					printf("\r\n");	
					printf("\r\n");
					*/
			
			

				//Spi_Data = 0x01;
				//DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_ISR	 , &Spi_Data);
				//DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
				}
				#endif
			//printf("tx_length:%d\r\n",mpktlen);
      //printf("etherdev_send == sel = 2 tx with rx by polling == \r\n");	
			etherdev_send(SPI_Data_Array, mpktlen);
			  //Stone add
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
				//DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
				//printf("register 0x70 = %x\r\n",Spi_Data);
					while(Spi_Data & 0x01) {
					//DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_ISR	 , &Spi_Data);	
   					rxlen = etherdev_read();
					//printf("rxlen:%ld\r\n",rxlen);
					if (rxlen >0) {
					p=p+rxlen;
					//printf("p:%ld\r\n",p);
					rx_n++;
						}
					if(p> 100000000) {
						p= p % 100000000;
						y++;
						}
					/*
				 	mem_loc = 0x0;
					line_cnt= rxlen / 16;
					rest_cnt = rxlen % 16; 
					printf("0x%04X: ", mem_loc);
						for (i=0; i<line_cnt; i++) {
						mem_loc += 16;
			
							for (j=0;j < 16; j++) {
							printf("0x%02X ", (unsigned int)SPI_Data_Array[j+16*i]);
				
							}
						printf("\r\n");
						printf("0x%04X: ", mem_loc);
							}
			
					mem_loc += 16;
					for (j=0; j < rest_cnt; j++) {
					printf("0x%02X ", (unsigned int)SPI_Data_Array[16*line_cnt+j]);
					}
					printf("\r\n");	
					printf("\r\n");
					*/
			
			

				//Spi_Data = 0x01;
				//DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_ISR	 , &Spi_Data);
				//DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
				}
				
			/* cal tx num and bytes */ 
			g++;
			h = h + mpktlen;
			if(h > 100000000) {
			h = h % 100000000;
			c++;
				}
			if((g % 1000) == 0) {
			
				if (c>0 | y>0 ) {
				printf("tx %ld pkts, %d%ld bytse\r\n",g,c,h);
				printf("%ld pkt received, %d%ld bytes received\r\n",rx_n,y,p);
				} else {
				printf("tx %ld pkts,%ld bytse\r\n",g,h);
				printf("%ld pkt received, %ld bytes received\r\n",rx_n,p);
				}
			printf("\r\n");
			}
			
			//wait_phy_end();
			//DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
			} 			
   			   				
   		

			else if(sel == 0)
			{
			//printf("etherdev_send == sel = 0 tx only == \r\n");
			etherdev_send(SPI_Data_Array, mpktlen);
			
			      /* cal tx num and bytes */ 
				g++;
				h = h + mpktlen;
				if(h > 100000000) {
				h = h % 100000000;
				c++;
					}
				if((g % 1000) == 0) {
				
					if (c>0 | y>0 ) {
					printf("tx %ld pkts, %d%ld bytse\r\n",g,c,h);
					printf("%ld pkt received, %d%ld bytes received\r\n",rx_n,y,p);
					} else {
					printf("tx %ld pkts,%ld bytse\r\n",g,h);
					printf("%ld pkt received, %ld bytes received\r\n",rx_n,p);
					}
				printf("\r\n");
				}
			}

			if(min_len == 0) {
				srand(mpktlen);
				curlen = rand() % 1508;
			
				if (curlen < 64) mpktlen = curlen;  // hank
				} else 
				{
				curlen= curlen+ delta;
				if( curlen> max_len) {
			    curlen = min_len;
				}
			}
   		}
		if ((c>0) | (y>0)) {
				printf("tx %ld pkts, %d%ld bytse\r\n",g,c,h);
				printf("%ld pkt received, %d%ld bytes received\r\n",rx_n,y,p);
				} else {
				printf("tx %ld pkts,%ld bytse\r\n",g,h);
				printf("%ld pkt received, %ld bytes received\r\n",rx_n,p);
				}
			printf("\r\n");
			

		printf("FIFO Overflow Error:%d, CRC Error:%d\r\n",fifo_over_err,crc_err);
		break;

  		case 6:
				  printf("\r\n");
  			//while(1) {
					printf("Read PHYREG No: 0x");
					scanf( "%x", &sel_1);
					if(sel_1 > 0xff) break;		// escape this loop
						
					//Command = sel | SPI_READ;
					mdata = DM9051_Read_Phy(sel_1);
					//while (!NSSMD0);                    // Wait until the Read transfer has
        		    wait_phy_end();
					printf("\r\n");                           // finished
					printf("PHYREG_%Xh = 0x%04X\r\n", sel_1, mdata);
					
				//};
				
  			break;
			
  		case 7:
				  printf("\r\n");
  			//while(1) {
  				printf("Write PHYREG No: 0x");
  				scanf("%x", &sel_1);
  				if(sel_1 > 0xff) break;		// escape this loop
  				printf("Write PHYREG_%Xh = 0x", sel_1);
  				scanf("%x", &mdata);
  				
  				//Command = sel | SPI_WRITE;
  				//SPI_Data = (unsigned char)mdata;
  				DM9051_Write_Phy(sel_1, mdata);
  				//while (!NSSMD0);
  				wait_phy_end();
  				//Read back
  				//Command = sel | SPI_READ;
					mdata = DM9051_Read_Phy(sel_1);
					//while (!NSSMD0);                    // Wait until the Read transfer has
        		    wait_phy_end();    
					 printf("\r\n");                    // finished
					printf("Read PHYREG_%Xh = 0x%04X\r\n", sel_1, mdata);
					
  			//};
  			break;
		
  		case 8:		// Read EEPROM
			        printf(" Read n words!\r\n");
  		            printf("Read EEPROM start Word: 0x");
					scanf( "%x", &sub_sel);
					printf("%x\r\n",sub_sel);
					//if(sub_sel > 0xff) break;		// escape this loop
					printf("Read number of Words: 0x");
					scanf( "%x", &sel);
					printf("%x\r\n",sel);
					//if(sel > 0xff) break;		// escape this loop
					for(i=0;i<sel;i++)
					{
	  			        DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_EPAR, &sub_sel);	 //0x0c
						eerom_data=0x04;
						DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_EPCR, &eerom_data);	  //0x0b
						eerom_data=0;
						do
						{
							DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_EPCR, &eerom_data);
						}
						while((eerom_data)&1);	
						for(j=0;j<500;j++);
						eerom_data=0;
						DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_EPCR, &eerom_data);	  //clear command
						DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_EPDRH, &eerom_data);
						mdata_h= eerom_data;
						DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_EPDRL, &eerom_data);
						mdata_l= eerom_data;
						mdata= (mdata_h<<8)|(mdata_l) ;
						//mdata = (unsigned int)dm9051_eeprom_read(sel);
						//while (!NSSMD0);                    // Wait until the Read transfer has
	        		                               // finished
						if(i%4==3)
						printf("WORD[%Xh] = 0x%04X\r\n", sub_sel, mdata);
						else
						printf("WORD[%Xh] = 0x%04X  ", sub_sel, mdata);
						
						sub_sel+=1;
					}
  			        break;
  		case 9:		//Write EEPROM
  			//while(1) {
  				printf("Write EEPROM Word: 0x");
  				scanf("%x", &sel);
  				if(sel > 0xff) break;		// escape this loop
  				printf("Write EEPROM Word_%Xh = 0x", sel);
  				scanf("%x", &mdata);
  				DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_EPAR, &sel);	 //0x0c
				mdata_h=mdata>>8;
				DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_EPDRH, &mdata_h);	  //write data
				mdata_l=mdata;
				DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_EPDRL, &mdata_l);	  //write data
				mdata=0x12;
				DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_EPCR, &mdata);	  //write data
				eerom_data=0;
				do
				{
						DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_EPCR, &eerom_data);
				}
				while((eerom_data)&1);	
					for(j=0;j<500;j++);
				sel=0;
				DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_EPCR, &sel);	  //clear command

  				//Command = sel | SPI_WRITE;
  				//SPI_Data = (unsigned char)mdata;
  				//dm9051_eeprom_write(sel, mdata);
  				//while (!NSSMD0);
  				
  				//Read back
  				//Command = sel | SPI_READ;
					//mdata = (unsigned int)dm9051_eeprom_read(sel);
					//while (!NSSMD0);                    // Wait until the Read transfer has
        		                               // finished
					//printf("Read EEPROM Word_%Xh = 0x%04X\r\n", sel, mdata);
  			//};
  			break;
case  10 :
			printf("\r\n");
	       	printf("(a) Magic packet\r\n");
			printf("(b) Link change\r\n");
			printf("(c) Sample frame\r\n");																			  
			scanf("%c", &wol_type);
			switch(wol_type) 
			{
			case 'a':
			DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);
		    mdata|=0x08;// magic packet  event enable
		    DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);	 //0x0F
		    DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);
		    mdata|=0x40;//wake up remote enable
		    DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);	 //0x00
		    mpktlen = 108;
			for(i=0;i<6;i++)
			{
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_PAR+i, &spi_t[i]); 
			}
  			//Creat buffer data 
  			SPI_Data_Array[0] = 0xff;
  			SPI_Data_Array[1] = 0xff;
  			SPI_Data_Array[2] = 0xff;
  			SPI_Data_Array[3] = 0xff;
  			SPI_Data_Array[4] = 0xff;
  			SPI_Data_Array[5] = 0xff;
  			for (i = 6; i < mpktlen;)
   			{
      		  SPI_Data_Array[i] = (uint8_t)spi_t[0];
			  SPI_Data_Array[i+1] = (uint8_t)spi_t[1];
			  SPI_Data_Array[i+2] = (uint8_t)spi_t[2];
			  SPI_Data_Array[i+3] = (uint8_t)spi_t[3];
			  SPI_Data_Array[i+4] = (uint8_t)spi_t[4];
			  SPI_Data_Array[i+5] = (uint8_t)spi_t[5];
			  i=i+6;
   			}
			 //Send a packet
   			etherdev_send(SPI_Data_Array, mpktlen); 
				//Receive a packet
   			rxlen = etherdev_read();
			for(i = 0; i < rxlen; i++) {
   				printf("%X ",(unsigned int)SPI_Data_Array[i]);	
   			}
   			printf("\r\n");
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);
			  if((mdata&0x01)==0x01) 
			  	printf("Magic packet received and event occured!!\r\n");
			  else
				printf("Magic packet is not received and event not occured!!\r\n"); 
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_NSR, &mdata);
			  if((mdata&0x20)==0x20) 
			  {
			    mdata|=0x20;
			    DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_NSR, &mdata);
			  	printf("Magic packet test finished!!\r\n");
			  }
			  else
			    printf("Magic packet test failed!!\r\n");
			  break;
			case  'b':
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);
		      mdata|=0x20;// Link change event enable
		      DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);	 //0x0F
		      DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);
		      mdata|=0x40;//wake up remote enable
		      DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);	 //0x00
			    scanf("%c", &test_temp);
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);
			  if((mdata&0x04)==0x04) 
			  	printf("Link change event occured!!\r\n");
			  else
			    printf("Link change event failed\r\n");
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_NSR, &mdata);
			  if((mdata&0x20)==0x20) 
			  {
			    mdata|=0x20;
			    DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_NSR, &mdata);
			  	printf("Link change test finished!!\r\n");
			  }
			  else
			    printf("Link change test failed!!\r\n");
			  break;
			case  'c':
			/* software reset  */
			Spi_Data = 0x01;
      		       DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
				   
			 DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_NSR, &mdata);
				    mdata|=0x20;
				    DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_NSR, &mdata);
				  	//printf("Sample frame test finished!!\r\n");
								
				DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);//0x0F
			    mdata&=0x00;	 //sample frame  disable
			    DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);//0x0F 
				
				
			  printf("Sample frame test started...!\r\n");
	
#if 0		  
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_RCR, &mdata);//0x05
			  mdata&=0xFE;	 //clear RX EN bit
			   DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_RCR, &mdata);//0x05
			   
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0xFF
			   mdata&=0x7F; //Clear IMR PAR bit7
			  DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0xFF
#endif			  
          mdata=0x00;   
          DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0x7F
			  //mdata&=0x81; //limit to 64 bytes
			  //DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_RLENCR, &mdata);//0x81

#if 0
			  // initialize condition bits
 for(i=0x2000;i<=0x3ffc; i+=4)
    {
	 // write pattern byte
        iow(0x7a,i & 0xff); //Low byte
        iow(0x7b,(i>>8) & 0xff); //High byte
        iow(0x78,0xaa);
	//printf("i=0x%04X\r\n",i);
	iow(0x7a,(i+1) & 0xff);
        iow(0x7b,((i+1)>>8) & 0xff);
        iow(0x78,0x0a);
	 

 	}
#endif	
			  
			  //6 sample frames put in 16KB RX/TX FIFO RAM
			
			/*  
			sample_length=sizeof(s0);
			for(i=0;i<sample_length-1;i++) {
			SPI_Data_Array[i]=0x01;
				}
			SPI_Data_Array[sample_length-1]=0x03;
			printf("size of array s0:%d\r\n", sample_length);  
			sample_frame_mapping(s0, SPI_Data_Array,sample_length,0);

			sample_length=sizeof(s1);
			for(i=0;i<sample_length-1;i++) {
			SPI_Data_Array[i]=0x01;
				}
			SPI_Data_Array[sample_length-1]=0x03;
			printf("size of array s1:%d\r\n", sample_length);  
			sample_frame_mapping(s1, SPI_Data_Array,sample_length,1);
			*/
#if 1	
	    //Stone add for WOL sample frame debug! 
      printf("Sample frame length (16 or 256)\r\n");																			  
			scanf("%d", &sample_len);
			if (sample_len == 256)
			  sample_length=sizeof(s1);
			else sample_length = 0x10;
			
			for(i=0;i<sample_length-1;i++) {
			SPI_Data_Array[i]=0x01;
				}
			SPI_Data_Array[sample_length-1]=0x03;
			printf("size of array s1:%d\r\n", sample_length);  
			sample_frame_mapping(s1, SPI_Data_Array,sample_length,1);
				
			printf("Prepare data in memory already!\r\n"); 	
#endif

			//sample_length=sizeof(frame_s2);
			//printf("size of array fram_s2:%d\r\n", sample_length);  
			//======================================================	
				
			//sample_frame_mapping(frame_s2,flag_s2,sample_length,2);
			
			/*
			sample_length=sizeof(s3);
			for(i=0;i<sample_length-1;i++) {
			SPI_Data_Array[i]=0x01;
				}
			SPI_Data_Array[sample_length-1]=0x03;
			printf("size of array s3:%d\r\n", sample_length);  
			sample_frame_mapping(s3,SPI_Data_Array,sample_length,3);

			sample_length=sizeof(s4);
			for(i=0;i<sample_length-1;i++) {
			SPI_Data_Array[i]=0x01;
				}
			SPI_Data_Array[sample_length-1]=0x03;
			printf("size of array s4:%d\r\n", sample_length);  
			sample_frame_mapping(s4,SPI_Data_Array,sample_length,4);

			sample_length=sizeof(s5);
			for(i=0;i<sample_length-1;i++) {
			SPI_Data_Array[i]=0x01;
				}
			SPI_Data_Array[sample_length-1]=0x03;
			printf("size of array s5:%d\r\n", sample_length);  
			sample_frame_mapping(s5, SPI_Data_Array,sample_length,5);
			*/


#if 0
			/* Sample fram enable */
			DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);//0x0f
			  mdata|=0x18;	 //clear wol EN bit
			   DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);//0x0f

			   
			/* wakeup remote enable */
			DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);//0x00
			  mdata|=0x00;	 //clear RX EN bit
			   DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);//0x00
			   
			DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);//0x00
			  mdata|=0x40;	 //clear RX EN bit
			   DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);//0x40
			   
			  

			/* enable RX machine filter */
			  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_RCR, &mdata);//0x05
			  mdata|=0x0e;	 //clear RX EN bit
			   DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_RCR, &mdata);//0x05
#endif
        mdata=0x0e; 
        DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_RCR, &mdata);//0x05

        mdata=0x48; 
        DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);//0x00

        mdata=0x18; 
        DM9051_Write_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);//0x0F

        mdata=0x00; 
        DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0x7F
 
       
				/* show reg status */
			   DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_RCR, &mdata);//0x05
			   printf("Reg[0x%02X]=0x%02X\r\n",DM9000_REG_RCR,mdata);

			   DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_NCR, &mdata);//0x00
			   printf("Reg[0x%02X]=0x%02X\r\n",DM9000_REG_NCR,mdata);

			   DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);//0x0f
			   printf("Reg[0x%02X]=0x%02X\r\n",DM9000_REG_WAR,mdata);

			   DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0x7F
			   printf("Reg[0x%02X]=0x%02X\r\n",DM9051_REG_IMR,mdata);

			   printf("Sample frame allocated ready, waiting for wol of saple frame packet.\r\n");
			   
			/*
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0xFF
			   mdata&=0x7F; //Clear IMR PAR bit7
			  DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0xFF	
			*/	
#if 0			
				do
				{
				   DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);
				   
				 }
				while((mdata &0x02)!=0x02);
				DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);//0x0f
			    printf("after trigger Reg[0x%02X]=0x%02X\r\n",DM9000_REG_WAR,mdata);
			if((mdata&0x02)==0x02) 
			  	    printf("Sample frame event occured!!\r\n");
			else
			        printf("Sample frame event failed\r\n");
			
				
			   
				DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0xFF
			    mdata|=0x80;	 //set IMR PAR bit7
				//mdata|=0x01;
			    DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &mdata);//0xFF
#endif
        DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_WAR, &mdata);//0x0f
			    printf("after trigger Reg[0x%02X]=0x%02X\r\n",DM9000_REG_WAR,mdata);
         			
			    break;
			  

			}
		    break;	
  	case  51:
				//k= 0;
			rx_n = 0;
			/* software reset  */
			Spi_Data = 0x01;
      		       DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			 wait_phy_end();
			/*  rx all packet */
			Spi_Data = 0x0f;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_RCR, &Spi_Data);
			/* enable flow control */
			Spi_Data = 0x21;			
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_FCR,&Spi_Data);
			Spi_Data = 0x00;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_PPCSR,&Spi_Data);

		    /* enable pkt rx interrupt */
			/*DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data);			
			printf("read rx reg:0x7F=%x\r\n",Spi_Data);
			Spi_Data = 0x81;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);

			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data);
			printf("rx reg:0x7F :%x\r\n",Spi_Data);		   */
			Interrupt_initial_setting();
			fifo_over_err = 0;
			crc_err = 0;
			
		   while(1)
		   {
		   	              UART_RxBuf=0;
#if 0
					 DrvUART_Read(UART_PORT0,&UART_RxBuf, 1);
#else
					printf("scanf scan a char: \r\n");
					scanf("%c", &UART_RxBuf);
#endif
					 		
					if(UART_RxBuf==27)
					{
					   printf("Exit RX only test item!!\r\n");  
					   UART_RxBuf=0;
					   sel=0;
					   Interrupt_disable_setting();	
					   printf("Number of pkt %ld received\r\n",rx_n);
			                 printf("FIFO Overflow Error:%d, CRC Error:%d\r\n",fifo_over_err,crc_err);				   
					   break;
					}
			}
			
			break;
			case  61:
							//k= 0;
						rx_n = 0;
						/* software reset  */
						Spi_Data = 0x01;
							   DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
						 wait_phy_end();
						/*	rx all packet */
						Spi_Data = 0x0f;
						DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_RCR, &Spi_Data);
						/* enable flow control */
						Spi_Data = 0x21;			
						DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_FCR,&Spi_Data);
						/*
						Spi_Data = 0x00;
						DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_PPCSR,&Spi_Data);
						*/
						/* enable pkt rx interrupt */
						/*DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data); 		
						printf("read rx reg:0x7F=%x\r\n",Spi_Data);
						Spi_Data = 0x81;
						DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);
			
						DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data);
						printf("rx reg:0x7F :%x\r\n",Spi_Data);		   */
						Interrupt_initial_setting();
						
						fifo_over_err = 0;
						crc_err = 0;
						
					   while(1)
					   {
								 UART_RxBuf=0;
#if 0	 
								 DrvUART_Read(UART_PORT0,&UART_RxBuf, 1);
								//printf("rx length:%d\r\n",rxlen);
#else
								printf("scanf scan a char: \r\n");
								scanf("%c", &UART_RxBuf);
#endif
								if(rxlen >0 ) {
									etherdev_send(SPI_Data_Array, rxlen);
									rxlen = 0;
									}
								
								if(UART_RxBuf==27)
								{
								   printf("Exit RX only test item!!\r\n");  
								   UART_RxBuf=0;
								   sel=0;
								   Interrupt_disable_setting(); 
								   printf("Number of pkt %ld received\r\n",rx_n);
										 printf("FIFO Overflow Error:%d, CRC Error:%d\r\n",fifo_over_err,crc_err);				   
								   break;
								}
						}
						
						break;

			case  52:
			k= 0;
			rx_n = 0;
			/* software reset  */
			//Spi_Data = 0x01;
			//DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			// wait_phy_end();
			/*  rx all packet */
			Spi_Data = 0x0f;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_RCR, &Spi_Data);
			/* enable flow control */
			Spi_Data = 0x21;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_FCR,&Spi_Data);
			
		    /* enable pkt rx interrupt */
			
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data);
			Spi_Data = Spi_Data | 0x01;
			printf("Spi_Data :%x\r\n",Spi_Data);
			DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);

			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
			printf("rx reg:0x7E :%x\r\n",Spi_Data);
																
			fifo_over_err = 0;
			crc_err = 0;
			mdata = 0;
			while(k < 500000){
			
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
			//printf("rx reg:0x70 :%x\r\n",Spi_Data);
				if(Spi_Data & 0x01) {
			//	 DM9051_Write_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
				rxlen = etherdev_read();
				mdata = mdata + rxlen;
					/*
					for(i = 0; i < rxlen; i++) {
   					printf("%X ",(unsigned int)SPI_Data_Array[i]);
					}
					*/
					/*
					mem_loc = 0x0;
					line_cnt= rxlen / 16;
					rest_cnt = rxlen % 16; 
					printf("0x%04X: ", mem_loc);
						for (i=0; i<line_cnt; i++) {
						mem_loc += 16;
			
							for (j=0;j < 16; j++) {
							printf("0x%02X ", (unsigned int)SPI_Data_Array[j+16*i]);
				
							}
						printf("\r\n");
						printf("0x%04X: ", mem_loc);
							}
			
					mem_loc += 16;
					for (j=0; j < rest_cnt; j++) {
					printf("0x%02X ", (unsigned int)SPI_Data_Array[16*line_cnt+j]);
					}
					printf("\r\n");	
				*/	
				rx_n++;
				if((rx_n% 1024)==0) {
				printf("%ld pkt received, %ld bytes received\r\n",rx_n,mdata);
					}
				k=0;
				}
		//	printf("k = %d\r\n",k);
			k++;
			}
		
			printf("Number of pkt %d received, %ld bytes received\r\n",rx_n,mdata);
			printf("FIFO Overflow Error:%d, CRC Error:%d\r\n",fifo_over_err,crc_err);
			break;

		case  62:
			k= 0;
			rx_n = 0;
			
			/*  rx all packet */
			Spi_Data = 0x0f;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_RCR, &Spi_Data);
			/* enable flow control */
			Spi_Data = 0x21;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_FCR,&Spi_Data);
			
		    /* enable pkt rx interrupt */
			/*
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR,&Spi_Data);
			Spi_Data = Spi_Data | 0x01;
			printf("Spi_Data :%x\r\n",Spi_Data);
			DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);

			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
			printf("rx reg:0x7E :%x\r\n",Spi_Data);
			*/													
			fifo_over_err = 0;
			crc_err = 0;
			mdata = 0;
			while(k < 100000){
			
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
			
				if(Spi_Data & 0x01) {
				rxlen = etherdev_read();
				mdata = mdata + rxlen;
					/*
					for(i = 0; i < rxlen; i++) {
   					printf("%X ",(unsigned int)SPI_Data_Array[i]);
					}
					*/
					/*
					mem_loc = 0x0;
					line_cnt= rxlen / 16;
					rest_cnt = rxlen % 16; 
					printf("0x%04X: ", mem_loc);
						for (i=0; i<line_cnt; i++) {
						mem_loc += 16;
			
							for (j=0;j < 16; j++) {
							printf("0x%02X ", (unsigned int)SPI_Data_Array[j+16*i]);
				
							}
						printf("\r\n");
						printf("0x%04X: ", mem_loc);
							}
			
					mem_loc += 16;
					for (j=0; j < rest_cnt; j++) {
					printf("0x%02X ", (unsigned int)SPI_Data_Array[16*line_cnt+j]);
					}
					printf("\r\n");	
				*/	
				rx_n++;
				if((rx_n% 1024)==0) {
				printf("%ld pkt received, %ld bytes received\r\n",rx_n,mdata);
					}
				etherdev_send(SPI_Data_Array, rxlen);
				k=0;
				}
		//	printf("k = %d\r\n",k);
			k++;
			}
		
			printf("Number of pkt %d received, %ld bytes received\r\n",rx_n,mdata);
			printf("FIFO Overflow Error:%d, CRC Error:%d\r\n",fifo_over_err,crc_err);
			break;	
		case 11:
		
			/* software reset  */
			Spi_Data = 0x02;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			 wait_phy_end();
			 dm9k_udelay(1000);
			 
			Spi_Data = 0x01;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_NCR, &Spi_Data);
			 wait_phy_end();
			 dm9k_udelay(1000);
			 
			/*  rx  packet with filter */
			Spi_Data = 0x81;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_RCR, &Spi_Data);
			wait_phy_end();
			dm9k_udelay(1000);
			 DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_RCR,&Spi_Data);
			 printf("reg 0x05=%x\r\n",  Spi_Data);
			/* enable flow control */
			Spi_Data = 0x21;
			DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_FCR,&Spi_Data);
			dm9k_udelay(1000);
			wait_phy_end();
			 DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_FCR,&Spi_Data);
			 printf("reg 0x0a=%x\r\n",  Spi_Data);
			/* TCP and UDP tx check sum enable */
			Spi_Data = 0x07;
			DM9051_Write_Register(eDRVSPI_PORT1,	DM9000_REG_TCSCR , &Spi_Data);
			dm9k_udelay(1000);
			wait_phy_end();
			DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_TCSCR,&Spi_Data);
			 printf("reg 0x31=%x\r\n",  Spi_Data);
			mpktlen = 64;
			for (i = 0; i < mpktlen; i++)
   			{
      		SPI_Data_Array[i] = (uint8_t)i;
   			}
			
			ip_len = mpktlen - 14;
			SPI_Data_Array[12]=0x08;
        	SPI_Data_Array[13]=0x00;
			
        	SPI_Data_Array[14]=0x45;
        	SPI_Data_Array[15]=0x00;
			SPI_Data_Array[16] = (ip_len & 0xff00) >> 8;
            SPI_Data_Array[17] = (ip_len & 0x00ff);
			SPI_Data_Array[20]=0x40;
        	SPI_Data_Array[21]=0x00;
			
			SPI_Data_Array[23]=0x11;
			 
			SPI_Data_Array[38] = ((ip_len-20) & 0xff00) >> 8;
            SPI_Data_Array[39] = ((ip_len-20) & 0x00ff);
			 
		

		for (g=0;g<2;g++)  {
			
			for (k=0;k<8;k++) {
				for(y=0;y<8;y++)  {
				hash_matrix[k][y] = 0;
				}	
    		}

			for (i=0;i<1;i++)  {
				for (l=0x5e;l<0x5f;l++)  {  //20
					for (h=0x00;h<0x20;h++)  { //20	
						for (p=0;p<10;p++)  {  //10
						
						m[0]=2*i+1;
						m[1]=0x0;
						m[2]=l;;
						m[3]=h;
						m[4]=p;
						m[5]=h;
						/* pass filter test , set hash table all 0 */
						
						if(g == 0)  {
						
							for(j=0x16;j<=0x1d;j++)  {
							sel = j;
							Spi_Data = 0x00;
							DM9051_Write_Register(eDRVSPI_PORT1,sel,&Spi_Data);
							wait_phy_end();
							}
						
							dm9k_hash_table();
							bit_ptr=0x00;
							crc32 = ether_crc(6, m);
							crc6 = (crc32 >> 26) & 0x3f;
							//printf("crc32= %x, crc6= %x\r\n",crc32, crc6);
							byte_ptr = tabx[crc6 & 0x07];
							bit_ptr = tabx[crc6 >> 3];
//							crc_tab[byte_ptr] = 1 << bit_ptr;
							//printf("mac:%x-%x-%x-%x-%x-%x\r\n",m[0],m[1],m[2],m[3],m[4],m[5]);
							//printf("hash[%x]=%x\r\n",byte_ptr,crc_tab[byte_ptr]);
							//printf("byte_ptr=%x, bit_ptr=%x\r\n",byte_ptr+0x16,1 << bit_ptr);
							
							sel = byte_ptr+0x16; 
	    						Spi_Data = 1 << bit_ptr;
	    						
	  						DM9051_Write_Register(eDRVSPI_PORT1,sel,&Spi_Data);
							wait_phy_end();						
						//hash_matrix[byte_ptr][bit_ptr]++;
						//printf("%ld\r\n", hash_matrix[byte_ptr][bit_ptr]);
						}  else  {
							for(j=0x16;j<=0x1d;j++)  {
							sel= j; 
    						Spi_Data= 0xff;
    						DM9051_Write_Register(eDRVSPI_PORT1,sel,&Spi_Data);
							wait_phy_end();
							}
							bit_ptr=0x00;
							crc32 = ether_crc(6, m);
							crc6 = (crc32 >> 26) & 0x3f;
							//printf("crc32= %x, crc6= %x\r\n",crc32, crc6);
							byte_ptr = tabx[crc6 & 0x07];
							bit_ptr = tabx[crc6 >> 3];
//							crc_tab[byte_ptr] = 1 << bit_ptr;
							//printf("mac:%x-%x-%x-%x-%x-%x\r\n",m[0],m[1],m[2],m[3],m[4],m[5]);
							//printf("hash[%x]=%x\r\n",byte_ptr,crc_tab[byte_ptr]);
							//printf("byte_ptr=%x, bit_ptr=%x\r\n",byte_ptr+0x16,1 << bit_ptr);
							
							sel = byte_ptr+0x16; 
	    					Spi_Data = 0xff ^ (1 << bit_ptr);
	    					
	  						DM9051_Write_Register(eDRVSPI_PORT1,sel,&Spi_Data);
							wait_phy_end();
						
						}
							
			for (c = 0; c < mpktlen; c++)
   			{
      		SPI_Data_Array[c] = c;
   			}
			
			ip_len = mpktlen - 14;
			SPI_Data_Array[12]=0x08;
        	SPI_Data_Array[13]=0x00;
			
        	SPI_Data_Array[14]=0x45;
        	SPI_Data_Array[15]=0x00;
			SPI_Data_Array[16] = (ip_len & 0xff00) >> 8;
            SPI_Data_Array[17] = (ip_len & 0x00ff);
			SPI_Data_Array[20]=0x40;
        	SPI_Data_Array[21]=0x00;
			
			SPI_Data_Array[23]=0x11;
			 
			SPI_Data_Array[38] = ((ip_len-20) & 0xff00) >> 8;
            SPI_Data_Array[39] = ((ip_len-20) & 0x00ff);/* tx a packet */
							/* prepare mac  */
							for (c=0; c<6; c++) {
							SPI_Data_Array[c]=m[c];
								}
						//		printf("mac:%x-%x-%x-%x-%x-%x\r\n",m[0],m[1],m[2],m[3],m[4],m[5]);
						//	printf("hash[%x]=%x\r\n",byte_ptr,crc_tab[byte_ptr]);
						//	printf("byte_ptr=%x, bit_ptr=%x\r\n",byte_ptr+0x16,1 << bit_ptr);
							/* tx a packet */
							etherdev_send(SPI_Data_Array, mpktlen);
							wait_phy_end();
							dm9k_udelay(1000);
							/* receive a pkt */
							rxlen = etherdev_read();
     						/* count  */ 
							if (rxlen > 0) {
       						hash_matrix[byte_ptr][bit_ptr]++;
							//	printf("\r\n");
			for (k=0;k<8;k++) {
				for(y=0;y<8;y++)  {
				printf("%d\t",hash_matrix[k][y]);
				}
				}
			printf("\r\n");
								}           					
		 				
        					   
						}  // p
						
					}  // h
				}  // l
			
			}  // i
			printf("\r\n");
			for (k=0;k<8;k++) {
				for(y=0;y<8;y++)  {
				printf("%d\t",hash_matrix[k][y]);
				}
			printf("\r\n");
			}	
    		
			} // g

		break;

		case 12:
			printf("Contineous tx test, enter :Esc to stop tx loop.\r\n");
		  //Stone add for SED need
			//printf("Random Length [64,1518]\r\n");
		  printf("Input packet length [64-1518]\r\n");
		  scanf("%d", &packet_len);
			printf("Input tx pkg content, 0: Random, 1: pattern \r\n");
			scanf("%d", &pat_sel);
			if (pat_sel) {
			printf("keyin pkt pattern, 0x00~ff \r\n");
			scanf("%x", &pats); //&pat
				}
			p= 1;
			 //srand(time(NULL));
			
				mpktlen = packet_len;
			
			do {
#if 0				
			srand(mpktlen);	
			mpktlen = rand() % 1508;
			if (mpktlen	< 64) mpktlen = 64;
			printf(" random :%d\r\n",mpktlen);
#endif				
  			for (i = 0; i < mpktlen; i++)
   			{
   				if(pat_sel) {
				
      			SPI_Data_Array[i] = pats;
   					}
				else {
					srand(mpktlen);
					r = rand();

					SPI_Data_Array[i] = r % 256;
					}
   			}
			
		  	etherdev_send(SPI_Data_Array, mpktlen);
#if 0
			DrvUART_Read(UART_PORT0,&c, 1);
#else
			printf("scanf scan a char: \r\n");
			scanf("%c", &c);
#endif
			 //if(getchar()) p = 0;
			}while(c!=27);
			c=0;
			break;
			case 13:
				printf("\r\n");
			for(i=0x00;i<0x80;i++) {
				
				sel = i;
				DM9051_Read_Register(eDRVSPI_PORT1, sel, &Spi_Data);
				if ((i % 0x08)==0 && (i >0) ) printf("\r\n");
				printf("  Mac[%02XH]=%02X",sel,Spi_Data);
				
				
				}
			break;
			case 14:
			printf("\r\n");
			for(i=0;i<12;i++) {
				
				sel = phy[i];
				mdata = DM9051_Read_Phy(sel);
				wait_phy_end();
				if ((i % 4)==0 && (i >0) ) printf("\r\n");
				printf(" Phy[%02d]=%04X",sel,mdata);
				}
			break;
  		default:
  			printf("Error selection...\r\n");
			
			/* */
  			break;
  	}	//end of switch(sel)	
  } //end of while(1)
}

void  Interrupt_initial_setting(void)
{
   	uint32_t Spi_Data;
	  DM9051_Read_Register(eDRVSPI_PORT1, DM9000_REG_INT_CON, &Spi_Data);
	  Spi_Data|=0x01;//INT active low and  INT pin  output
	  DM9051_Write_Register(eDRVSPI_PORT1,DM9000_REG_INT_CON,&Spi_Data);
	  
//	  DrvGPIO_EnableInt (E_PORT3, E_PIN2, E_IO_FALLING, E_MODE_EDGE);
	  //DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
	  Spi_Data=0x81;
	  DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);	
//	  DrvGPIO_SetIntCallback(E_PORT3, External_INT_GRP0);
	  //DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_IMR, &Spi_Data);
	  //printf("interrupt_initial_setting reg:0x7F=%x\r\n",Spi_Data);
}	

void  Interrupt_disable_setting(void)
{
   	uint32_t Spi_Data;
//	  DrvGPIO_DisableInt (E_PORT3, E_PIN2);
	  Spi_Data=0x80;
	  DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_IMR,&Spi_Data);
	  

}

void External_INT_GRP0(uint32_t Spi_Data)
{
	uint16_t mem_loc,line_cnt,rest_cnt,i,j,type,mpktlen,tst_ctr;
	uint8_t val,loop; //led_on_off,led_flag,
	uint32_t sel,mdata;
	type = 0x0000;
       #if 0
   	  uint16_t INT_rx;
	  DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
		INT_rx = ior(DM9051_REG_MRCMDX);
		  INT_rx = ior(DM9051_REG_MRCMDX);	
	  while((Spi_Data & INT_rx) == 0x01)
	  {

	  	while(ior(DM9000_REG_TCR) & DM9000_TCR_SET) 
		{
		if((ior(	DM9000_REG_NSR) & 0x0c) == 0x0c)
		dm9k_udelay(5);
		//	dm9k_reset();
		else  break;
		}
		rxlen = etherdev_read();
			  if (rxlen > 0)
		      {
			    
	
			  }
			INT_rx = ior(DM9051_REG_MRCMDX);
		  	INT_rx = ior(DM9051_REG_MRCMDX);
		  }
	  Spi_Data = 0x01;
	 DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_ISR,&Spi_Data);  
	#endif
	#if 1
	  DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_ISR,&Spi_Data);
	  if((Spi_Data&0x01)==0x01)
	  {
		  DM9051_Write_Register(eDRVSPI_PORT1,DM9051_REG_ISR,&Spi_Data);
		  DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
		  DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
		  while(Spi_Data & 0x01) 
		  {
			 rxlen = etherdev_read();
			  
				  if (rxlen > 0)
			        {
			           p=p+rxlen;
				//    rx_n++;
					 rx_n++;
					mem_loc = 0x0;
					line_cnt= rxlen / 16;
					rest_cnt = rxlen % 16; 
					printf("0x%04X: ", mem_loc);
						for (i=0; i<line_cnt; i++) {
						mem_loc += 16;
			
							for (j=0;j < 16; j++) {
							printf("0x%02X ", (unsigned int)SPI_Data_Array[j+16*i]);
				
							}
						printf("\r\n");
						printf("0x%04X: ", mem_loc);
							}
			
					mem_loc += 16;
					for (j=0; j < rest_cnt; j++) {
					printf("0x%02X ", (unsigned int)SPI_Data_Array[16*line_cnt+j]);
					}
					printf("\r\n");	
					printf("\r\n");

					type = SPI_Data_Array[12]<<8 | SPI_Data_Array[13];
					printf("type : %04X\r\n",type);

					for(i=0;i<6;i++) {
					host_MAC[i]=SPI_Data_Array[i+6];
						}
					for(i=0;i<6;i++) {
					SPI_Data_Array[6+i]=local_MAC[i];
						}

					for(i=0;i<6;i++) {
					SPI_Data_Array[i]= host_MAC[i];
						}
					
					switch(type) 
					{
						/* Polling command */					
						case 0x0900:	
						printf("0900 selection...\r\n");
						mpktlen = 0x45;
						SPI_Data_Array[13]=0x01;
						etherdev_send(SPI_Data_Array, mpktlen);
						break;
						/* LED Command */
						case 0x0A00:	
						printf("0A00 selection...\r\n");
//						led_on_off = SPI_Data_Array[14];
//						led_flag = SPI_Data_Array[15];

//		if(led_on_off) {
//			if (led_flag & 0x01)  	DrvGPIO_ClrBit(E_PORT2,E_PIN5); // switch on LED5
//			if (led_flag & 0x02)	DrvGPIO_ClrBit(E_PORT2,E_PIN6); // switch on LED6
//			if (led_flag & 0x04)	DrvGPIO_ClrBit(E_PORT2,E_PIN7); // switch on LED7
//			if (led_flag& 0x08)		DrvGPIO_ClrBit(E_PORT4,E_PIN0); // switch on LED8
//			if (led_flag& 0x10)		DrvGPIO_ClrBit(E_PORT4,E_PIN1); // switch on LED9
//			if (led_flag& 0x20)		DrvGPIO_ClrBit(E_PORT4,E_PIN3); // switch on LED10
//			if (led_flag& 0x40)		DrvGPIO_ClrBit(E_PORT1,E_PIN0); // switch on LED11
//			if (led_flag& 0x80)		DrvGPIO_ClrBit(E_PORT1,E_PIN4); // switch on LED12
//			}
  			//for(k=0;k<4000000;k++);  { 
   
  				//}
//  		else {		
//			if (led_flag& 0x01)  DrvGPIO_SetBit(E_PORT2,E_PIN5);  //switch LED5 off
//			if (led_flag& 0x02)  DrvGPIO_SetBit(E_PORT2,E_PIN6);
// 			if (led_flag& 0x04)	 DrvGPIO_SetBit(E_PORT2,E_PIN7); // switch off LED7
// 			if (led_flag& 0x08)	 DrvGPIO_SetBit(E_PORT4,E_PIN0); // switch off LED8
// 			if (led_flag& 0x10)	 DrvGPIO_SetBit(E_PORT4,E_PIN1); // switch off LED9
// 			if (led_flag& 0x20)	 DrvGPIO_SetBit(E_PORT4,E_PIN3); // switch onff LED10
// 			if (led_flag& 0x40)	 DrvGPIO_SetBit(E_PORT1,E_PIN0); // switch off LED11
// 			if (led_flag& 0x80)  DrvGPIO_SetBit(E_PORT1,E_PIN4); // switch off LED12

//  			}
		/* reply */
						mpktlen = 0x45;
						SPI_Data_Array[13]=0x01;
						etherdev_send(SPI_Data_Array, mpktlen);
			
			break;

						case 0x0B00:	
						printf("0B00 selection...\r\n");
						sel=SPI_Data_Array[14];
						val=SPI_Data_Array[15];
							switch(val) {
							case 0xFF:
								mdata= 0x05E1;
								break;
							case 0x88:
								mdata = 0x0501;
								break;
							case 0x44:
								mdata = 0x0481;
								break;
							case 0x22:
								mdata = 0x0441;
								break;
							case 0x11:
								mdata = 0x0421;
								break;
							default:
								mdata = val;
								break;
								}
						DM9051_Write_Phy(sel, mdata);	
						wait_phy_end();
						DM9051_Write_Phy(0x0000, 0x1200);
						wait_phy_end();
						tst_ctr=0;
    					loop=1;
    					while(loop)
    					{
        						
	    				DM9051_Read_Register(eDRVSPI_PORT1,0x01,&mdata);	
						printf("Link status, reg_0x01=%x\r\n",mdata);
        				if(!(mdata&0x40))
        				{
                        tst_ctr++;
							for(j=0;j<1000;j++);  { 
   								for(i=0;i<1000;i++) {
   									}
  							}
							DM9051_Read_Register(eDRVSPI_PORT1,0x01,&mdata);
							printf("Link status, reg_0x01=%x\r\n",mdata);
							printf("Link try:%x\r\n",tst_ctr);
            				if(tst_ctr > 0xffff)
            				{
                			printf("Link: TIMEOUT!!!\r\n\r\n");
							printf("Link status, reg_0x01=%x\r\n",mdata);
                			break;
            				}
							
        				}
        				else
        				{
            			 printf("Link status=%x\r\n",mdata);
            			loop=0;
        				}
    					}
						
						/* phy_reg4, val [14, 15:16] */
						/* phy_reg5, val [17, 18:19]  */
						SPI_Data_Array[13]=0x01;
						SPI_Data_Array[14]=0x04;
						mdata = DM9051_Read_Phy(0x04);
						SPI_Data_Array[15]= mdata >> 8;
						SPI_Data_Array[16]= mdata & 0xff;
						
						SPI_Data_Array[17]=0x05;
						mdata = DM9051_Read_Phy(0x05);
						SPI_Data_Array[18]= mdata >> 8;
						SPI_Data_Array[19]= mdata & 0xff;
						
						printf("send pkt\r\n");
						mpktlen = 0x45;
						etherdev_send(SPI_Data_Array, mpktlen);
						break;
						
						case 0x0C00:	
						printf("0C00 selection...\r\n");
						mpktlen = rxlen;
						SPI_Data_Array[13]=0x01;
						etherdev_send(SPI_Data_Array, mpktlen);
						break;


						default:
  						printf("Error selection...\r\n");
			
  						break;
						
					}
		
				  }
			         if(p> 100000000) 
				  {
					p= p % 100000000;
					y++;
				   }
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);
			DM9051_Read_Register(eDRVSPI_PORT1, DM9051_REG_MRCMDX,&Spi_Data);

		  }	
	  }
	  #endif
}

