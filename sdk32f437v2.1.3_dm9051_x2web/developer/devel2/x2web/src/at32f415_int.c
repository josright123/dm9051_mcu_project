/**
  **************************************************************************
  * @file     at32f415_int.c
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
#include "at32f415_int.h"
#include "developer_conf.h" //#include "main.h" 
//#include "netconf.h"
#include "dm9051_lw.h"
#include "dm9051_lw_conf.h"

/** @addtogroup AT32F415_periph_examples
  * @{
  */

/** @addtogroup 415_USART_printf
  * @{
  */
  
/*__IO */ //extern u32 g_RunTime; //JJ-Comp

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
	//g_RunTime++;
}

/**
  * @brief  this function handles timer6 overflow handler.
  * @param  none
  * @retval none
  */
#include "at32f415_board.h" //"at32f403a_407_board.h"

//extern u32 lwip_sys_now; //jj-temp-extern

#if 1 //to-research
//:TMR3_GLOBAL_IRQHandler
//:TMR4_GLOBAL_IRQHandler
void TMR3_GLOBAL_IRQHandler(void) {
	printf("1 times - TMR3 control\r\n");
}
void TMR4_GLOBAL_IRQHandler(void) {
	printf("1 times - TMR4 control\r\n");
}
#endif

//:TMR5_GLOBAL_IRQHandler
//~TMR6_GLOBAL_IRQHandler
//:TMR1_OVF_TMR10_IRQHandler
#if 1
void TMR5_GLOBAL_IRQHandler(void) //Accept TMR5, NOT startup_at32f415.s ; TMR6 (Porting by JJ, as 'TMR6')
{
  if(tmr_flag_get(TMR5, TMR_OVF_FLAG) != RESET) //TMR6
  {
    time_update(); /* Update the all_local_time by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
    tmr_flag_clear(TMR5, TMR_OVF_FLAG); //TMR6
  }
}
#endif
#if 1
void TMR1_OVF_TMR10_IRQHandler(void)
{
  if(tmr_flag_get(TMR1, TMR_OVF_FLAG) == SET)
  {
    time_update(); /* Update the all_local_time by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
    tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
}
#endif

void EXINT9_5_IRQHandler(void)
{
  dm9051_irqlines_proc();
}


/**
  * @}
  */

/**
  * @}
  */
