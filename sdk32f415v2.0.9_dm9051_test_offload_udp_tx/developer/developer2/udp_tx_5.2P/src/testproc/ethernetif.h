#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"

#define OVERSIZE_LEN			PBUF_POOL_BUFSIZE //#define _PBUF_POOL_BUFSIZE 1514 //defined in "lwipopts.h" (JJ20201006)
#define RXBUFF_OVERSIZE_LEN		(OVERSIZE_LEN+2)

uint8_t *get_rx_buffer(void);
uint8_t *get_tx_buffer(void);

extern const uint8_t buffer_discover[384];

typedef enum {
	OUT_NORMAL = 0,
	OUT_TEST_UDP = 1,
	OUT_TEST_TCP = 2
} outpacket_t;

err_t out_packet(outpacket_t opt, uint8_t *buffer, uint16_t l, const uint8_t *update_buffer, uint16_t upate_l);

//#include "at32f435_437.h" //"at32f413.h" //#include "at32f415.h"

void dm9051_init_dual(uint8_t *adr);
uint16_t dm9051_link_update_dual(void);
void dm9051_tx_dual(uint8_t *buf, uint16_t len);
uint16_t dm9051_rx_dual(uint8_t *buff);

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

err_t ethernetif_inp(struct netif *netif);

void lwip_set_mac(unsigned char* macadd);
void lwip_get_mac(char *adr);

void line7_proc(void);
void reset_proc(void);

#endif
