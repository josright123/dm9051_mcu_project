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

#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"

#include "dm9051_lw_conf.h"
#include "dm9051_lw.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_USART_printf USART_printf
  * @{
  */

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
}

/**
  * @}
  */

/**
  * @}
  */
