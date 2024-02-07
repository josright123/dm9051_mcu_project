/**
 * @file
 * Dynamic pool memory manager
 *
 * lwIP has dedicated pools for many structures (netconn, protocol control blocks,
 * packet buffers, ...). All these pools are managed here.
 *
 * @defgroup mempool Memory pools
 * @ingroup infrastructure
 * Custom memory pools

 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include <string.h>

/* Make sure we include everything we need for size calculation required by memp_std.h */
#include "lwip/pbuf.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/altcp.h"
#include "lwip/ip4_frag.h"
#include "lwip/netbuf.h"
#include "lwip/api.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/priv/api_msg.h"
#include "lwip/priv/sockets_priv.h"
#include "lwip/etharp.h"
#include "lwip/igmp.h"
#include "lwip/timeouts.h"
/* needed by default MEMP_NUM_SYS_TIMEOUT */
#include "netif/ppp/ppp_opts.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/priv/nd6_priv.h"
#include "lwip/ip6_frag.h"
#include "lwip/mld6.h"

#if 0 //.
//.#define LWIP_MEMPOOL(name,num,size,desc) LWIP_MEMPOOL_DECLARE(name,num,size,desc)
//.#include "lwip/priv/memp_std.h"
#else
#define LWIP_MEMPOOL(name,num,size,desc) LWIP_MEMPOOL_DECLARE(name,num,size,desc)

LWIP_MEMPOOL(RAW_PCB,        MEMP_NUM_RAW_PCB,         sizeof(struct raw_pcb),        "RAW_PCB") //..vds.vds.
LWIP_MEMPOOL(UDP_PCB,        MEMP_NUM_UDP_PCB,         sizeof(struct udp_pcb),        "UDP_PCB") //..vds.vds.
LWIP_MEMPOOL(TCP_PCB,        MEMP_NUM_TCP_PCB,         sizeof(struct tcp_pcb),        "TCP_PCB") //..vds.vds.
LWIP_MEMPOOL(TCP_PCB_LISTEN, MEMP_NUM_TCP_PCB_LISTEN,  sizeof(struct tcp_pcb_listen), "TCP_PCB_LISTEN")
LWIP_MEMPOOL(TCP_SEG,        MEMP_NUM_TCP_SEG,         sizeof(struct tcp_seg),        "TCP_SEG")
LWIP_MEMPOOL(REASSDATA,      MEMP_NUM_REASSDATA,       sizeof(struct ip_reassdata),   "REASSDATA") //..vds.vds.
LWIP_MEMPOOL(FRAG_PBUF,      MEMP_NUM_FRAG_PBUF,       sizeof(struct pbuf_custom_ref),"FRAG_PBUF") //..vds.vds.
	
//= LWIP_MEMPOOL(SYS_TIMEOUT,    MEMP_NUM_SYS_TIMEOUT,     sizeof(struct sys_timeo),      "SYS_TIMEOUT") //..vds.vds.
//= #define LWIP_MEMPOOL_DECLARE(name,num,size,desc)
  LWIP_DECLARE_MEMORY_ALIGNED(
		memp_memory_SYS_TIMEOUT_base, 
		(MEMP_NUM_SYS_TIMEOUT * (MEMP_SIZE + MEMP_ALIGN_SIZE(sizeof(struct sys_timeo))))
	);
    
  LWIP_MEMPOOL_DECLARE_STATS_INSTANCE(memp_stats_SYS_TIMEOUT) 
    
  static struct memp *memp_tab_SYS_TIMEOUT; 
    
  /**
	* const char *desc;
	* struct stats_mem *stats;
	* u16_t size;
	* u16_t num;
	* u8_t *base;
	* struct memp **tab;
	*/
  const struct memp_desc memp_SYS_TIMEOUT = { 
    DECLARE_LWIP_MEMPOOL_DESC("SYS_TIMEOUT") 
    LWIP_MEMPOOL_DECLARE_STATS_REFERENCE(memp_stats_SYS_TIMEOUT) 
    LWIP_MEM_ALIGN_SIZE(sizeof(struct sys_timeo)), 
    MEMP_NUM_SYS_TIMEOUT, 
    memp_memory_SYS_TIMEOUT_base, 
    &memp_tab_SYS_TIMEOUT
  };
  
//#define LWIP_PBUF_MEMPOOL(name, num, payload, desc) \
//	LWIP_MEMPOOL(name, num, (LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf)) + LWIP_MEM_ALIGN_SIZE(payload)), desc)
LWIP_MEMPOOL(PBUF,           MEMP_NUM_PBUF,            sizeof(struct pbuf),           "PBUF_REF/ROM")
//= LWIP_PBUF_MEMPOOL(PBUF_POOL, PBUF_POOL_SIZE,           PBUF_POOL_BUFSIZE,             "PBUF_POOL")
LWIP_MEMPOOL(PBUF_POOL,		PBUF_POOL_SIZE,			  (LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf)) + \
													   LWIP_MEM_ALIGN_SIZE(PBUF_POOL_BUFSIZE)),		"PBUF_POOL")
#undef LWIP_MEMPOOL
#endif

const struct memp_desc *const memp_pools[MEMP_MAX] = {
#define LWIP_MEMPOOL(name,num,size,desc) &memp_ ## name,
#include "lwip/priv/memp_std.h"
};

