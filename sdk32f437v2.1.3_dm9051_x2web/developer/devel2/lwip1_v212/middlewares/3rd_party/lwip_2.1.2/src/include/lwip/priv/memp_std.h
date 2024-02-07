/**
 * @file
 * lwIP internal memory pools (do not use in application code)
 * This file is deliberately included multiple times: once with empty
 * definition of LWIP_MEMPOOL() to handle all includes and multiple times
 * to build up various lists of mem pools.
 */

/*
 * SETUP: Make sure we define everything we will need.
 *
 * We have create three types of pools:
 *   1) MEMPOOL - standard pools
 *   2) MALLOC_MEMPOOL - to be used by mem_malloc in mem.c
 *   3) PBUF_MEMPOOL - a mempool of pbuf's, so include space for the pbuf struct
 *
 * If the include'r doesn't require any special treatment of each of the types
 * above, then will declare #2 & #3 to be just standard mempools.
 */
/* This treats "malloc pools" just like any other pool.
   The pools are a little bigger to provide 'size' as the amount of user data. */
#define LWIP_MALLOC_MEMPOOL(num, size) LWIP_MEMPOOL(POOL_##size, num, (size + LWIP_MEM_ALIGN_SIZE(sizeof(struct memp_malloc_helper))), "MALLOC_"#size)
#define LWIP_MALLOC_MEMPOOL_START
#define LWIP_MALLOC_MEMPOOL_END

/* This treats "pbuf pools" just like any other pool.
 * Allocates buffers for a pbuf struct AND a payload size */

/*
 * A list of internal pools used by LWIP.
 *
 * LWIP_MEMPOOL(pool_name, number_elements, element_size, pool_description)
 *     creates a pool name MEMP_pool_name. description is used in stats.c
 */
#if LWIP_RAW
LWIP_MEMPOOL(RAW_PCB,        MEMP_NUM_RAW_PCB,         sizeof(struct raw_pcb),        "RAW_PCB") //..vds.vds.
#endif /* LWIP_RAW */

#if LWIP_UDP
LWIP_MEMPOOL(UDP_PCB,        MEMP_NUM_UDP_PCB,         sizeof(struct udp_pcb),        "UDP_PCB") //..vds.vds.
#endif /* LWIP_UDP */

#if LWIP_TCP
LWIP_MEMPOOL(TCP_PCB,        MEMP_NUM_TCP_PCB,         sizeof(struct tcp_pcb),        "TCP_PCB") //..vds.vds.
LWIP_MEMPOOL(TCP_PCB_LISTEN, MEMP_NUM_TCP_PCB_LISTEN,  sizeof(struct tcp_pcb_listen), "TCP_PCB_LISTEN")
LWIP_MEMPOOL(TCP_SEG,        MEMP_NUM_TCP_SEG,         sizeof(struct tcp_seg),        "TCP_SEG")
#endif /* LWIP_TCP */

#if LWIP_IPV4 && IP_REASSEMBLY
LWIP_MEMPOOL(REASSDATA,      MEMP_NUM_REASSDATA,       sizeof(struct ip_reassdata),   "REASSDATA") //..vds.vds.
#endif /* LWIP_IPV4 && IP_REASSEMBLY */
#if (IP_FRAG && !LWIP_NETIF_TX_SINGLE_PBUF) || (LWIP_IPV6 && LWIP_IPV6_FRAG)
LWIP_MEMPOOL(FRAG_PBUF,      MEMP_NUM_FRAG_PBUF,       sizeof(struct pbuf_custom_ref),"FRAG_PBUF") //..vds.vds.
#endif /* IP_FRAG && !LWIP_NETIF_TX_SINGLE_PBUF || (LWIP_IPV6 && LWIP_IPV6_FRAG) */

#if LWIP_TIMERS && !LWIP_TIMERS_CUSTOM
LWIP_MEMPOOL(SYS_TIMEOUT,    MEMP_NUM_SYS_TIMEOUT,     sizeof(struct sys_timeo),      "SYS_TIMEOUT") //..vds.vds.
#endif /* LWIP_TIMERS && !LWIP_TIMERS_CUSTOM */

/*
 * A list of pools of pbuf's used by LWIP.
 *
 * LWIP_PBUF_MEMPOOL(pool_name, number_elements, pbuf_payload_size, pool_description)
 *     creates a pool name MEMP_pool_name. description is used in stats.c
 *     This allocates enough space for the pbuf struct and a payload.
 *     (Example: pbuf_payload_size=0 allocates only size for the struct)
 */

//#ifndef LWIP_PBUF_MEMPOOL
//#endif /* LWIP_PBUF_MEMPOOL */
#define LWIP_PBUF_MEMPOOL(name, num, payload, desc) \
	LWIP_MEMPOOL(name, num, (LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf)) + LWIP_MEM_ALIGN_SIZE(payload)), desc)
		
LWIP_MEMPOOL(PBUF,           MEMP_NUM_PBUF,            sizeof(struct pbuf),           "PBUF_REF/ROM")
LWIP_PBUF_MEMPOOL(PBUF_POOL, PBUF_POOL_SIZE,           PBUF_POOL_BUFSIZE,             "PBUF_POOL")


/*
 * REQUIRED CLEANUP: Clear up so we don't get "multiply defined" error later
 * (#undef is ignored for something that is not defined)
 */
#undef LWIP_MEMPOOL
#undef LWIP_MALLOC_MEMPOOL
#undef LWIP_MALLOC_MEMPOOL_START
#undef LWIP_MALLOC_MEMPOOL_END
#undef LWIP_PBUF_MEMPOOL
