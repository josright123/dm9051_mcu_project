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
#include <stdint.h>
#include <string.h>
#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "tmr_init.h"
#include "usart3proc_fun.h"
// "dm9051a_eep.h"
#include "../dm9051a/dm9051a_eep.h"

/** @addtogroup AT32F403A_periph_examples
 * @{
 */

/** @addtogroup 403A_USART_interrupt USART_interrupt
 * @{
 */

static uint32_t current_timeout_due_time;

uint8_t usart3_tx_buffer[USART3_TX_BUFFER_SIZE];
uint8_t usart3_rx_buffer[USART3_RX_BUFFER_SIZE];
uint8_t usart3_data_buffer[USART3_RX_BUFFER_SIZE];
uint16_t usart3_tx_buffer_size = USART3_TX_BUFFER_SIZE;
uint16_t usart3_rx_buffer_size = USART3_RX_BUFFER_SIZE;
uint16_t usart3_tx_length = 0;
uint16_t usart3_data_buffer_length = 0;
volatile uint16_t usart3_tx_counter = 0x00;
volatile uint16_t usart3_rx_counter = 0x00;

static uint8_t m_uiMachineState;

// usart3_rx 接收資料完成標誌
// typedef enum usart3_rx_complete_flag ,ok = 1, error = 0, timeout = 2
typedef enum
{
  USART3_RX_COMPLETE_NONE = 0,
  USART3_RX_COMPLETE_OK = 1,
  USART3_RX_COMPLETE_ERROR = 2,
  USART3_RX_COMPLETE_TIMEOUT = 3,
  // crc16 error
  USART3_RX_COMPLETE_CRC16_ERROR = 4,
} usart3_rx_complete_flag;

usart3_rx_complete_flag usart3_rx_complete_status = USART3_RX_COMPLETE_NONE;

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

  // Initial Variables
  SetMachineState(OSMS_PWRBEACON_STATE); // Initial state

  // delay_sec(1);
  printf(": usart3proc_init ...\r\n");
  return 0;
}

void usart3proc_time_event(int ms)
{
  // timeout reset the rx counter.
  // if (usart3_rx_counter > 0 && usart3_rx_counter < USART3_RX_BUFFER_SIZE && ms > 1000)
  if (usart3_rx_counter > 0 && usart3_rx_counter < USART3_RX_BUFFER_SIZE && sys_now() > current_timeout_due_time)
  {
    usart3_rx_counter = 0;
    usart3_rx_complete_status = USART3_RX_COMPLETE_TIMEOUT;
  }
}

// Function to calculate CRC16
uint16_t calculate_crc16(const uint8_t *data, size_t length)
{
  uint16_t crc = 0xFFFF; // Initial value
  for (size_t i = 0; i < length; ++i)
  {
    crc ^= data[i];
    for (int j = 0; j < 8; ++j)
    {
      crc = (crc & 1) ? ((crc >> 1) ^ 0xA001) : (crc >> 1);
    }
  }
  return crc;
}

void SetMachineState(uint8_t uiMachineState)
{
  m_uiMachineState = uiMachineState;
}

void usart3proc_rx_data_interrupt(uint8_t rx_data)
{
  uint16_t rx_data_len;
  P_USART3_PROTOCOL_DATA pProtocolData = (P_USART3_PROTOCOL_DATA)usart3_rx_buffer;
  // variable length data
  //  pProtocolData->data = &usart3_data_buffer[5];

  // If this is the first byte we are receiving, set the timeout due time
  if (usart3_rx_counter == 0)
  {
    current_timeout_due_time = sys_now() + 1000; // Set the due time to 1 second from now
  }

  // Store the received byte in the buffer
  usart3_rx_buffer[usart3_rx_counter++] = rx_data;

  // If the buffer is full, reset the counter
  if (usart3_rx_counter == USART3_RX_BUFFER_SIZE)
  {
    usart3_rx_counter = 0;
  }

  // rx_data_len = usart3_rx_buffer[1] | (usart3_rx_buffer[2] << 8);
  rx_data_len = pProtocolData->len;

  // If we have received the complete protocol data (based on the length field)
  // if (usart3_rx_counter >= 3 + CRC16_LENGTH && usart3_rx_counter == (usart3_rx_buffer[1] | (usart3_rx_buffer[2] << 8)) + CRC16_LENGTH)
  if (usart3_rx_counter >= 3 + CRC16_LENGTH && usart3_rx_counter == rx_data_len + CRC16_LENGTH)
  {
    // Check if the last character is ETX
    // if (usart3_rx_buffer[rx_data_len] != ETX)

    if (*(usart3_rx_buffer + rx_data_len) != ETX)
    {
      // The last character is not ETX, reset the counter and return
      usart3_rx_counter = 0;
      usart3_rx_complete_status = USART3_RX_COMPLETE_ERROR;
      // at32_led_toggle(LED3);
      return;
    }

    // Reset the counter for the next protocol data
    // usart3_rx_counter = 0;

    memcpy((void *)&usart3_data_buffer, (void *)&usart3_rx_buffer, usart3_rx_counter);
    usart3_data_buffer_length = usart3_rx_counter;

    // test dm9051a show_status
    show_status();

    usart3_rx_counter = 0;
    usart3_rx_complete_status = USART3_RX_COMPLETE_OK;
    // at32_led_toggle(LED2);
  }
}

