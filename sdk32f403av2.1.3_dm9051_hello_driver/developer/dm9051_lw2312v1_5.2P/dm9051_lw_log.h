/**
  **************************************************************************
  * @file     _dm9051_lw_log.h
  * @version  v1.0
  * @date     2023-04-28
  * @brief    header file of dm9051 environment config program.
  **************************************************************************
  */
#ifndef __DM9051_DBG_H
#define __DM9051_DBG_H

typedef struct {
	int allow_num;
} tx_monitor_t;

#define TX_MODLE_DECLARATION \
const tx_monitor_t tx_modle = { \
	16, /*10,*/ \
}; \
const tx_monitor_t tx_all_modle = { \
	5, \
}

/* Put here, instead. Instead of "dm9051_lw_log.h", some generic called in the other implementation place.
 */
#if HELLO_DRIVER_API
#if DM9051_DEBUG_ENABLE == 1
void dm9051_rxlog_ack(void *payload, uint16_t tot_len, char *result_str);
void dm9051_rxlog_arp(void *payload, uint16_t tot_len, uint16_t len);
void dm9051_txlog_disp(uint8_t *buffer, int len);
void dm9051_txlog_monitor_tx(int hdspc, const uint8_t *buffer, size_t len);
#else
#define dm9051_rxlog_ack( payload,  tot_len,  result_str)
#define dm9051_rxlog_arp( payload,  tot_len,  len)
#define dm9051_txlog_disp(buffer,  len)
#define dm9051_txlog_monitor_tx( hdspc,   buffer,  len)
#endif
#endif

#if DM9051_DEBUG_ENABLE == 1
//void dm9051_log_dump0(char *prefix_str, size_t tlen, const void *buf, size_t len);
void dm9051_rxlog_monitor_rx(int hdspc, const uint8_t *buffer, size_t len);
void dm9051_txlog_monitor_tx_all(int hdspc, const uint8_t *buffer, size_t len);
void dm9051_rxlog_monitor_rx_all(int hdspc, const uint8_t *buffer, size_t len);
#define bannerline_log()   printf("\r\n")
#endif

#if DM9051_DEBUG_ENABLE == 0
//.#define dm9051_log_dump0(prefix_str, tlen, buf, len)
#define dm9051_rxlog_monitor_rx( hdspc,   buffer,  len)
#define dm9051_txlog_monitor_tx_all( hdspc,   buffer,  len)
#define dm9051_rxlog_monitor_rx_all( hdspc,   buffer,  len)
#define bannerline_log()
#endif

#endif //__DM9051_DBG_H
