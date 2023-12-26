// #include "at32f403a_407_board.h"
// #include "at32f403a_407_clock.h"
#include "dm9051_lw.h"      //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "dm9051a_eep.h"    //#include "dm9051a_eep.h"

/*************************************************************************/
/*      wait end of PHY operation                                        */
/*************************************************************************/
int8_t wait_phy_end()
{
  int loop;
  unsigned long tst_ctr;
  unsigned char reg;
  unsigned short length;
  unsigned char buf[SZBUF];

  tst_ctr = 0;
  loop = 1;
  while (loop)
  {
    reg = 0x0b; // DM9051_EPCR
    length = 1;
    // dm_rd_reg(usb_handle, reg, length, buf);
    // EEPROM Access Status or PHY Access Status Register (EPCR) bit 0 = 0, EEPROM operation is complete.
    DM9051_Read_Regnx(reg, length, buf);
    if (buf[0] & 0x01)
    {
      /* printf("EEP status=%x\n",buf[0]); */
      tst_ctr++;
      if (tst_ctr > 0x7fff)
      {
        printf("EEP: TIMEOUT!!!\n\n");
        return (-1);
      }
    }
    else
    {
      /* printf("EEP status=%x\n",buf[0]); */
      loop = 0;
    }
  }
  return (0);
}

/*************************************************************************/
/*      write EEPROM ------ E-Fuse                                       */
/*************************************************************************/
void dm9051a_wr_eep(uint8_t addr, uint16_t wdata)
{
  uint8_t reg;
  uint8_t length;
  uint8_t buf[SZBUF];

  reg = 0x0c; // DM9051_EPAR
  length = 1;
  buf[0] = addr;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(DM9051_EPAR, length, buf);

  reg = 0x0d; // DM9051_EPDRL
  length = 2;
  // buf[0] = valx;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, (uint8_t *)&wdata);

  reg = 0x0b; // DM9051_EPCR
  length = 1;
  buf[0] = 0x12; // write eeprom enable, write eeprom start
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(DM9051_EPCR, length, buf);

  wait_phy_end();

  reg = 0x0b; // DM9051_EPCR
  length = 1;
  buf[0] = 0x00; // write eeprom disable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(DM9051_EPCR, length, buf);
}

/*************************************************************************/
/*      read  EEPROM ------ E-Fuse                                       */
/*************************************************************************/
uint16_t dm9051a_rd_eep(unsigned char addr)
{
  unsigned int datax;
  unsigned char reg;
  unsigned short length;
  unsigned char buf[SZBUF];

  reg = 0x0c; // DM9051_EPAR
  length = 1;
  buf[0] = addr;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(DM9051_EPAR, length, buf);

  reg = 0x0b; // DM9051_EPCR
  length = 1;
  buf[0] = 0x04; // read eeprom enable, EPCR_ERPRR=1 , EPCR_EPOS=0
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(DM9051_EPCR, length, buf);

  wait_phy_end();

  reg = 0x0b; // DM9051_EPCR
  length = 1;
  buf[0] = 0x00;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(DM9051_EPCR, length, buf);

  reg = 0x0d; // DM9051_EPDRL
  length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  DM9051_Read_Regnx(DM9051_EPDRL, length, buf);

  // datax = (buf[1] << 8) + buf[0];
  // *(uint16_t *)buf
  datax = *(uint16_t *)buf;

  return (datax);
}

/*************************************************************************/
/*      write e-fuse data (MAC Address)                                   */
/*************************************************************************/
uint16_t dm9051a_wr_e_fuse(unsigned int addr, unsigned int wdata)
{
  unsigned int datax;
  unsigned char reg;
  unsigned short length;
  unsigned char buf[SZBUF];
  unsigned char reg_num;

  // Power on e-fuse, write enable, power down phy chip
  // reg = 0x1f;    // General Purpose Register 1 (GPR)
  // length=1;
  // buf[0]=0x01; // power down PHY chip
  // dm_wr_reg(usb_handle, reg, length, buf);

  reg = 0x58; // e-fuse control register
  length = 1;
  buf[0] = 0x88; // write e-fuse enable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  reg_num = addr & 0xff;
  // wr_eep(reg_num, wdata);
  dm9051a_wr_eep(reg_num, wdata);

  delay_ms(4);

  reg = 0x58; // e-fuse control register
  length = 1;
  buf[0] = 0x80; // read e-fuse enable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  // datax = rd_eep(reg_num);
  datax = dm9051a_rd_eep(reg_num);

  printf("e-fuse[%02X] = %04X \r\n", addr, datax);

#if 1
  reg = 0x58; // e-fuse control register
  length = 1;
  buf[0] = 0x00; // read e-fuse disable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);
#endif

  // Power off e-fuse, write disable, power up phy chip
  // reg = 0x1f;    // General Purpose Register 1 (GPR)
  // length=1;
  // buf[0]=0x00; // power up PHY chip
  // dm_wr_reg(usb_handle, reg, length, buf);
  return datax;
}