uint8_t usart3proc_tx_data_interrupt(void)
{
  if (usart3_tx_counter < usart3_tx_length)
  {
    usart_data_transmit(USART3, usart3_tx_buffer[usart3_tx_counter++]);
  }
  else
  {
    usart_interrupt_enable(USART3, USART_TDBE_INT, FALSE);
    usart3_tx_counter = 0;
    usart3_tx_length = 0;
  }
  return 0;
}

/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int usart3proc_main(void)
{
  uint16_t rx_data_len;
  uint16_t data_crc16;
  uint16_t calculated_crc;

  // USART3_PROTOCOL_DATA protocolData;
  P_USART3_PROTOCOL_DATA pProtocolData = (P_USART3_PROTOCOL_DATA)usart3_data_buffer;

  // // Initial Variables
  // SetMachineState(OSMS_PWRBEACON_STATE); // Initial state

  // 加入 switch case m_uiMachineState Machine State
  switch (m_uiMachineState)
  {
  case OSMS_PWRBEACON_STATE:
    printf(": OSMS_PWRBEACON_STATE...\r\n");
    SetMachineState(OSMS_DATA_STATE);
    break;

  case OSMS_DATA_STATE:
    // printf(": OSMS_DATA_STATE...\r\n");
    // SetMachineState(OSMS_REPLYRESULT_STATE);

    switch (usart3_rx_complete_status)
    {
    case USART3_RX_COMPLETE_OK:
      usart3_rx_complete_status = USART3_RX_COMPLETE_NONE;
      rx_data_len = pProtocolData->len;

      data_crc16 = usart3_data_buffer[rx_data_len + 1] | (usart3_data_buffer[rx_data_len + 2] << 8);
      calculated_crc = calculate_crc16((uint8_t *)usart3_data_buffer + 1, rx_data_len);

      printf(": calculated_crc: %X \r\n", calculated_crc);

      if (calculated_crc == data_crc16)
      {
        // CRC16 is valid, process the data
        // CRC16 is correct, process the data
        usart3_tx_length = usart3_data_buffer_length;
        memcpy((void *)&usart3_tx_buffer, (void *)&usart3_data_buffer, usart3_data_buffer_length);
        usart_interrupt_enable(USART3, USART_TDBE_INT, TRUE);
        //      usart3_rx_counter = 0;
        at32_led_toggle(LED2);
        // printf(": calculated_crc OK...\r\n");
        printf(": USART3_RX_COMPLETE_OK...\r\n");
      }
      else
      {
        // CRC16 is invalid, handle the error
        usart3_tx_length = usart3_data_buffer_length;
        memcpy((void *)&usart3_tx_buffer, (void *)&usart3_data_buffer, usart3_data_buffer_length);
        usart_interrupt_enable(USART3, USART_TDBE_INT, TRUE);
        usart3_rx_counter = 0;
        at32_led_toggle(LED2);
        usart3_rx_complete_status = USART3_RX_COMPLETE_CRC16_ERROR;
        printf(": calculated_crc ERROR...\r\n");
      }

      break;

    case USART3_RX_COMPLETE_ERROR:
      // usart3_rx_complete_status = USART3_RX_COMPLETE_NONE;
      // at32_led_toggle(LED2);
      at32_led_toggle(LED3);
      delay_ms(200);
      printf(": USART3_RX_COMPLETE_ERROR...\r\n");
      break;

    case USART3_RX_COMPLETE_TIMEOUT:
      // usart3_rx_complete_status = USART3_RX_COMPLETE_NONE;
      at32_led_toggle(LED2);
      at32_led_toggle(LED3);
      at32_led_toggle(LED4);
      delay_ms(500);
      printf(": USART3_RX_COMPLETE_TIMEOUT...\r\n");
      break;

    case USART3_RX_COMPLETE_CRC16_ERROR:
      // usart3_rx_complete_status = USART3_RX_COMPLETE_NONE;
      at32_led_toggle(LED2);
      at32_led_toggle(LED3);
      at32_led_toggle(LED4);
      delay_ms(100);
      printf(": USART3_RX_COMPLETE_CRC16_ERROR...\r\n");
      break;

    default:
      // Handle any other cases if needed
      break;
    }
    // case OSMS_DATA_STATE: end
    break;

  case OSMS_REPLYRESULT_STATE:
    printf(": OSMS_REPLYRESULT_STATE...\r\n");
    SetMachineState(OSMS_PWRBEACON_STATE);

    // case OSMS_REPLYRESULT_STATE: end
    break;

  case OSMS_FAIL_STATE:
    printf(": OSMS_FAIL_STATE...\r\n");
    SetMachineState(OSMS_PWRBEACON_STATE);

    // case OSMS_FAIL_STATE: end
    break;

  default:

    break;
  }

  return 0;
}
