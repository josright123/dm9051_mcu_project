/**
  **************************************************************************
  * @file     at32f435_437_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f435_437_int.h"
#include "developer_conf.h" //#include "main.h"
//#include "netconf.h" //_line7()
#include "dm9051_lw_conf.h"
//#include "dm9051_lw.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_SPI_fullduplex_polling
  * @{
  */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* go to infinite loop when hard fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* go to infinite loop when memory manage exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* go to infinite loop when bus fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* go to infinite loop when usage fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
}

/**
  * @brief  this function handles timer1 overflow handler.
  * @param  none
  * @retval none
  */
#if 1
void TMR1_OVF_TMR10_IRQHandler(void)
{
  if(tmr_flag_get(TMR1, TMR_OVF_FLAG) == SET)
  {
    /* add user code..., at32_led_toggle(LED3); */
    time_update(); /* Update the all_local_time by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
    tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
}
#endif
#if 0
//void TMR5_GLOBAL_IRQHandler(void) //Accept TMR5, NOT startup_at32f415.s ; TMR6 (Porting by JJ, as 'TMR6')
//{
//  if(tmr_flag_get(TMR5, TMR_OVF_FLAG) == SET) //TMR6
//  {
//    time_update(); /* Update the all_local_time by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
//    tmr_flag_clear(TMR5, TMR_OVF_FLAG); //TMR6
//  }
//}
#endif

void EXINT9_5_IRQHandler(void) {
	dm9051_irqlines_proc(); //line7();
}

/**
  * @}
  */

/**
  * @}
  */


