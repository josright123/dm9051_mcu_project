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
#include "testproc/testproc_lw.h" //#include "testproc/dm9051_lw_testproc.h"
//#include "at32f435_437_board.h"
//#include "at32f435_437_clock.h"
//#include "at32f413_board.h" //"at32f403a_407_board.h"
//#include "at32f413_clock.h" //"at32f403a_407_clock.h"
//#include "at32f415_board.h"
//#include "at32f415_clock.h"

/** @addtogroup AT32F415_periph_examples
  * @{
  */

/** @addtogroup 415_USART_printf USART_printf
  * @{
  */

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  //uint16_t id;
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);

  testproc_board_initialize(); //_dm9051_board_initialize
  //id = 
  testproc_drv_initialize();
  in_time_link_up_found();
//testproc_net_test();
  testproc_run();
}

/**
  * @}
  */

/**
  * @}
  */