/*************************************************************************/
/*      read e-fuse data one word                                        */
/*************************************************************************/
void dm9051a_rd_e_fuse(unsigned char addr)
{
  unsigned int datax;
  unsigned char reg;
  unsigned short length;
  unsigned char buf[SZBUF];

  reg = 0x58; // e-fuse control register
  length = 1;
  buf[0] = 0x80; // read e-fuse enable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  // datax = rd_eep(reg_num);
  datax = dm9051a_rd_eep(addr);

  printf("e-fuse[%02X] = %04X \r\n", addr, datax);

  reg = 0x58; // e-fuse control register
  length = 1;
  buf[0] = 0x00; // read e-fuse disable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);
}

/*************************************************************************/
/*      read e-fuse data N bytes                                       */
/*************************************************************************/
void dm9051a_read_e_fuse_nbytes(uint8_t start_addr, uint8_t length, uint8_t *buf)
{
  unsigned int datax;
  unsigned char reg;
  unsigned char write_buf[2];

  reg = 0x58;          // e-fuse control register
  write_buf[0] = 0x80; // read e-fuse enable
  DM9051_Write_Regnx(reg, 1, write_buf);

  // bytes to words conversion for length
  for (uint8_t i = 0; i < length; i++)
  {
    uint8_t addr = start_addr + i;
    datax = dm9051a_rd_eep(addr);
    *(uint16_t *)(buf + i * 2) = datax;

    // buf[i * 2] = (uint8_t)(datax & 0xFF);
    // buf[i * 2 + 1] = (uint8_t)((datax >> 8) & 0xFF);
    printf("e-fuse[%02X] = %04X \r\n", addr, datax);
  }

#if 1
  reg = 0x58; // e-fuse control register
  // length=1;
  buf[0] = 0x00; // read e-fuse disable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, 1, buf);
#endif
}

/*************************************************************************/
/*      read e-fuse data N words                                         */
/*************************************************************************/
void dm9051a_read_e_fuse_nwords(uint8_t start_addr, uint8_t length, uint16_t *buf)
{
  unsigned int datax;
  unsigned char reg;
  unsigned char write_buf[2];

  reg = 0x58;          // e-fuse control register
  write_buf[0] = 0x80; // read e-fuse enable
  DM9051_Write_Regnx(reg, 1, write_buf);

  // bytes to words conversion for length
  for (uint8_t i = 0; i < length; i++)
  {
    uint8_t addr = start_addr + i;
    datax = dm9051a_rd_eep(addr);
    *(buf + i) = datax;
    printf("e-fuse[%02X] = %04X \r\n", addr, datax);
  }

  reg = 0x58;          // e-fuse control register
  uint8_t zero = 0x00; // read e-fuse disable
  DM9051_Write_Regnx(reg, 1, &zero);
}

/*************************************************************************/
/*      read e-fuse data (MAC Address)                                   */
/*************************************************************************/
void dm9051a_show_e_fuse()
{
  int i;
  unsigned int datax;
  unsigned char reg;
  unsigned short length;
  unsigned char buf[SZBUF];

  reg = 0x58; // e-fuse control register
  length = 1;
  buf[0] = 0x80; // read e-fuse enable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  for (i = 0; i < 24; i++)
  {
    // i = index of e-fuse word.
    datax = dm9051a_rd_eep(i);
    // printf("e-fuse[%x] = %x \r\n", i, datax);
    printf("e-fuse[%04X] = %04X \r\n", i, datax);
  }

#if 1
  reg = 0x58; // e-fuse control register
  length = 1;
  buf[0] = 0x00; // read e-fuse disable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, 1, buf);
#endif
}

/*************************************************************************/
/*      write e-fuse data N bytes                                        */
/*************************************************************************/
// 以2的倍數(word)寫入 e-fuse, start_addr = 0x00, length = 24
int8_t dm9051a_write_e_fuse_nbytes(uint8_t start_addr, uint8_t length, uint8_t *buf)
{
  // Power on e-fuse, write enable, power down phy chip
  // reg = 0x1f;    // General Purpose Register 1 (GPR)
  // length=1;
  // buf[0]=0x01; // power down PHY chip
  // dm_wr_reg(usb_handle, reg, length, buf);

  unsigned char write_buf[SZBUF];
  unsigned char reg = 0x58; // e-fuse control register
  write_buf[0] = 0x88;      // write e-fuse enable
  // DM9051_Write_Regnx(reg, length, write_buf);
  DM9051_Write_Regnx(reg, 1, write_buf);

  for (int i = 0; i < (length / 2); i++)
  {
    // dm9051a_wr_eep(start_addr + i, *(uint16_t *)(buf + i * 2));
    printf("e-fuse[%02X] = %04X \r\n", start_addr + i, *(uint16_t *)(buf + i * 2));
    dm9051a_wr_eep(start_addr + i, *(uint16_t *)(buf + (i * 2)));
    delay_ms(4);
  }

#if 1
  reg = 0x58; // e-fuse control register
  // length=1;
  buf[0] = 0x00; // write e-fuse disable
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, 1, buf);
#endif

  // Power off e-fuse, write disable, power up phy chip
  // reg = 0x1f;    // General Purpose Register 1 (GPR)
  // length=1;
  // buf[0]=0x00; // power up PHY chip
  // dm_wr_reg(usb_handle, reg, length, buf);

  return 0;
}

