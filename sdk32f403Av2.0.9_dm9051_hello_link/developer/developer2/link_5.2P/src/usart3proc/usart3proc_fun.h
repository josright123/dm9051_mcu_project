#ifndef __USART3PROC_FUN_H
#define __USART3PROC_FUN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

  //------------------------------------------------------------------------------
  //--- usart3 buffer ---
  //------------------------------------------------------------------------------

#define USART3_TX_BUFFER_SIZE 256
#define USART3_RX_BUFFER_SIZE 256

#define STX 0x02 // Adjust this value based on your protocol
#define ETX 0x03 // Adjust this value based on your protocol

// #define CRC16_LENGTH 2 // Length of CRC16 in bytes
#define CRC16_LENGTH (2 + 1) // Length of CRC16 in bytes + STX byte

//==============================================================================
//--- Declarative Machine State ---
//==============================================================================
// Operating system machine state
#define OSMS_PWRBEACON_STATE 1 //
#define OSMS_DATA_STATE 2
#define OSMS_REPLYRESULT_STATE 3
#define OSMS_FAIL_STATE 4

#define OSMS_WAITTING_RESULT_STATE 30

#pragma pack(push)
#pragma pack(1)
  typedef struct
  {
    uint8_t stx;
    uint16_t len;
    uint8_t op;
    uint8_t cmd;
    // uint8_t *data;
    // variable length data
    // uint8_t etx;
    // uint16_t crc16;
  } USART3_PROTOCOL_DATA, *P_USART3_PROTOCOL_DATA;

#pragma pack(pop)

  // #define MAX_BARCODE_CATEGORY			3				// Category
  // #define MAX_BARCODE_SPECIES				4				// Species
  // #define MAX_BARCODE_DASH				1				// Dash
  // #define MAX_BARCODE_IDENTIFIER			10				// Identifier
  // #define MAX_BARCODE_SCAN				(128)			// The maximum size of the scan buffer (DMC-XXXX-1234567890 / 6889420075...)
  // #define MAX_BARCODE_TAG					(4+1+10)		// The maximum  size of the tag ID (XXXX-1234567890)
  // #define MAX_BARCODE_COMMAND				(4+1+10)		// The maximum  size of the command ID. (XXXX-1234567890)
  // #define MAX_BARCODE_PRODUCT				(16)			// The maximum  size of the product ID. (1234567890...)
  // #define MAX_BARCODE_FACTORY				(16)			// The maximum  size of the command ID. (XXXX-1234567890)

  void usart3_configuration(void);
  int usart3proc_init(void);
  void usart3proc_time_event(int ms);
  void usart3proc_rx_data_interrupt(uint8_t rx_data);
  uint8_t usart3proc_tx_data_interrupt(void);
  int usart3proc_main(void);
  void SetMachineState(uint8_t uiMachineState);

#ifdef __cplusplus
}
#endif

#endif //__USART3PROC_FUN_H
