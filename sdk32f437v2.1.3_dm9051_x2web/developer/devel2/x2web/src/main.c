/**
  **************************************************************************
  * @file     main.c
  * @version  v1.0
  * @date     2023-04-28
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

//#include "at32_cm4_device_support.h" //#include "at32f435_437_board.h"
//#include "at32f435_437_clock.h"
//#include "at32f413_board.h" //"at32f403a_407_board.h"
//#include "at32f413_clock.h" //"at32f403a_407_clock.h"

#include "dm9051_lw_conf.h"
#include "dm9051_lw.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/fs.h"
#include "ethernetif.h"
//
//#include "dm9051_at32_decl.h" 
#include "lwip_initialize.h" 
#include "developer_conf.h" //#include "main.h" //#include "developer_conf.h" LWIP_UDP
#include "netconf.h"
//#include "..\..\..\lwip_dm9051_driver2303v3\debug.h" //..#include "FreeRTOS.h" (#include "debug.h")
#include "testproc/testproc_lw.h"
#include "tmr_init.h"

//....jkiuiuil.

/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 415_dm9051_example
  * @{
  */

//u32_t g_RunTime = 0; //JJ-Comp
volatile uint32_t all_local_time = 0;
//mqtt_sending_t mqttpingreq_sending_cyclictmr; //int mqtt_sending flg = 0;
int timeouts_init_num_cyclic_timers = 0; //JJ.

void time_update(void)
{
  time_update_support();
}

/*
 * Check if first/lastest topic and/(or) min/max expire-time
 */
#if MQTT_CLIENT_SUPPORT
	int min_score_times(uint16_t times, int chkindex)
	{
		int i, min_idx = 0;
		
		for (i = get_mqtt_max_publish_num() - 1; i >= 0; i--) {
			if (get_publish_disp_time(i) <= times)
				min_idx = i;
		}

		return (chkindex == min_idx) ? 1 : 0;
	}

	int max_score_times(uint16_t times, int chkindex)
	{
		int i, max_idx = 0;
		
		for (i = 0; i < get_mqtt_max_publish_num(); i++) {
			if (get_publish_disp_time(i) >= times)
				max_idx = i;
		}

		return (chkindex == max_idx) ? 1 : 0;
	}
#endif

#if 0
  __IO uint32_t time_cnt = 0;
#endif
	
void set_testplanlog(enable_t flag) {
}
	
/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();
  uart_print_init(115200);
  at32_board_init();

  bannerline();
  bannerline();
  printf("x2web start: [BOARD_SPI COUNT] %d  /  Operating: [ETHERNET COUNT] %d\r\n", BOARD_SPI_COUNT, ETHERNET_COUNT);

  dm9051_options_display();

  dm9051_boards_initialize(); //at32_dm9051_initialize();
  ethcnt_ifdiplay_iomode();

  lwip_initialize();
  tasks_base_init();
  tmr_init(); //Used for link-down/link-up check, to be the init(s) latest-one !!
  netlink_spring_wait();

  set_testplanlog(DM_TRUE); //sel DM_TRUE to have log for rx and lwip input flow
  testmode_real();
  set_testplanlog(DM_FALSE); //set_dm9051opts_testplanlog(DM_FALSE);  
  
  if (!OPT_CONFIRM(rxtypemode)) { //== (isrxtype_test())
	  printf("...................... plan a rx type test rx mode .......................\r\n");
	  testing_loop();
  }
  proc_test_plan_alldone(); // for restore to be as init.

  lwip_reinitialize();
  tasks_mach_reinit(); //net->cbf = cb;
  lwip_app_init();
 low_level_output_test();
  proc_runs();
}

/*uint8_t my_debounce = 0;

void line7(void) {
  if(exint_flag_get(EXINT_LINE_7) != RESET)
  {
	  exint_flag_clear(EXINT_LINE_7);

	  if (my_debounce == 0) {
		my_debounce = 8;
		//at32_led_toggle(LED2);
		//at32_led_toggle(LED3);
		//at32_led_on(LED4);
		at32_led_toggle(LED4);
	  }
	  
	  lwip_rx_loop_handler();
	  my_debounce += 8; // if this only, when 256, i.e. 0
	  my_debounce &= 0x3f; // when 64, i.e. 0
  }
}*/

/**
  * @}
  */

/**
  * @}
  */
