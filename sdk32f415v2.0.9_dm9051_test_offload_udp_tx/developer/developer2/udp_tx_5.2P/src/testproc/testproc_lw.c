/*
 * testproc_lw.c
 */
#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "testproc/testproc_lw.h"
#include "testproc/ethernetif.h"

#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/prot/dhcp.h"

extern struct netif netif;

//
// pbuf.c
//

#define EST_RX_PAYLOAD_LEN		  120 //Joseph's estimated reserved (a.s. tx 120)
#define SIZEOF_STRUCT_PBUF        LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf))
	
#define PBUF_POOL_BUFSIZE_ALIGNED LWIP_MEM_ALIGN_SIZE(PBUF_POOL_BUFSIZE)

#if 0
/* Initialize members of struct pbuf after allocation */
static void
pbuf_init_alloced_pbuf(struct pbuf *p, void *payload, u16_t tot_len, u16_t len, pbuf_type type, u8_t flags)
{
  p->next = NULL;
  p->payload = payload;
  p->tot_len = tot_len;
  p->len = len;
  p->type_internal = (u8_t)type;
  p->flags = flags;
  p->ref = 1;
  p->if_idx = NETIF_NO_INDEX;
  printf("pbuf_alloc2 pbuf_init_alloced_pbuf, p->tot_len = %u, p->len = %u\r\n", tot_len, len);
}
#endif
	
char pbuf_buff2[LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF)+EST_RX_PAYLOAD_LEN]; //I think 'EST_RX_PAYLOAD_LEN' is no need.

#define UIP_LLH_LEN				14  //#define EST_RX_EXTRA_LEN		UIP_LLH_LEN //14
#define MY_WORK_DHCP_MSG_LEN	342
#define EST_RX_SUCH_AS_CRC		4
//char to_rx_dhcp_msg_buff[sizeof(struct dhcp_msg)+EST_RX_EXTRA_LEN+EST_RX_SUCH_AS_CRC]; //struct dhcp_msg dhcp_msg_buff;
char to_rx_dhcp_msg_buff[UIP_LLH_LEN+ 20 + 8 + MY_WORK_DHCP_MSG_LEN + EST_RX_SUCH_AS_CRC];

  //[debug]
  extern int testing_tx_count;
  extern int testing_rx_count;
  //extern int _sending_done_flag;

struct pbuf *
pbuf_alloc2(pbuf_layer layer, u16_t length, pbuf_type type)
{
	
  //struct pbuf *p = &dhcp_msg_buff;
  struct pbuf *p;
  u16_t offset = (u16_t)layer; //PBUF_RAW= 0
	
	//x PBUF_TRANSPORT/ PBUF_RAM
	//o PBUF_RAW(= 0)/ PBUF_POOL
  switch (type) {
	  case PBUF_POOL: {
		struct pbuf *q, *last;
		u16_t rem_len; /* remaining length */
		p = NULL;
		last = NULL;
		rem_len = length;
		do {
		u16_t qlen;
		q = (struct pbuf *) pbuf_buff2; //memp_malloc(MEMP_PBUF_POOL);
		if (q == NULL) {
		  #if 0
		    PBUF_POOL_IS_EMPTY();
		  #endif
		  /* free chain so far allocated */
		  if (p) {
			pbuf_free(p);
		  }
		  /* bail out unsuccessfully */
		  return NULL;
		}
		qlen = LWIP_MIN(rem_len, (u16_t)(PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(offset)));
#if 0
bannerline();
printf("EthII/IPH/UDPH/payload/crc %d, %d, %d, %d, %d \r\n", 14, 20, 8, sizeof(struct dhcp_msg), 4);
printf("pbuf_alloc2 need buffsize %u, prepare-size %d\r\n", rem_len, sizeof(to_rx_dhcp_msg_buff));
		pbuf_init_alloced_pbuf(q, /*LWIP_MEM_ALIGN((void *)((u8_t *)q + SIZEOF_STRUCT_PBUF + offset))*/ &to_rx_dhcp_msg_buff,
							   rem_len, qlen, type, 0);
printf("next pass to lwip by such 'pbuf' - - - - - - - - - debug (txc %d rxc %d) (checkrxbcount %d)\r\n", 
		testing_tx_count, testing_rx_count, check_get()); //"sending_done_flag %d", sending_done_flag
printf("\r\n");
#endif
		LWIP_ASSERT("pbuf_alloc: pbuf q->payload properly aligned",
					((mem_ptr_t)q->payload % MEM_ALIGNMENT) == 0);
		LWIP_ASSERT("PBUF_POOL_BUFSIZE must be bigger than MEM_ALIGNMENT",
					(PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(offset)) > 0 );
		if (p == NULL) {
		  /* allocated head of pbuf chain (into p) */
		  p = q;
		} else {
		  /* make previous pbuf point to this pbuf */
		  last->next = q;
		}
		last = q;
		rem_len = (u16_t)(rem_len - qlen);
		offset = 0;
		} while (rem_len > 0);
		break;
	  }
	  /*case PBUF_RAM: {
		u16_t payload_len = (u16_t)(LWIP_MEM_ALIGN_SIZE(offset) + LWIP_MEM_ALIGN_SIZE(length));
		mem_size_t alloc_len = (mem_size_t)(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF) + payload_len);

		// bug #50040: Check for integer overflow when calculating alloc_len 
		if ((payload_len < LWIP_MEM_ALIGN_SIZE(length)) ||
		  (alloc_len < LWIP_MEM_ALIGN_SIZE(length))) {
			return NULL;
		}

		// If pbuf is to be allocated in RAM, allocate memory for it. 
		p = (struct pbuf *)pbuf_buff2; //= mem_malloc(alloc_len);
		if (p == NULL) {
		return NULL;
		}
		_pbuf_init_alloced_pbuf(p, //LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset)), //&dhcp_msg_buff,
							 length, length, type, 0);
        break;
      }*/
  }
  return p;
}

