/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt
 *
 */
#ifndef LWIP_HDR_LWIPOPTS_H
#define LWIP_HDR_LWIPOPTS_H

#define NOT_lwip_init_SYS_TIMEOUTS_INIT()
#define PUT_proc_runs_init_SYS_TIMEOUTS_INIT()	sys_timeouts_init()

#define LWIP_HTTPD_CUSTOM_FILES       	  0
#define HTTPD_JJ_DYNAMIC_FSDATA			  1 //1
#define HTTPD_USE_CUSTOM_FSDATA			  1 //0 //1

#define LWIP_IPV4			1



#define MQTT_REQ_MAX_IN_FLIGHT            25 //24 //.12 //.8
#define MQTT_VAR_HEADER_BUFFER_LEN 		 (128 + 135 + 18) //JJ (max incoming topic length(128) + max payload length(128/135) + 8(18))
#define MQTT_OUTPUT_RINGBUF_SIZE		 (256 + 55 + 18) //org 256, JJ.to 329
//#define SYS_LIGHTWEIGHT_PROT             0
//#define NO_SYS                           1
/* We link to special sys_arch.c (for basic non-waiting API layers unit tests) */
#define NO_SYS                           1

#define SYS_LIGHTWEIGHT_PROT             0
#define LWIP_NETCONN                     !NO_SYS
#define LWIP_SOCKET                      !NO_SYS
#define LWIP_NETCONN_FULLDUPLEX          LWIP_SOCKET
#define LWIP_NETBUF_RECVINFO             1
#define LWIP_HAVE_LOOPIF                 1
#define LWIP_NETIF_LINK_CALLBACK         1

#define LWIP_DHCP                        0 //1 //1

#define TCP_MSS                          (1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           		 4
//#define MEM_SIZE                         (13*1024) //.(20*1024)
//#define MEM_SIZE                         ((12*1024)+1024) //!512 !256
#define MEM_SIZE                         ((12*1024)+1024-(1*1024)-1341) //!512 !256

#define TCP_SND_QUEUELEN                 (6 * TCP_SND_BUF)/TCP_MSS
#define MEMP_NUM_TCP_SEG                 TCP_SND_QUEUELEN

#define TCP_SND_BUF                     (2 * TCP_MSS)
#define TCP_WND                         (4 * TCP_MSS)

#define LWIP_WND_SCALE                   0
#define TCP_RCV_SCALE                    0

//#define LWIP_TESTMODE                   0
#define LWIP_TESTMODE					 1 //.JJ in developping, the used code style
#define LWIP_TESTMODE_REAL				 0 
											//if, Davicom (can RST such way!)
											//else, _CH390 (can not as RST above way!)
											//1 //0 //.JJ must exactly reflect the true function's version.

#define LWIP_DEBUG_TIMERNAMES			 LWIP_DBG_ON //LWIP_DBG_OFF //LWIP_DBG_ON
/* 'PBUF_POOL_BUFSIZE': the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_SIZE                   7 //.10 /* pbuf tests need ~200KByte */
#define PBUF_POOL_BUFSIZE                (1514+4) //.2000	//.2000(tested) //dm,jj=1514

#define MEMP_NUM_UDP_PCB                4 //(3) //[JJ todo, 4 --> (3)]  (elem-size 40)
#define MEMP_NUM_TCP_PCB                5 //(3) //[JJ do reduce. 5 --> (3)] (elem-size 156)
#define MEMP_NUM_TCPIP_MSG_API			5 //[JJ want reduce.] (NO USED, ANY-CHANGE TO IT IS NO EFFECT.)

/* ---------- TCP options ---------- */
#define LWIP_TCP                         1
#define TCP_TTL                          255

#define LWIP_IGMP                        0 //1
#define LWIP_MDNS_RESPONDER              1
#define LWIP_NUM_NETIF_CLIENT_DATA       (LWIP_MDNS_RESPONDER)

#define ETHARP_SUPPORT_STATIC_ENTRIES    1

#define TMR_PHY_LNK_NUM_JJ				 2 //1
#define TMR_PUBLISH_NUM_JJ				 0
#define TMR_MQTT_CYC_NUM_JJ				 0

/* (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 8) */
#define MEMP_NUM_SYS_TIMEOUT             (LWIP_NUM_SYS_TIMEOUT_INTERNAL + TMR_PHY_LNK_NUM_JJ + TMR_PUBLISH_NUM_JJ + TMR_MQTT_CYC_NUM_JJ)

#define MIB2_STATS                       1

#define LWIP_MEM_ILLEGAL_FREE(msg)       /* to nothing */

#define MQTT_DEBUG                       LWIP_DBG_OFF

#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0 
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
  /* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               1
#endif

#endif /* LWIP_HDR_LWIPOPTS_H */