/*************************************************************************/
/*      write e-fuse data N words                                        */
/*************************************************************************/
int8_t dm9051a_write_e_fuse_nwords(uint8_t start_addr, uint8_t length, uint16_t *buf)
{
  // Power on e-fuse, write enable, power down phy chip
  // reg = 0x1f;    // General Purpose Register 1 (GPR)
  // length=1;
  // buf[0]=0x01; // power down PHY chip
  // dm_wr_reg(usb_handle, reg, length, buf);

  unsigned char write_buf[SZBUF];
  unsigned char reg = 0x58; // e-fuse control register
  write_buf[0] = 0x88;      // write e-fuse enable
  DM9051_Write_Regnx(reg, 1, write_buf);

  for (int i = 0; i < length; i++)
  {
    uint16_t data = *(buf + i);
    printf("e-fuse[%02X] = %04X \r\n", start_addr + i, data);
    dm9051a_wr_eep(start_addr + i, data);
    delay_ms(4);
  }

  reg = 0x58;          // e-fuse control register
  uint8_t zero = 0x00; // write e-fuse disable
  DM9051_Write_Regnx(reg, 1, &zero);

  // Power off e-fuse, write disable, power up phy chip
  // reg = 0x1f;    // General Purpose Register 1 (GPR)
  // length=1;
  // buf[0]=0x00; // power up PHY chip
  // dm_wr_reg(usb_handle, reg, length, buf);

  return 0;
}

/*************************************************************************/
/*   show chip status                                                    */
/*************************************************************************/
void dm9051a_show_status()
{
  // unsigned char reg;
  // unsigned short length;
  unsigned char buf[SZBUF];

  // **** Chip ID
  // reg = 0x2a;
  // length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  // printf("REG2B_2A=%2x", buf[1]);
  // if (buf[0] > 15)
  //   printf("%2x", buf[0]);
  // else
  //   printf("0%1x", buf[0]);

  uint16_t id = read_chip_id();
  printf(": Chip ID: %04x\r\n", id);

  // VID = 0x0a46, PID = 0x0901
  // reg = 0x28;
  // length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  // printf("  REG29_28=%2x", buf[1]);
  // if (buf[0] > 15)
  //   printf("%2x", buf[0]);
  // else
  //   printf("0%1x", buf[0]);

  // reg = 0x28;
  // buf[0] = DM9051_Read_Reg(reg);
  // reg = 0x29;
  // buf[1] = DM9051_Read_Reg(reg);
  // printf("REG29_28=%04X", (uint16_t)(buf[1] << 8 | buf[0]));

  // reg = 0x28; 0x46, low byte, VID = 0x0A46
  // reg = 0x29; 0x0A, high byte, 0x0a
  // buf[0] = DM9051_Read_Reg(0x28); // low byte
  // buf[1] = DM9051_Read_Reg(0x29); // high byte
  // // printf(": REG29_28=%04X\r\n", (uint16_t)(buf[1] << 8 | buf[0]));
  // printf(": REG29_28=%04X\r\n", *(uint16_t *)buf);

  // DM9051_Read_Regnx(reg, length, buf);
  // DM9051_Read_Regnx(0x28, 2, buf);
  // DM9051_VIDL register name is 0x28.
  DM9051_Read_Regnx(DM9051_VIDL, 2, buf);
  printf(": REG29_28=%04X\r\n", *(uint16_t *)buf);

  // MAC address = 0x00:0x60:0x6e:0x55:0x01:0x25  // original
  // Physical Address Registers (PARs) 0x10~0x15
  // reg = 0x10;
  // length = 6;
  DM9051_Read_Regnx(DM9051_PAR, 6, buf);
  printf(": REG10~15=");
  for (int i = 0; i < 6; i++)
  {
    printf("%02X", buf[i]);
    if (i != 5)
    {
      printf(":");
    }
  }
  printf("\r\n");

  // Test MAC address = 0xF0, 0x60, 0xFE, 0xF5, 0xF1, 0xF5
  const uint8_t MACaddr[6] = {0xF0, 0x60, 0xFE, 0xF5, 0xF1, 0xF5};
  // const uint8_t MACaddr[6] = {0x00, 0x60, 0x6e, 0x55, 0x01, 0x25};
  // Physical Address Registers (PARs) 0x10~0x15
  // reg = 0x10;
  DM9051_Write_Regnx(DM9051_PAR, 6, (uint8_t *)MACaddr);

  // Physical Address Registers (PARs) 0x10~0x15
  DM9051_Read_Regnx(DM9051_PAR, 6, buf);
  printf(": REG10~15=");
  for (int i = 0; i < 6; i++)
  {
    printf("%02X", buf[i]);
    if (i != 5)
    {
      printf(":");
    }
  }
  printf("\r\n");
}
