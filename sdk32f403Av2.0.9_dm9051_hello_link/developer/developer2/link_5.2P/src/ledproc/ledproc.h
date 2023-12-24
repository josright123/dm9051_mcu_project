#ifndef __LEDPROC_H
#define __LEDPROC_H

#ifdef __cplusplus
extern "C"
{
#endif

// #define MQTT_CLIENT_LINK_TMR_MS 200
#define LED_STATUS_NONE_TMR_MS 10
#define LED_STATUS_OK_TMR_MS 1000
#define LED_STATUS_ERROR_TMR_MS 500
#define LED_STATUS_TIMEOUT_TMR_MS 200
#define LED_STATUS_CRC16_ERROR_TMR_MS 100

  typedef enum
  {
    LED_STATUS_NONE = 0,
    LED_STATUS_OK = 1,
    LED_STATUS_ERROR = 2,
    LED_STATUS_TIMEOUT = 3,
    // crc16 error
    LED_STATUS_CRC16_ERROR = 4,
  } led_status_flag_t;
  // led_status_flag_t led_status_flag;
  extern led_status_flag_t led_status_flag;

  // typedef enum
  // {
  //   LED_STATUS_OFF = 0,
  //   LED_STATUS_ON = 1,
  //   LED_STATUS_BLINK = 2,
  // } led_status_t;
  // led_status_t led_status;
  // // led_status_t led_status = LED_STATUS_OFF;

#ifdef __cplusplus
}
#endif

#endif //__LEDPROC_H
