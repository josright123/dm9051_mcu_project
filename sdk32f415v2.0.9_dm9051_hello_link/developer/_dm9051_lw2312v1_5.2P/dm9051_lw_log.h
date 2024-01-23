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

/* Put here, instead. Instead of "dm9051_lw_log.h", some generic called in the other implementation place.
 */
void dm9051_log_dump0(const char *prefix_str, size_t tlen, const void *buf, size_t len);
void dm9051_txlog_monitor_tx_all(int hdspc, const uint8_t *buffer, size_t len);
void dm9051_txlog_disp(uint8_t *buffer, int len);
void dm9051_rxlog_arp(void *payload, uint16_t tot_len, uint16_t len);
void dm9051_rxlog_ack(void *payload, uint16_t tot_len, char *result_str);
	
#endif //__DM9051_DBG_H