#if MEMP_MEM_MALLOC && MEMP_OVERFLOW_CHECK >= 2
#undef MEMP_OVERFLOW_CHECK ................rgjkbbjhntr......................................
/* MEMP_OVERFLOW_CHECK >= 2 does not work with MEMP_MEM_MALLOC, use 1 instead */
#define MEMP_OVERFLOW_CHECK 1
#endif

/**
 * @param desc pool to initialize
 */
void
memp_init_pool(const struct memp_desc *desc)
{
#if MEMP_MEM_MALLOC
//.  LWIP_UNUSED_ARG(desc); .........fdgdgeeee......
#else
#endif /* !MEMP_MEM_MALLOC */
	
  int i;
  struct memp *memp;

  *desc->tab = NULL;
  memp = (struct memp *)LWIP_MEM_ALIGN(desc->base);

  /* create a linked list of memp elements */
#if 0
//printf("[memp_init_pool] \r\n");
  printf("  [on]memp base %06x (desc_base %06x, desc_desc %s, desc_size %d, desc_num %d)\r\n", 
		 (void *) memp, desc->base, desc->desc, desc->size, desc->num);
#endif
  for (i = 0; i < desc->num; ++i) {
    memp->next = *desc->tab;
    *desc->tab = memp;
    /* cast through void* to get rid of alignment warnings */
#if 0
	printf("%d.alloc memp addr %06x\r\n", i, (void *)((u8_t *)memp + MEMP_SIZE + desc->size)); //...
#endif
    memp = (struct memp *)(void *)((u8_t *)memp + MEMP_SIZE + desc->size);
  }

#if MEMP_STATS
  desc->stats->avail = desc->num;
#endif /* MEMP_STATS */

#if MEMP_STATS && (defined(LWIP_DEBUG) || LWIP_STATS_DISPLAY)
  desc->stats->name  = desc->desc;
#endif /* MEMP_STATS && (defined(LWIP_DEBUG) || LWIP_STATS_DISPLAY) */
}
/**
 * Carves out memp_memory into linked lists for each pool-type.
 */
void
memp_init(void)
{
  u16_t i;

  /* for every pool: */
  for (i = 0; i < LWIP_ARRAYSIZE(memp_pools); i++) {
    memp_init_pool(memp_pools[i]);

#if LWIP_STATS && MEMP_STATS
    lwip_stats.memp[i] = memp_pools[i]->stats;
#endif
  } 
}

static void *do_memp_malloc_pool(const struct memp_desc *desc)
{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);
  SYS_ARCH_PROTECT(old_level);
  memp = *desc->tab;

  if (memp != NULL) {
    *desc->tab = memp->next;

    LWIP_ASSERT("memp_malloc: memp properly aligned",
                ((mem_ptr_t)memp % MEM_ALIGNMENT) == 0);
//#if MEMP_STATS
    desc->stats->used++;
    if (desc->stats->used > desc->stats->max) {
      desc->stats->max = desc->stats->used;
    }
//#endif
    SYS_ARCH_UNPROTECT(old_level);
    /* cast through u8_t* to get rid of alignment warnings */
    return ((u8_t *)memp + MEMP_SIZE);
  } else {
//#if MEMP_STATS
    desc->stats->err++;
//#endif
	  
    SYS_ARCH_UNPROTECT(old_level);
    LWIP_DEBUGF(MEMP_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("memp_malloc: out of memory in pool %s\n", desc->desc));
  }
  return NULL;
}

/**
 * Get an element from a specific pool.
 *
 * @param type the pool to get an element from
 *
 * @return a pointer to the allocated memory or a NULL pointer on error
 */
void *memp_malloc(memp_t type)
{
  void *memp;
  LWIP_ERROR("memp_malloc: type < MEMP_MAX", (type < MEMP_MAX), return NULL;);

  memp = do_memp_malloc_pool(memp_pools[type]);
  return memp;
}


static void do_memp_free_pool(const struct memp_desc *desc, void *mem)
{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

  LWIP_ASSERT("memp_free: mem properly aligned",
              ((mem_ptr_t)mem % MEM_ALIGNMENT) == 0);

  /* cast through void* to get rid of alignment warnings */
  memp = (struct memp *)(void *)((u8_t *)mem - MEMP_SIZE);

  SYS_ARCH_PROTECT(old_level);

#if MEMP_STATS
  desc->stats->used--;
#endif

#if MEMP_MEM_MALLOC
//.  LWIP_UNUSED_ARG(desc); ...............jhyujhy,iy..
//.  SYS_ARCH_UNPROTECT(old_level);
//.  mem_free(memp);
#else
#endif /* !MEMP_MEM_MALLOC */

  memp->next = *desc->tab;
  *desc->tab = memp;

#if MEMP_SANITY_CHECK
//.  LWIP_ASSERT("memp sanity", memp_sanity(desc)); ........,mlmkdf kl df.....
#endif /* MEMP_SANITY_CHECK */

  SYS_ARCH_UNPROTECT(old_level);
}
/**
 * Put an element back into its pool.
 *
 * @param type the pool where to put mem
 * @param mem the memp element to free
 */
void
memp_free(memp_t type, void *mem)
{
  LWIP_ERROR("memp_free: type < MEMP_MAX", (type < MEMP_MAX), return;);

  if (mem == NULL) {
    return;
  }

  do_memp_free_pool(memp_pools[type], mem);
}
