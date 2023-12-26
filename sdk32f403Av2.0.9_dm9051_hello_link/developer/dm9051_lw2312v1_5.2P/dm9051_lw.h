/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __LWIP_DM9051_H
#define __LWIP_DM9051_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>	 
//#include "dm9051opts.h" //driver attributes

#define RX_POOL_BUFSIZE	(1514+4)
	 
//#include "lwip/opt.h" //use 'PBUF_POOL_BUFSIZE' (why? moved!!)

//.#include "lwip/opt.h" //use 'PBUF_POOL_BUFSIZE'
//#define DM9051_DEFAULT_BUFSIZE	(1514+4)
//#ifndef PBUF_POOL_BUFSIZE
//#define PBUF_POOL_BUFSIZE		DM9051_DEFAULT_BUFSIZE
//#endif

#define OPC_REG_W       0x80  // Register Write
#define OPC_REG_R       0x00  // Register Read
	 
/* Private constants ---------------------------------------------------------------------------------------*/
#define DM9051_PHY          (0x40)    		/* PHY address 0x01 */

//#define PHY_ADDRESS       (0x01)       	/*!< relative to at32 board */
#define PHY_STATUS_REG      (0x01)       	/*!< basic mode status register */
#define PHY_LINKED_BIT   	(0x0004)     	/*!< link status */

/* Exported constants --------------------------------------------------------------------------------------*/ 
#define DM9051_ID           (0x90510A46)		/* DM9051A ID                                               */
#define DM9051_PKT_MAX      (1536)          /* Received packet max size                                     */
#define DM9051_PKT_RDY      (0x01)          /* Packet ready to receive                                      */

#define DM9051_NCR          (0x00)
#define DM9051_NSR          (0x01)
#define DM9051_TCR          (0x02)
#define DM9051_TSR1         (0x03)
#define DM9051_TSR2         (0x04)
#define DM9051_RCR          (0x05)
#define DM9051_RSR          (0x06)
#define DM9051_ROCR         (0x07)
#define DM9051_BPTR         (0x08)
#define DM9051_FCTR         (0x09)
#define DM9051_FCR          (0x0A)
#define DM9051_EPCR         (0x0B)
#define DM9051_EPAR         (0x0C)
#define DM9051_EPDRL        (0x0D)
#define DM9051_EPDRH        (0x0E)
#define DM9051_WCR          (0x0F)

#define DM9051_PAR          (0x10)
#define DM9051_MAR          (0x16)

#define DM9051_GPCR         (0x1e)
#define DM9051_GPR          (0x1f)
#define DM9051_TRPAL        (0x22)
#define DM9051_TRPAH        (0x23)
#define DM9051_RWPAL        (0x24)
#define DM9051_RWPAH        (0x25)

#define DM9051_VIDL         (0x28)
#define DM9051_VIDH         (0x29)
#define DM9051_PIDL         (0x2A)
#define DM9051_PIDH         (0x2B)

#define DM9051_CHIPR        (0x2C)
#define DM9051_TCR2         (0x2D)
#define DM9051_OTCR         (0x2E)
#define DM9051_SMCR         (0x2F)

#define DM9051_ETCR         (0x30)    /* early transmit control/status register                             */
#define DM9051_CSCR         (0x31)    /* check sum control register                                         */
#define DM9051_RCSSR        (0x32)    /* receive check sum status register                                  */

#define DM9051_PBCR			(0x38)
#define DM9051_INTR			(0x39)
#define DM9051_PPCR			(0x3D)
#define DM9051_MPCR			(0x55)
#define DM9051_LMCR			(0x57)
#define DM9051_MBNDRY		(0x5E)
#define DM9051_MRCMDX       (0x70)
#define DM9051_MRCMD        (0x72)
#define DM9051_MRRL         (0x74)
#define DM9051_MRRH         (0x75)
#define DM9051_MWCMDX       (0x76)
#define DM9051_MWCMD        (0x78)
#define DM9051_MWRL         (0x7A)
#define DM9051_MWRH         (0x7B)
#define DM9051_TXPLL        (0x7C)
#define DM9051_TXPLH        (0x7D)
#define DM9051_ISR          (0x7E)
#define DM9051_IMR          (0x7F)

#define CHIPR_DM9051A       (0x19)
#define CHIPR_DM9051B       (0x1B)

#define DM9051_REG_RESET    (0x01)
#define DM9051_IMR_OFF      (0x80)
#define DM9051_TCR2_SET     (0x90)	/* set one packet */
#define DM9051_RCR_SET      (0x31)
#define DM9051_BPTR_SET     (0x37)
#define DM9051_FCTR_SET     (0x38)
#define DM9051_FCR_SET      (0x28)
//#define DM9051_TCR_SET    (0x01)


#define NCR_EXT_PHY         (1 << 7)
#define NCR_WAKEEN          (1 << 6)
#define NCR_FCOL            (1 << 4)
#define NCR_FDX             (1 << 3)
#define NCR_LBK             (3 << 1)
#define NCR_RST             (1 << 0)
#define NCR_DEFAULT			(0x0)    /* Disable Wakeup */

#define NSR_SPEED           (1 << 7)
#define NSR_LINKST          (1 << 6)
#define NSR_WAKEST          (1 << 5)
#define NSR_TX2END          (1 << 3)
#define NSR_TX1END          (1 << 2)
#define NSR_RXOV            (1 << 1)
#define NSR_CLR_STATUS		(NSR_WAKEST | NSR_TX2END | NSR_TX1END)

