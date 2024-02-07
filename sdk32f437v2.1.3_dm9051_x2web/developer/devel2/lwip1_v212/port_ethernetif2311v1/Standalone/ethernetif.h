#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"

//#include "at32f435_437.h" //"at32f413.h" //#include "at32f415.h"

void dm9051_init_nondual(void); //dm9051_init_dual(void);
uint16_t dm9051_link_update_dual(void);
void dm9051_tx_dual(uint8_t *buf, uint16_t len);
uint16_t dm9051_rx_dual(uint8_t *buff);

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

err_t ethernetif_inp(struct netif *netif);
void low_level_output_test(void); //test.

void lwip_set_mac(const uint8_t* macadd);
void lwip_get_mac(uint8_t *adr);

void ethernetif_line7_proc(void);
void ethernetif_reset_proc(void);

uint8_t *get_rx_buffer(void);
uint8_t *get_tx_buffer(void);

typedef enum {
	OUT_NORMAL = 0,
	OUT_TEST_UDP = 1,
	OUT_TEST_TCP = 2
} outpacket_t;

err_t out_packet(outpacket_t opt, uint8_t *buffer, uint16_t l, const uint8_t *update_buffer, uint16_t upate_l);
#endif
