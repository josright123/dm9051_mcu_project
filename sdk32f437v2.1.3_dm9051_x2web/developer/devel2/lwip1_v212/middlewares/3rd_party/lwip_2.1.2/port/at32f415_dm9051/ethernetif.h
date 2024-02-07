#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"

/* phy status register */
//#define PHY_LINKED_STATUS_BIT            (0x0004)     /*!< link status */

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

//struct netif *ethernetif_register(void);
//int ethernetif_poll(void);

void lwip_set_mac(unsigned char* macadd);
void lwip_get_mac(char *adr);

//.void ethernetif_initialize(void); //ethernetif_create(void);

#ifdef SERVER

//#define MAC_ADDR0 0x00
//#define MAC_ADDR1 0x00
//#define MAC_ADDR2 0x00
//#define MAC_ADDR3 0x00
//#define MAC_ADDR4 0x00
//#define MAC_ADDR5 0x01

#else

//#define MAC_ADDR0 0x00
//#define MAC_ADDR1 0x00
//#define MAC_ADDR2 0x00
//#define MAC_ADDR3 0x00
//#define MAC_ADDR4 0x00
//#define MAC_ADDR5 0x03

#endif

#endif
