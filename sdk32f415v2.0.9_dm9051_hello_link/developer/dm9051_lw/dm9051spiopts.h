#ifndef __BOARD_SPI_H
#define __BOARD_SPI_H

#define BOARD_SPI_COUNT						5 //3 //2 //1 //4 //3 //1 //2 //1 //3 //1 //2 //1 //x //ETH_COUNT

/* 
 * Sanity. ETHERNET COUNT is defined in "dm9051opts.h" 
 * Since the application did declare the ethernet count, firstly.
 */
#if (BOARD_SPI_COUNT < ETHERNET_COUNT)
#error "Please make sure that _BOARD_SPI_COUNT(config here) must large equal to _ETHERNET_COUNT"
#endif

#endif //__BOARD_SPI_H
