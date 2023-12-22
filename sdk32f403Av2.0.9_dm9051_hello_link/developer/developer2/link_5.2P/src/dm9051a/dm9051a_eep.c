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
    reg = 0x0b;
    length = 1;
    // dm_rd_reg(usb_handle, reg, length, buf);
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
/*      read  EEPROM ------ E-Fuse                                       */
/*************************************************************************/
int dm9051a_rd_eep(unsigned char num)
{
  unsigned char valx;
  unsigned int datax;
  unsigned char reg;
  unsigned short length;
  unsigned char buf[SZBUF];

  valx = 0x00 | num;
  reg = 0x0c; // DM9051_EPAR
  length = 1;
  buf[0] = valx;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  reg = 0x0b;
  length = 1;
  buf[0] = 0x00;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  reg = 0x0b;
  length = 1;
  buf[0] = 0x04;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  reg = 0x0b;
  length = 1;
  buf[0] = 0x00;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  // wait_phy_end();
  if (wait_phy_end() < 0)
  {
    return -1;
  }

  reg = 0x0d;
  length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  DM9051_Read_Regnx(reg, length, buf);
  // datax = (buf[1] << 8) + buf[0];
  // *(uint16_t *)buf
  datax = *(uint16_t *)buf;

  return (datax);
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

  reg = 0x58;
  length = 1;
  buf[0] = 0x80;
  // dm_wr_reg(usb_handle, reg, length, buf);
  DM9051_Write_Regnx(reg, length, buf);

  for (i = 0; i < 24; i++)
  {
    // i = index of e-fuse word.
    datax = dm9051a_rd_eep(i);
    // printf("e-fuse[%x] = %x \r\n", i, datax);
    printf("e-fuse[%04X] = %04X \r\n", i, datax);
  }

#if 0
  reg = 0x58; length=1;
  buf[0]=0x00;
  dm_wr_reg(usb_handle, reg, length, buf);
#endif
}

/*************************************************************************/
/*   show chip status                                                    */
/*************************************************************************/
void dm9051a_show_status()
{
  unsigned char reg;
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
  reg = 0x10;
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
  reg = 0x10;
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
