// #include "at32f403a_407_board.h"
// #include "at32f403a_407_clock.h"
#include "dm9051_lw.h"      //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "dm9051a_eep.h"    //#include "dm9051a_eep.h"

/*************************************************************************/
/*   show chip status                                                    */
/*************************************************************************/
void show_status()
{
  unsigned char reg;
  // unsigned short length;
  unsigned char buf[SZBUF];

  /* read register , with error report*/
  /*
      reg = 0x2a; length=1;
      retval = dm_rd_reg(usb_handle, reg, length, buf);
      if (retval<0) printf("%s\n", usb_strerror());
      else printf("retval=%d, reg 0x%02x = 0x%02x\n", retval, reg, buf[0]);
  */

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
  DM9051_Read_Regnx(0x28, 2, buf);
  printf(": REG29_28=%04X\r\n", *(uint16_t *)buf);

  // reg = 0x40;
  // length = 5;
  // dm_rd_reg(usb_handle, reg, length, buf);
  // printf("  REG40-44=%2x,%2x,%2x,%2x,%2x",
  //        buf[0], buf[1], buf[2], buf[3], buf[4]);
  // printf("\n");

  // reg = 0x1e;
  // length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  // printf("  REG1E=%2x", buf[0]);
  // printf("  REG1F=%2x", buf[1]);

  // MAC address = 0x00:0x60:0x6e:0x55:0x01:0x25  // original
  // Physical Address Registers (PARs) 0x10~0x15
  reg = 0x10;
  // length = 6;
  DM9051_Read_Regnx(reg, 6, buf);
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
  DM9051_Write_Regnx(reg, 6, (uint8_t *)MACaddr);

  // Physical Address Registers (PARs) 0x10~0x15
  DM9051_Read_Regnx(reg, 6, buf);
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

  // reg = 0x20;
  // length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  // printf("REG21_20(TX_WR)=%2x", buf[1]);
  // if (buf[0] > 15)
  //   printf("%2x", buf[0]);
  // else
  //   printf("0%1x", buf[0]);

  // reg = 0x24;
  // length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  // printf("   REG25_24(RX_WR)=%2x", buf[1]);
  // if (buf[0] > 15)
  //   printf("%2x", buf[0]);
  // else
  //   printf("0%1x", buf[0]);
  // printf("\n");

  // reg = 0x22;
  // length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  // printf("REG23_22(TX_RD)=%2x", buf[1]);
  // if (buf[0] > 15)
  //   printf("%2x", buf[0]);
  // else
  //   printf("0%1x", buf[0]);

  // reg = 0x26;
  // length = 2;
  // dm_rd_reg(usb_handle, reg, length, buf);
  // printf("   REG27_26(RX_RD)=%2x", buf[1]);
  // if (buf[0] > 15)
  //   printf("%2x", buf[0]);
  // else
  //   printf("0%1x", buf[0]);
  printf("\n");
}
