/*==============================================================================
|                     dm9k_uip.h: Version 0.04 06/13/2006                      |
|                                                                              |
|          Davicom DM9000A uP NIC fast Ethernet driver for uIP.                |
|                                                                              |
|    This program is free software; you can redistribute it and/or modify it   |
|under the terms of the GNU General Public License as published by the Free    |
|Software Foundation; either version 2 of the License, or (at your option) any |
|later version.                                                                |
|                                                                              |
| This program is distributed in the hope that it will be useful,but WITHOUT   |
|ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS |
|FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.|
|                                                                              |
|    (C)Copyright 1997-1998 DAVICOM Semiconductor,Inc. All Rights Reserved.    |
|============================================================================= |
| China Agent:                        |||�j���N�z�ӡG                          |
|   AXW Technology Limited.           |||  �R�Y���ަ������q                  |
|   TEL : +86 (755) 8368-8556         |||  �q�ܡG+86 (755) 8368-8556           |
|         +86 (755) 8368-8967         |||        +86 (755) 8368-8967           |
|   FAX : +86 (755) 8377-8765         |||  �ǯu�G+86 (755) 8377-8765           |
|   MAIL: sales@axwdragon.com         |||  �l�q�Gsales@axwdragon.com           |
==============================================================================*/

#ifndef _DM9K_H_
#define _DM9K_H_

/* DM9000 REGISTER LIST */
#define DM9000_REG_NCR        0x00
#define DM9000_REG_NSR        0x01
#define DM9000_REG_TCR        0x02
#define DM9000_REG_TSR1       0x03
#define DM9000_REG_TSR2       0x04
#define DM9000_REG_RCR        0x05
#define DM9000_REG_RSR        0x06
#define DM9000_REG_ROCR       0x07
#define DM9000_REG_BPTR       0x08
#define DM9000_REG_FCTR       0x09
#define DM9000_REG_FCR        0x0A
#define DM9000_REG_EPCR       0x0B
#define DM9000_REG_EPAR       0x0C
#define DM9000_REG_EPDRL      0x0D
#define DM9000_REG_EPDRH      0x0E
#define DM9000_REG_WAR        0x0F
#define DM9000_REG_PAR        0x10
#define DM9000_REG_MAR        0x16
#define DM9000_REG_GPCR       0x1E
#define DM9000_REG_GPR        0x1F
#define DM9000_REG_VID_L      0x28
#define DM9000_REG_VID_H      0x29
#define DM9000_REG_PID_L      0x2A
#define DM9000_REG_PID_H      0x2B
#define DM9000_REG_CHIPR      0x2C
#define DM9000_REG_TCR2       0x2D
#define DM9000_REG_OTCR       0x2E
#define DM9000_REG_SMCR       0x2F
#define DM9000_REG_ETXCSR     0x30
#define DM9000_REG_TCSCR      0x31
#define DM9000_REG_RCSCSR     0x32
#define DM9000_REG_INT_CON    0x39
#define DM9051_REG_PPCSR      0x3D
#define DM9051_REG_MBS        0x5D  //Stone add for Memory BIST test!
#define DM9051_REG_MRCMDX     0x70
#define DM9051_REG_MRCMD      0x72
#define DM9051_REG_MRRL       0x74
#define DM9051_REG_MRRH       0x75
#define DM9051_REG_MWCMDX     0x76
#define DM9051_REG_MWCMD      0x78
#define DM9051_REG_MWRL       0x7A
#define DM9051_REG_MWRH       0x7B
#define DM9051_REG_TXPLL      0x7C
#define DM9051_REG_TXPLH      0x7D
#define DM9051_REG_ISR        0x7E
#define DM9051_REG_IMR        0x7F


/* �������]�m */
#define DM9000_PRODUCT        0x9000
#define DM9000_VENDER         0x0A46
#define DM9000_BYTE_MODE      0x01
#define DM9000_WORD_MODE      0x00
#define DM9000_PHY            0x40
#define DM9000_PKT_RDY        0x01
#define DM9000_PKT_NORDY      0x00                

