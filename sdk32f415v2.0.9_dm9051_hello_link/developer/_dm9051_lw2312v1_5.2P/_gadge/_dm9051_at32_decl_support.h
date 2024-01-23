/**
  **************************************************************************
  * @file     dm9051_at32_decl_support.h
  * @version  v1.0
  * @date     2023-11-06
  * @brief    header file of dm9051 environment config program.
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
#ifndef __DM9051_DECL_SUPPORT_H
#define __DM9051_DECL_SUPPORT_H

//for 'main'
//...static void test_dm9051_init_dual(uint8_t *adr);
//...void dm9051_board_initialize(void);

//for 'ethernetif', lwip support
int DBG_IS_ARP(void *dataload);
int DBG_IS_TCP(void *dataload);
void debug_tx(const uint8_t *buf, uint16_t len);
void debug_rx(const uint8_t *buf, uint16_t len);

#endif //__DM9051_DECL_SUPPORT_H
