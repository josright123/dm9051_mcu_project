/**
 * @file
 * memory pools lwIP internal implementations (do not use in application code)
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

#ifndef LWIP_HDR_MEMP_PRIV_H
#define LWIP_HDR_MEMP_PRIV_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/mem.h"
#include "lwip/priv/mem_priv.h"

/* No sanity checks
 * We don't need to preserve the struct memp while not allocated, so we
 * can save a little space and set MEMP_SIZE to 0.
 */
#define MEMP_SIZE           0
#define MEMP_ALIGN_SIZE(x) (LWIP_MEM_ALIGN_SIZE(x))

struct memp {
  struct memp *next;
};

/** Memory pool descriptor */
struct memp_desc {
  /** Textual description */
  const char *desc; //..........................m,mnn...
	
  /** Statistics */
  struct stats_mem *stats;

  /** Element size */
  u16_t size;

  /** Number of elements */
  u16_t num;

  /** Base address */
  u8_t *base;

  /** First free element of each pool. Elements form a linked list. */
  struct memp **tab;
};

#define DECLARE_LWIP_MEMPOOL_DESC(desc) (desc), //.................fm ml,dgb....

#define LWIP_MEMPOOL_DECLARE_STATS_INSTANCE(name) static struct stats_mem name;
#define LWIP_MEMPOOL_DECLARE_STATS_REFERENCE(name) &name,

void memp_init_pool(const struct memp_desc *desc);
void *memp_malloc_pool(const struct memp_desc *desc);
void  memp_free_pool(const struct memp_desc* desc, void *mem);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_MEMP_PRIV_H */