#define TCR_TJDIS           (1 << 6)
#define TCR_EXCECM          (1 << 5)
#define TCR_PAD_DIS2        (1 << 4)
#define TCR_CRC_DIS2        (1 << 3)
#define TCR_PAD_DIS1        (1 << 2)
#define TCR_CRC_DIS1        (1 << 1)
#define TCR_TXREQ           (1 << 0) /* Start TX */
#define TCR_DEFAULT			(0x0)

#define TSR_TJTO            (1 << 7)
#define TSR_LC              (1 << 6)
#define TSR_NC              (1 << 5)
#define TSR_LCOL            (1 << 4)
#define TSR_COL             (1 << 3)
#define TSR_EC              (1 << 2)

#define RCR_WTDIS           (1 << 6)
#define RCR_DIS_LONG        (1 << 5)
#define RCR_DIS_CRC         (1 << 4)
#define RCR_ALL             (1 << 3)
#define RCR_RUNT            (1 << 2)
#define RCR_PRMSC           (1 << 1)
#define RCR_RXEN            (1 << 0)
#define RCR_DEFAULT			(RCR_DIS_LONG | RCR_DIS_CRC)

#define RSR_RF              (1 << 7)
#define RSR_MF              (1 << 6)
#define RSR_LCS             (1 << 5)
#define RSR_RWTO            (1 << 4)
#define RSR_PLE             (1 << 3)
#define RSR_AE              (1 << 2)
#define RSR_CE              (1 << 1)
#define RSR_FOE             (1 << 0)

#define EPCR_REEP           (1<<5)
#define EPCR_WEP            (1<<4)
#define EPCR_EPOS           (1<<3)
#define EPCR_ERPRR          (1<<2)
#define EPCR_ERPRW          (1<<1)
#define EPCR_ERRE           (1<<0)

#define INTR_ACTIVE_LOW		(1 << 0)

#define BPTR_DEFAULT		(0x3f)
#define FCTR_DEAFULT		(0x38)
#define FCR_DEFAULT			(0xFF)
#define SMCR_DEFAULT		(0x0)
#define PBCR_MAXDRIVE		(0x44)

/* 0x3D */
/* Pause Packet Control Register - default = 1 */
#define PPCR_PAUSE_COUNT	0x08

/* 0x57 */
/* LEDMode Control Register - LEDMode1 */
/* Value 0x81 : bit[7] = 1, bit[2] = 0, bit[1:0] = 01b */
#define LMCR_NEWMOD			(1 << 7)
#define LMCR_TYPED1			(1 << 1)
#define LMCR_TYPED0			(1 << 0)
#define LMCR_MODE1			(LMCR_NEWMOD | LMCR_TYPED0)

/* 0x5E */
#define MBNDRY_WORD			0
#define MBNDRY_BYTE			(1 << 7)

#define IMR_PAR             (1 << 7)
#define IMR_PRM             (1 << 0)

#define LEN64				64	//To observ the tcp/ip packets which as 64-bytes

//uint8_t DM9051_Read_Reg(uint8_t reg);
//void DM9051_Write_Reg(uint8_t reg, uint8_t val);
//uint8_t DM9051_Read_Mem2X(void);
//void DM9051_Read_Mem(uint8_t *buf, uint16_t len);
//void DM9051_Write_Mem(uint8_t *buf, uint16_t len);
//uint16_t phy_read(uint16_t uReg);
//void phy_write(uint16_t reg, uint16_t value); //[function "phy_write" was available but could never referenced.]

void dm9051_poweron_rst(void);
uint16_t dm9051_init(const uint8_t *adr);
void dm9051_start(const uint8_t *adr);
uint16_t dm9051_rx(uint8_t *buff);
void dm9051_tx(uint8_t *buf, uint16_t len);
void dm9051_reset_process(void);

uint16_t read_chip_id(void);
uint16_t dm9051_bmsr_update(void);
uint16_t dm9051_link_update(void);
uint16_t dm9051_link_show(void);

#define DM9051_FLAG_LINK_UP				0x01U

#define dm9051_set_flags(flg, set_flags)     do { flg = (u8)(flg | (set_flags)); } while(0)
#define dm9051_clear_flags(flg, clr_flags)   do { flg = (u8)(flg & (u8)(~(clr_flags) & 0xff)); } while(0)
#define dm9051_is_flag_set(flg, flag)        ((flg & (flag)) != 0)

#define chipid_on_pin_code_log_s(str, fstr)	printf("...%s...          Read Chip ID: %02x\r\n", fstr, id)
#define chipid_on_pin_code_log_err(str)	printf(":  %s, Read Chip ID error!\r\n", str)

void display_verify_chipid(char *str, char *spiname, uint16_t id);

/*
 * export open functions
 */
//void poweron_pin_rst(void);
//#define spi_read_reg cspi_read_reg
//#define spi_write_reg cspi_write_reg
//uint8_t spi_read_mem2x(void);
//void spi_read_mem(u8 *buf, u16 len);
//void spi_write_mem(u8 *buf, u16 len);

#ifdef __cplusplus
}
#endif

#endif //__LWIP_DM9051_H
