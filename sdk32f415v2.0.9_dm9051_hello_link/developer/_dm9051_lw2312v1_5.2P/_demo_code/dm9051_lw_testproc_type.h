/*
 * dm9051_lw_testproc_type.h 
 *
 * - this is some lwip types declared, inside.
 *
 */
#ifndef __DM9051_DECL_TYPE_H
#define __DM9051_DECL_TYPE_H

#define IPADDR_NONE         				((uint32_t)0xffffffffUL)

#define ETH_HWADDR_LEN    					6
#define ETH_PAD_SIZE                    	0
#define SIZEOF_ETH_HDR						(14 + ETH_PAD_SIZE)

// [lwip isn't exist]
//.#if DEVICE_SUPPORT_LWIP_MODE == 0 
	struct eth_addr { //: lwip isn't exist (isn't used.)
	  uint8_t addr[6]; //: ETH_HWADDR_LEN
	}; 

	struct eth_hdr {
	  struct eth_addr dest;
	  struct eth_addr src;
	  uint16_t type;
	};
	struct ip4_addr {
	  uint32_t addr;
	};
	struct ip4_addr_wordaligned {
	  uint16_t addrw[2];
	};

	typedef struct ip4_addr ip4_addr_t;
	typedef ip4_addr_t ip_addr_t;

	struct etharp_hdr {
	  uint16_t hwtype;
	  uint16_t proto;
	  uint8_t  hwlen;
	  uint8_t  protolen;
	  uint16_t opcode;
	  struct eth_addr shwaddr;
	  struct ip4_addr_wordaligned sipaddr;
	  struct eth_addr dhwaddr;
	  struct ip4_addr_wordaligned dipaddr;
	};
//.#endif

// [dm9051_debug_lw.c use below uip's structure declaration]
struct uip_eth_addr { //: while lwip exist.
  uint8_t addr[6];
};
	
struct uip_eth_hdr {
  struct uip_eth_addr dest; //uip_eth_addr dest;
  struct uip_eth_addr src; //uip_eth_addr src;
  uint16_t type;
};

struct arp_hdr {
  struct uip_eth_hdr ethhdr;
  uint16_t hwtype;
  uint16_t protocol;
  uint8_t hwlen;
  uint8_t protolen;
  uint16_t opcode;
  struct uip_eth_addr shwaddr; //uip_eth_addr
  uint16_t sipaddr[2];
  struct uip_eth_addr dhwaddr; //uip_eth_addr
  uint16_t dipaddr[2];
};

/* The TCP and IP headers. */
struct uip_tcpip_hdr {
  /* IPv4 header. */
  uint8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  uint16_t ipchksum;
  uint16_t srcipaddr[2],
    destipaddr[2];
  
  /* TCP header. */
  uint16_t srcport,
    destport;
  uint8_t seqno[4],
    ackno[4],
    tcpoffset,
    flags,
    wnd[2];
  uint16_t tcpchksum;
  uint8_t urgp[2];
  uint8_t optdata[4];
};

struct uip_udpip_hdr { /* The UDP and IP headers. */
  /* IP header. */
  uint8_t vhl, tos,
    len[2], ipid[2], ipoffset[2],
    ttl, proto;
  uint16_t ipchksum;
  uint16_t srcipaddr[2],
    destipaddr[2];
  
  /* UDP header. */
  uint16_t srcport,
    destport;
  uint16_t udplen;
  uint16_t udpchksum;
};

struct uip_icmpip_hdr { /* The ICMP and IP headers. */
  /* IPv4 header. */
    uint8_t vhl, tos,
      len[2], ipid[2], ipoffset[2],
      ttl, proto;
    uint16_t ipchksum;
    uint16_t srcipaddr[2],
      destipaddr[2];
  
  /* ICMP (echo) header. */
  uint8_t type, icode;
  uint16_t icmpchksum;
  uint16_t id, seqno;
};

struct ethip_hdr {
  struct uip_eth_hdr ethhdr;
  /* IP header. */
  uint8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  uint16_t ipchksum;
  uint16_t srcipaddr[2],
    destipaddr[2];
};

#endif //__DM9051_DECL_TYPE_H