//
// ethernetif.c
//

	static struct pbuf *
	low_level_inp(struct netif *netif)
	{
	  struct pbuf *p, *q;
	  uint16_t len;
	  int l = 0;
	  uint8_t *buffer = get_rx_buffer(); //&EthBuff[0].rx;
		
	  len = dm9051_rx(buffer);
	  if (!len)
		  return NULL;
	  
	  testing_rx_count++;
	  dm9051_rxlog_monitor_rx(2, buffer, len);
	  //dm9051_rxlog_monitor_rx_all(2, buffer, len);
	  
	  p = pbuf_alloc2(PBUF_RAW, len, PBUF_POOL);
	  if (p)
	  {
		for (q = p; q != NULL; q = q->next)
		{
		  memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
		  l = l + q->len;
		}
	  }
	  return p;
	}

err_t ethernetif_inp(struct netif *netif)
{
	  err_t err = ERR_OK;
	  struct pbuf *p;
	
//.

	  p = low_level_inp(netif);
	  if (p == NULL) 
		return ERR_INPROGRESS; //JJ.

#if 0
	  //
	  // .lwip core is to parse, processing... (Here we skip-it)
	  //
	  err = netif->input(p, netif);
	  if (err != ERR_OK)
	  {
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
		pbuf_free(p);
	  }
#endif
	  return err;
}

//
// netconf.c
//

void dump_rx_hdlr(void)
{
	  //uint16_t len;
	  //int l = 0;
	  uint8_t *buffer = get_rx_buffer(); //&EthBuff[0].rx;
		
	  //len = 
		dm9051_rx_dump(buffer); //dm9051_rx_dump(buffer); //dm9051_rx(buffer);
	  //if (!len)
	  //  return NULL;
}

err_t lwip_rx_hdlr(void)
{
  err_t err;
#if TEST_PLAN_MODE
  if (!check_get()) //!checkrxbcount
	  return ERR_INPROGRESS;
#endif
  err = ethernetif_inp(&netif);
  DM9051_RX_DBGMSG(err != ERR_OK && err != ERR_INPROGRESS, ("[netconf.c] ethernetif_input: IP input error\r\n"));
  return err;
}


//
// testproc_lw.c
//

int test_total_send_count_setup; //.

//#ifdef AT32F437xx
//.uint16_t TestProcBuff_Len;
union {
	uint8_t rx;
	uint8_t tx;
} TestProcBuff[100];
int test_line7_enter = 0;
uint8_t my_debounce = 0;

uint16_t test_rx_hdlr(void)
{
	uint8_t *zbuff = &TestProcBuff[0].rx;
	uint16_t len;

printf("[rx_start.s]\r\n");
	len = dm9051_rx(zbuff);
printf("[rx_start.e] len = %d\r\n", len);
	
	if (len) {
		printf("  %s, recv, len %u\r\n", mstep_spi_conf_name(), len - 4); //sizeof(zbuff)
		dm9051_rxlog_monitor_rx_all(2, zbuff, len - 4); //HEAD_SPC= 2 //dm9051_rxlog_monitor_tx_all(zbuff, len - 4);
	}
	return len;
}

int CalcModelShow(int inParam)
{
	int outParam = inParam - 1;
	printf("................................[Yes-Good]: line7_proc() CalcModel-Show tested-packets %d recved-remain %d\r\n", inParam, outParam);
	return outParam;
}

void line7_proc(void) {
  uint16_t len;
	
  uint8_t isr;
  isr = cspi_read_reg(DM9051_ISR);
  
  printf("[INFO]: line7() enter %d ... isr %02x\r\n", ++test_line7_enter, isr);
  printf("................................ line7_proc(), where test_line7_enter_check_setup is %d\r\n", test_line7_enter_check_setup);
	
  if (test_line7_enter_check_setup == 0)
	  printf("................................ [WARN]: line7_proc() encounterred illegal enter! '_test_line7_enter_check_setup' still on 0\r\n");

  if (my_debounce == 0) {
	my_debounce = 8;
	at32_led_toggle(LED4);
	//at32_led_toggle(LED2);
	//at32_led_toggle(LED3);
	//at32_led_on(LED4);
  }
  
  #if 1
  //lwip_rx_loop_handler();
  //lwip_rx_hdlr();
  do {
	len = test_rx_hdlr();
	if (len) {
		if (test_total_send_count_setup) {
			
			test_total_send_count_setup = CalcModelShow(test_total_send_count_setup);
			
		} else 
			printf("[WARN]: line7() encounterred under_flow count!\r\n");
	}
  } while(len);
  #endif
  
  my_debounce += 8; // if this only, when 256, i.e. 0
  my_debounce &= 0x3f; // when 64, i.e. 0
  
  isr = cspi_read_reg(DM9051_ISR);
  printf("[INFO]: line7() exit %d ... isr %02x\r\n", ++test_line7_enter, isr);
  printf("[INFO]: line7() exit write isr %02x\r\n", isr);
  cspi_write_reg(DM9051_ISR, isr);
}
//#endif

static int checkrxbcount = 0;
static void check_set(int count) {
	checkrxbcount = count;
}

int check_get(void) {
	return checkrxbcount;
}
void check_decr_to_done(void) {
	checkrxbcount--;
}

void check_set_new(void) {
	check_set(10);
}
void check_set_done(void) {
	check_set(1);
}
int check_get_check_done(void) {
	return check_get() == 1 ? 1 : 0;
}
