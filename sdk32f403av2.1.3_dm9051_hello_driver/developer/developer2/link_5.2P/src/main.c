/**
 **************************************************************************
 * @file     main.c
 * @brief    main program
 **************************************************************************
 *                       Copyright notice & Disclaimer
 *
 * The software Board Support Package (BSP) that is made available to
 * download from Artery official website is the copyrighted work of Artery.
 * Artery authorizes customers to use, copy, and distribute the BSP
 * software and its related documentation for the purpose of design and
 * development in conjunction with Artery microcontrollers. Use of the
 * software is governed by this copyright notice and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
 * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
 * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
 * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
 * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 *
 **************************************************************************
 */
#include "dm9051_lw.h"
#include "dm9051_lw_conf.h"
// #include "at32f435_437_board.h"
// #include "at32f435_437_clock.h"
// #include "at32f413_board.h" //"at32f403a_407_board.h"
// #include "at32f413_clock.h" //"at32f403a_407_clock.h"
// #include "at32f415_board.h"
// #include "at32f415_clock.h"

/** @addtogroup AT32F415_periph_examples
 * @{
 */

/** @addtogroup 415_USART_printf USART_printf
 * @{
 */

#include "testproc/dm9051_lw_testproc.h" //#include "dm9051_at32_decl_support.h" //#include "dm9051_at32_decl.h"

// usart3proc_fun.h
#include "usart3proc/usart3proc_fun.h"

void title(char *appname) {
  bannerline_log();
  bannerline_log();
  printf("%s start: [BOARD_SPI COUNT] %d  /  Operating: [ETHERNET COUNT] %d\r\n", appname, BOARD_SPI_COUNT, ETHERNET_COUNT);
}

//void ethernets_init(void) {
//  int i;
//  for (i = 0; i < ETHERNET_COUNT; i++) {
//	mstep_set_net_index(i); //+
//	dm9051_init(mstep_eth_mac());
//  }
//}

void drviver_init(void) {
	dm9051_init(mstep_eth_mac());
	//.display_chipmac();
}

/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int main(void)
{
  uint16_t id;
  system_clock_config();
  at32_board_init(); 
  uart_print_init(115200);
	
	
  title("hello_driver");
  //GpioDisplay();
  //ethcnt_ifdiplay_iomode();

//#if 0
//  dm9051_boards_initialize(); //of-1.12
//  ethernet_interfaces_initialize(); //of-1.12
//#endif
#if 1
  dm9051_boards_initialize(); //of-1.12
  TRANS_CONN(drviver_init, ENUM_TRANS); //ethernets_init();
#endif
	
  while(1) {
    delay_sec(1);
    //printf("usart printf counter: %u\r\n",time_cnt++);
  }	
	
	
//  usart3_configuration();    

//  testproc_board_initialize(); // dm9051_board_initialize
//  id = testproc_drv_initialize();

//  printf(": test start...\r\n");
//  
//  if (id != (DM9051_ID >> 16))
//  {
//    printf("Chip ID wrong! Check the device board!\r\n");
//    printf(": test end\r\n");
//    printf(": while(1);\r\n");
//    while (1)
//      ;
//  }
//  testproc_net_test();
//  testproc_run();
}

/**
 * @}
 */

/**
 * @}
 */
