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
// include memset
#include <string.h>
#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "usart3proc_fun.h"

/** @addtogroup AT32F403A_periph_examples
 * @{
 */

/** @addtogroup 403A_USART_interrupt USART_interrupt
 * @{
 */

uint8_t usart3_tx_buffer[USART3_TX_BUFFER_SIZE];
uint8_t usart3_rx_buffer[USART3_RX_BUFFER_SIZE];
uint16_t usart2_tx_buffer_size = USART3_TX_BUFFER_SIZE;
uint16_t usart2_rx_buffer_size = USART3_RX_BUFFER_SIZE;
volatile uint16_t usart2_tx_counter = 0x00;
volatile uint16_t usart2_rx_counter = 0x00;
uint16_t usart3_rx_counter = 0;

/**
 * @brief  config usart
 * @param  none
 * @retval none
 */
void usart3_configuration(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the usart2 and gpio clock */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* enable the usart3 and gpio clock */
  crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the usart2 tx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_2;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure the usart3 tx pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_10;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure the usart2 rx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_3;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure the usart3 rx pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_11;
  gpio_init(GPIOB, &gpio_init_struct);

  /* config usart nvic interrupt */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(USART2_IRQn, 0, 0);
  nvic_irq_enable(USART3_IRQn, 0, 0);

  /* configure usart2 param */
  usart_init(USART2, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART2, TRUE);
  usart_receiver_enable(USART2, TRUE);

  /* configure usart3 param */
  usart_init(USART3, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART3, TRUE);
  usart_receiver_enable(USART3, TRUE);

  /* enable usart2 and usart3 interrupt */
  // usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE);
  // usart_enable(USART2, TRUE);

  usart_interrupt_enable(USART3, USART_RDBF_INT, TRUE);
  usart_enable(USART3, TRUE);

  //  usart_interrupt_enable(USART2, USART_TDBE_INT, TRUE);
  //  usart_interrupt_enable(USART3, USART_TDBE_INT, TRUE);
}

/**
 * @brief  compares two buffers.
 * @param  pbuffer1, pbuffer2: buffers to be compared.
 * @param  buffer_length: buffer's length
 * @retval 1: pbuffer1 identical to pbuffer2
 *         0: pbuffer1 differs from pbuffer2
 */
uint8_t buffer_compare(uint8_t *pbuffer1, uint8_t *pbuffer2, uint16_t buffer_length)
{
  while (buffer_length--)
  {
    if (*pbuffer1 != *pbuffer2)
    {
      return 0;
    }
    pbuffer1++;
    pbuffer2++;
  }
  return 1;
}

/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int usart3proc_init(void)
{
  memset((void *)&usart3_tx_buffer, 0, sizeof(usart3_tx_buffer));
  memset((void *)&usart3_rx_buffer, 0, sizeof(usart3_rx_buffer));
  at32_led_toggle(LED2);
  at32_led_toggle(LED3);
  at32_led_toggle(LED4);
  // delay_sec(1);
  printf(": usart3proc_init ...\r\n");
  return 0;
}

void usart3proc_time_event(int ms)
{
  // timeout reset the rx counter.
  usart3_rx_counter = 0;
}

void usart3proc_rx_data_interrupt(uint8_t rx_data)
{
  int rx_data_len;
  Protocol_data protocolData;

  // Store the received byte in the buffer
  usart3_rx_buffer[usart3_rx_counter++] = rx_data;

  // If the buffer is full, reset the counter
  if (usart3_rx_counter == USART3_RX_BUFFER_SIZE)
  {
    usart3_rx_counter = 0;
  }

  // (usart3_rx_buffer[1] | (usart3_rx_buffer[2] << 8)
  rx_data_len = usart3_rx_buffer[1] | (usart3_rx_buffer[2] << 8);
  // If we have received the complete protocol data (based on the length field)
  // if (usart3_rx_counter >= 3 && usart3_rx_counter == (usart3_rx_buffer[1] | (usart3_rx_buffer[2] << 8)))
  if (usart3_rx_counter >= 3 + CRC16_LENGTH && usart3_rx_counter == (usart3_rx_buffer[1] | (usart3_rx_buffer[2] << 8)) + CRC16_LENGTH)
  {
    // Check if the last character is ETX
    if (usart3_rx_buffer[rx_data_len - 1] != ETX)
    {
      // The last character is not ETX, reset the counter and return
      usart3_rx_counter = 0;
      at32_led_toggle(LED3);
      return;
    }

    // Parse the received data
    protocolData.stx = usart3_rx_buffer[0];
    protocolData.len = (usart3_rx_buffer[1] | (usart3_rx_buffer[2] << 8));
    protocolData.op = usart3_rx_buffer[3];
    protocolData.cmd = usart3_rx_buffer[4];
    protocolData.data = &usart3_rx_buffer[5]; // Assuming data starts at index 5
    protocolData.etx = usart3_rx_buffer[protocolData.len - 3];
    protocolData.crc = (usart3_rx_buffer[protocolData.len - 2] | (usart3_rx_buffer[protocolData.len - 1] << 8));

    // Process the protocol data...

    // Reset the counter for the next protocol data
    usart3_rx_counter = 0;
    at32_led_toggle(LED2);
  }
}

// int usart3proc_rx_data_interrupt(uint8_t *rx_data)
// {

// // ;PROTOCOL:STX,LEN_L,LEN_H,OP,CMD#,DATA,ETX,CRC_L,CRC_H
// // ;             |<-----LENGTH------------->|
// // ;             |<-------CRC-------------->|
// // ;********************************************************
// // STX     SET     02H
// // ETX     SET     03H

//   uint8_t rx_data_len = 0;
//   uint8_t i = 0;
//   for (i = 0; i < rx_data_len; i++)
//   {
//     usart3_rx_buffer[usart2_rx_counter++] = rx_data[i];
//     if (usart2_rx_counter == usart2_rx_buffer_size)
//     {
//       usart2_rx_counter = 0;
//     }
//   }

//   return 0;
// }