#define DM9000_RX_INTR        0x01                /* �������_�P�_ bit */
#define DM9000_TX_INTR        0x02                /* �ǰe���_�P�_ bit */
#define DM9000_OVERFLOW_INTR  0x04                /* ���s���X���_�P�_ bit */
#define DM9000_LINK_CHANG     0x20                /* �s���ܰʤ��_�P�_ bit */

#define DM9000_PHY_ON        0x00                /* �]�w PHY �}�� */
#define DM9000_PHY_OFF        0x01                /* �]�w PHY ���� */
//#define DM9000_RCR_SET        0x31                /* �]�w �����\�� (���� CRC �� �W���]) */
#define DM9000_RCR_SET        0x81
#define DM9000_TCR_SET        0x01                /* �]�w �ǰe�\�� */
#define DM9000_RCR_OFF        0x00                /* �]�w �����\���������]�m */
#define DM9000_BPTR_SET       0x37                /* �]�w Back Pressure ����]�m */
#define DM9000_FCTR_SET       0x38                /* �]�w Flow Control ����]�m */
#define DM9000_TCR2_SET       0x80                /* �]�m LED ��ܼҦ� */
#define DM9000_OTCR_SET       0x80                /* �]�m DM9000 �u�@�W�v 0x80 = 100Mhz */
#define DM9000_ETXCSR_SET     0x83                /* �]�m Early Tramsmit ����]�m */
#define DM9000_FCR_SET        0x28                /* �}�� �����y���\��]�m */
#define DM9000_TCSCR_SET      0x07                /* �]�w CHECKSUM �ǰe�B�� �]�m */
#define DM9000_RCSCSR_SET     0x03                /* �]�w CHECKSUM �����ˬd �]�m */
#define DM9000_IMR_SET        0x81                /* �]�w �ҥΤ��_�ϯ� ����]�m */
#define DM9000_IMR_OFF        0x80                /* �]�w �������_�ϯ� ����]�m */

#define DM9000_REG_RESET     0x01

#define UIP_ETHADDR0 0x00
#define UIP_ETHADDR1 0x60
#define UIP_ETHADDR2 0x6e
#define UIP_ETHADDR3 0x90
#define UIP_ETHADDR4 0x00
#define UIP_ETHADDR5 0xae

/*=============================================================================
  �t�Υ��쪺�]�m��
  =============================================================================*/
#undef    UINT8
#define   UINT8					uint8_t
#undef    UINT16
#define   UINT16				uint16_t
#undef    UINT32
#define   UINT32				uint32_t

//
#define SPI_WR_BURST		0xF8;
#define SPI_RD_BURST		0x72;
//
#define SPI_READ				0x03
#define SPI_WRITE        0x04
#define  SPI_WRITE_BUFFER  0x05        // Send a series of bytes from the
                                       // Master to the Slave
#define  SPI_READ_BUFFER   0x06        // Send a series of bytes from the Slave
                                       // to the Master
                                       
#define MAX_BUFFER_SIZE    1600           // Maximum buffer Master will send

#define SZBUF  100

extern uint8_t SPI_Data_Array[];			//Packet or memory share buffer
extern  uint8_t  fifo_over_err;
extern uint32_t crc_err;
/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void etherdev_init(void);
void etherdev_send(uint8_t*, uint32_t);
unsigned short etherdev_read(void);
//unsigned short etherdev_poll(void);
void etherdev_chkmedia(void);

/*** M08G16 ***/
//void DM9051_Read_Reg(E_DRVSPI_PORT, uint32_t, uint32_t*);
//void DM9051_Write_Reg(E_DRVSPI_PORT, uint32_t, uint32_t*);
//void DM9051_Write_Mem(E_DRVSPI_PORT, uint8_t*, uint32_t);
//void DM9051_Read_Mem(E_DRVSPI_PORT, uint8_t*, uint32_t);
//UINT16 DM9051_Read_Phy(uint32_t);
void wait_phy_end(void);

void  Interrupt_initial_setting(void);
void  Interrupt_disable_setting(void);
int ether_crc(int , unsigned char*);
void  Timer0_initial_setting(void);
void TMR0_Handler(uint32_t Timer0_Data);
#endif /* _DM9K_H_ */
