/**
  **************************************************************************
  * @file     dm9051_env.c ,dm9051_config.c ,at32_emac.c
  * @version  v1.0
  * @date     2023-04-28, 2023-10-17, 2023-10-24
  * @brief    dm9051 config program
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
//#include "at32_cm4_device_support.h" //#include "at32f413_board.h" //"at32f403a_407_board.h"
//#include <stdio.h>

//#include "dm9051_lw.h"
#include "dm9051_lw_conf.h"

/**
  * @brief  initialize !tmr6 --> tmr5 for emac
  * @param  none
  * @retval none
  */
#if 1
void at32_dm9051_tmr_init(void)
{
  /* tmr5 configuration */
  crm_clocks_freq_type crm_clocks_freq_struct = {0};
  crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE); //CRM_TMR6_PERIPH_CLOCK

  #if 0
  printf("\r\n");
  printf(":  dm9051_tmr_init()...\r\n");
  #endif
  
  crm_clocks_freq_get(&crm_clocks_freq_struct);
  
  /* time base configuration */
  /* systemclock/24000/100 = 100hz */
  tmr_base_init(TMR5, 99, (crm_clocks_freq_struct.ahb_freq / 10000) - 1); //TMR6
  tmr_cnt_dir_set(TMR5, TMR_COUNT_UP); //TMR6

  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR5, TMR_OVF_INT, TRUE); //TMR6

  /* tmr1 overflow interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR5_GLOBAL_IRQn, 0, 0); //TMR6_GLOBAL_IRQn
  tmr_counter_enable(TMR5, TRUE); //TMR6
}
#endif
#if 0
void at32_dm9051_tmr_init(void)
{
  /* tmr1 configuration */
  crm_clocks_freq_type crm_clocks_freq_struct = {0};
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE); /* enable tmr1 clock */

  printf("\r\n");
  printf("dm9051_tmr_init()\r\n");
  crm_clocks_freq_get(&crm_clocks_freq_struct);
  
  /* time base configuration */
  /* systemclock/24000/100 = 100hz */
  tmr_base_init(TMR1, 99, (crm_clocks_freq_struct.apb2_freq * 2 / 10000) - 1); //TMR1
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE); //TMR1

  /* tmr1 hall interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 1, 0); //TMR1_OVF_TMR10_IRQn
  tmr_counter_enable(TMR1, TRUE); //TMR1
}
#endif

uint32_t lwip_sys_now = 0;

uint32_t
sys_now(void)
{
  return lwip_sys_now;
}

//extern uint32_t lwip_sys_now;

//[Timer][Basic](lwip and mqtt)
/**
  * @brief  updates the system local time
  * @param  none
  * @retval none
  */
#define SYSTEMTICK_PERIOD_MS             	10
void time_update_support(void)
{
  lwip_sys_now += SYSTEMTICK_PERIOD_MS;
}

void time_update(void)
{
  time_update_support();
}
