#include <stdlib.h>         // for malloc and free
#include "dm9051_lw.h"      //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "dm9051_lw_testproc_type.h"
#include "dm9051_lw_testproc.h"
#include "tmr_init.h"
// usart3proc_fun.h
#include "../usart3proc/usart3proc_fun.h"
#include "../ledproc/ledproc.h"

int test_line7_enter_check_setup = 0; //.

int in_time_link_up_found(void)
{
  // uint8_t nsr;
  int i = 0;
  while (i++ < 20)
  {
    uint16_t bmsr = dm9051_bmsr_update(); // dm9051_link_update(); //nsr = spi_read_reg(DM9051_NSR);
    if (bmsr & PHY_LINKED_BIT)
    { //(nsr & NSR_LINKST)
      if (i > 1)
        printf(". link is up\r\n");
      return 1;
      // self_test_tx_and_rx(); //test...
      // break;
    }
    if (!(bmsr & PHY_LINKED_BIT))
    { //(nsr & NSR_LINKST)
      dm_delay_ms(100);
      printf("%s", (i < 20) ? "." : ". \r\n.  link is down\r\n");
    }
    if (i == 1)
      printf("  ");
  }
  return 0;
}

uint16_t testproc_drv_initialize(void)
{
  uint16_t id;
#if (ETHERNET_COUNT >= 2)
  //=ethernetif_dm9051_init_dual(MACaddr);
  int i;
  const uint8_t addr_meanless[6] = {0}; // no exactly need.

  for (i = 0; i < ETHERNET_COUNT; i++)
  {                         // get_eth_interfaces()
    mstep_set_net_index(i); // set_pin_code(i);
    dm9051_poweron_rst();
    dm_delay_ms(1);
    // id =
    id = dm9051_init(addr_meanless);
//    dm9051_board_irq_enable();
    dm9051_start(addr_meanless);
    // display_verify_chipid("dm9051_init", mstep_spi_conf_name(), id);
  }
  mstep_set_net_index(0);
  return 0;
#elif (ETHERNET_COUNT == 1)
  //
  // test init dual only need while only 1 ethernet port exist.
  //

  //=test_dm9051_init_dual(MACaddr);
  // uint16_t id;

  dm9051_poweron_rst();
  dm_delay_ms(1);
  // id =
  printf("................................ dm9051 init\r\n");
  id = dm9051_init(MACaddr);
  test_line7_enter_check_setup = 1;
  printf("................................ dm9051 start, test_line7_enter_check_setup %d\r\n", test_line7_enter_check_setup);
  //.display_verify_chipid("dm9051_init", mstep_spi_conf_name(), id);
#endif
  return id;
}

void testproc_net_test(void)
{
#if (ETHERNET_COUNT >= 2)
  //
  // JJ.Test Multi-SPI.
  //
//  bench_test_tx_and_rx(); // test_ethernetif_txrx();
  printf(": test end\r\n");
  // printf(": while(1);\r\n");
#elif (ETHERNET_COUNT == 1)
  //
  // my debug test!
  //

#if 0
	  //save.start
	  int i;
	  uint8_t rcr, rcrn;

	  rcr = cspi_read_reg(DM9051_RCR);
	  cspi_write_reg(DM9051_RCR, 0x31);
	  rcrn = cspi_read_reg(DM9051_RCR);
	  printf("\r\n");
	  printf(": .s bench_test rcr %02x to %02x\r\n", rcr, rcrn);

	  //only unicast for me is allowed.
	  for (i=0; i<8; i++)
		cspi_write_reg(DM9051_MAR+i, 0x00); // not accept broadcast, only unicast for me is allowed.
#endif

  in_time_link_up_found();

#if 0
	  if (in_time_link_up_found())
		  self_test_tx_and_rx(); //test...
#endif

#if 0
	  cspi_write_reg(DM9051_RCR, rcr);
	  rcr = cspi_read_reg(DM9051_RCR);
	  printf(": .e bench_test rcr %02x to %02x\r\n", rcrn, rcr);
	  //restore.done

	  printf(": test end\r\n");
	  //printf(": while(1);\r\n");
#endif

#endif
}

#define NETIF_FLAG_LINK_UP 0x04U
#define netif_set_flags(flags, set_flags)   \
  do                                        \
  {                                         \
    flags = (uint8_t)(flags | (set_flags)); \
  } while (0)
#define netif_clear_flags(flags, clr_flags)                    \
  do                                                           \
  {                                                            \
    flags = (uint8_t)(flags & (uint8_t)(~(clr_flags) & 0xff)); \
  } while (0)
#define netif_is_link_up(flags) ((flags & NETIF_FLAG_LINK_UP) ? (uint8_t)1 : (uint8_t)0)

typedef struct net_s net_t;

/** Function prototype for ... Register such a function
 * using netlink_init().
 *
 * @param net, argument to pass to the function - operation call by phy_link_timer()
 */
typedef void (*net_link_cb_t)(net_t *net, void *arg, uint8_t status);
// or typedef void (*net_link_cb_t)(net_t *net);

struct net_s
{
  /** link state */
  uint8_t link_state;
  /** net callback */
  void *net_arg;
  net_link_cb_t cbf; //=net_cb;
};

net_t __net_instance;
net_t __led_instance;

uint8_t netflags = 0;

void netif_set_link_up(void)
{
  if (!(netflags & NETIF_FLAG_LINK_UP))
    netif_set_flags(netflags, NETIF_FLAG_LINK_UP);
}

void netif_set_link_down(void)
{
  if (netflags & NETIF_FLAG_LINK_UP)
    netif_clear_flags(netflags, NETIF_FLAG_LINK_UP);
}

#define MQTT_CLIENT_LINK_TMR_MS 200

// enum
// {
//   NET_LINK_DOWN,
//   NET_LINK_UP
// };
typedef enum
{
  NET_LINK_DOWN = 0,
  NET_LINK_UP = 1
} net_link_status;

/** Create the list of all memory pools managed by memp. MEMP_MAX represents a NULL pool at the end */
typedef enum
{
  MEMP_SYS_TIMEOUT,
#if 0
#define LWIP_MEMPOOL(name, num, size, desc) MEMP_##name,
#include "lwip/priv/memp_std.h"
#endif
  MEMP_MAX
} memp_t;

void *memp_malloc(memp_t type);
void memp_free(memp_t type, void *mem);

//
// "sys_timeout.c" (timeouts.c)
//

#define sys_timeout(msecs, handler, arg) sys_timeout_debug(msecs, handler, arg, #handler)

/** Function prototype for a timeout callback function. Register such a function
 * using sys_timeout().
 *
 * @param arg Additional argument to pass to the function - set up by sys_timeout()
 */
typedef void (*sys_timeout_handler)(void *arg);

struct sys_timeo
{
  struct sys_timeo *next;
  uint32_t time;
  uint32_t intvl_msec;
  sys_timeout_handler h;
  void *arg;
#if 1 //.LWIP_DEBUG_TIMERNAMES
  const char *handler_name;
#endif // .LWIP_DEBUG_TIMERNAMES
};

#define LWIP_MAX_TIMEOUT 0x7fffffff

/* Check if timer's expiry time is greater than time and care about u32_t wraparounds */
#define TIME_LESS_THAN(t, compare_to) ((((uint32_t)((t) - (compare_to))) > LWIP_MAX_TIMEOUT) ? 1 : 0)

/** The one and only timeout list */
static struct sys_timeo *next_timeout = NULL;

static struct sys_timeo *
sys_timeout_abs1(uint32_t abs_time, sys_timeout_handler handler, void *arg, const char *handler_name)
{
  struct sys_timeo *timeout, *t;

  timeout = (struct sys_timeo *)memp_malloc(MEMP_SYS_TIMEOUT);
  // if (timeout == NULL) {
  // printf("Erroe: sys_timeout, pool MEMP_SYS_TIMEOUT is empty~\r\n");
  // return NULL;
  //}

  timeout->next = NULL;
  timeout->h = handler;
  timeout->arg = arg;
  timeout->time = abs_time;

#if 1 //.LWIP_DEBUG_TIMERNAMES
  timeout->handler_name = handler_name;
#if 0
  printf("sys_timeout: %p abs_time=%"U32_F" handler=%s arg=%p\r\n",
        (void *)timeout, abs_time, handler_name, (void *)arg);
#endif
#endif //.LWIP_DEBUG_TIMERNAMES

  if (next_timeout == NULL)
  {
    next_timeout = timeout;
    return timeout;
  }
  if (TIME_LESS_THAN(timeout->time, next_timeout->time))
  {
    timeout->next = next_timeout;
    next_timeout = timeout;
  }
  else
  {
    for (t = next_timeout; t != NULL; t = t->next)
    {
      if ((t->next == NULL) || TIME_LESS_THAN(timeout->time, t->next->time))
      {
        timeout->next = t->next;
        t->next = timeout;
        break;
      }
    }
  }
  return timeout;
}

void sys_timeout_debug(uint32_t msecs, sys_timeout_handler handler, void *arg, const char *handler_name)
{
  static struct sys_timeo *timeo;
  uint32_t next_timeout_time;

  next_timeout_time = (uint32_t)(sys_now() + msecs); /* overflow handled by TIME_LESS_THAN macro */
  timeo = sys_timeout_abs1(next_timeout_time, handler, arg, handler_name);
  if (timeo)
    timeo->intvl_msec = msecs;
}

//
// "sys_timeout.c" (timeouts.c)
//

struct sys_timeo mempfixed_desc_tab; // Global.

// void *memp_malloc(memp_t type)
// {
//   void *memp;
//   memp = &mempfixed_desc_tab;
// #if 0
//   memp = do_memp_malloc_pool(memp_pools[type]); //type = 'MEMP_SYS_TIMEOUT'
// #endif
//   return memp;
// }

// void memp_free(memp_t type, void *mem)
// {
//   if (mem == NULL)
//     return;
// #if 0
//   do_memp_free_pool(memp_pools[type], mem);
// #endif
// }

void *memp_malloc(memp_t type)
{
  struct sys_timeo *memp = malloc(sizeof(struct sys_timeo));
  if (memp == NULL)
  {
    // handle error here, e.g., return NULL or exit program
    printf("Erroe: memp_malloc, pool MEMP_SYS_TIMEOUT is empty~\r\n");
  }

  // do something with memp based on the value of type

  return memp;
}

void memp_free(memp_t type, void *mem)
{
  if (mem != NULL)
  {
    free(mem);
  }
}

void phy_link_timer(void *arg)
{
  net_t *net = arg;
  uint8_t chip_link_up = (dm9051_link_update() & 0x0004) ? 1 : 0;

  if (!netif_is_link_up(netflags))
  {
    if (chip_link_up)
    {
      net->link_state = NET_LINK_UP;
      net->cbf(net, net->net_arg, NET_LINK_UP);
    }
  }
  else if (netif_is_link_up(netflags))
  {
    if (!chip_link_up)
    {
      net->link_state = NET_LINK_DOWN;
      net->cbf(net, net->net_arg, NET_LINK_DOWN);
    }
  }
  usart3proc_time_event(MQTT_CLIENT_LINK_TMR_MS);
#if 1
  sys_timeout(MQTT_CLIENT_LINK_TMR_MS, phy_link_timer, net);
#endif
}

void linkup_cb(net_t *net, void *arg, uint8_t status)
{
  // xxx = arg; NULL;
#if 1
  if (status == NET_LINK_UP)
  {
    printf("dm9051 link up\r\n");
    netif_set_link_up(); // netif_set_link_up_INITAndTimer(&netif);

    // #if LWIP_NETIF_LINK_CALLBACK == 0
    //	env_ethernetif_update_config_cb(&netif);
    // #endif
  }

  if (status == NET_LINK_DOWN)
  {
    printf("dm9051 link down\r\n");
    netif_set_link_down(); // netif_set_link_down_Timer(&netif);
  }
#endif
}

// static void net_new_task(net_t *net, net_link_cb_t cb, void *arg)
//{
//   net->link_state = NET_LINK_DOWN; //net->cyclic_tick = 0;
//   net->cbf = cb;
//   net->net_arg = arg;
//   sys_timeout(MQTT_CLIENT_LINK_TMR_MS, phy_link_timer, net);
// }

// void netlink_init(void) // (are to phase-in, NOW start API-skelton.)
// {
//   // net_new_task(&__net_instance, linkup_cb, NULL);
//   __net_instance.link_state = NET_LINK_DOWN;
//   __net_instance.cbf = linkup_cb;
//   __net_instance.net_arg = NULL;
//   /* Start cyclic link_handle timer */
// #if 1
//   sys_timeout(MQTT_CLIENT_LINK_TMR_MS, phy_link_timer, &__net_instance); //(no sys_untimeout(), THIS IS FOREEVER LOOP CYCLE, such as 'link_handle()')
// #endif
// }

static void net_new_task(net_t *net, net_link_cb_t cb, void *arg)
{
  net->link_state = NET_LINK_DOWN;
  net->cbf = cb;
  net->net_arg = arg;
  sys_timeout(MQTT_CLIENT_LINK_TMR_MS, phy_link_timer, net);
}

void netlink_init(void)
{
  net_new_task(&__net_instance, linkup_cb, NULL);
  /* Start cyclic link_handle timer */
#if 0
  sys_timeout(MQTT_CLIENT_LINK_TMR_MS, phy_link_timer, &__net_instance);
#endif
}

// *********************************************************************************************************************
// void led_status_timer(void *arg)
// {
//   net_t *net = arg;

//   switch (net->link_state)
//   {
//   case LED_STATUS_OK:
//     // led_toggle(2);
//     net->link_state = LED_STATUS_OK;
//     net->cbf(net, net->net_arg, LED_STATUS_OK);
//     sys_timeout(LED_STATUS_OK_TMR_MS, led_status_timer, net);
//     break;

//   case LED_STATUS_ERROR:
//     // led_toggle(3);
//     net->link_state = LED_STATUS_ERROR;
//     net->cbf(net, net->net_arg, LED_STATUS_ERROR);
//     sys_timeout(LED_STATUS_ERROR_TMR_MS, led_status_timer, net);
//     break;

//   case LED_STATUS_TIMEOUT:
//     // led_toggle(4);
//     net->link_state = LED_STATUS_TIMEOUT;
//     net->cbf(net, net->net_arg, LED_STATUS_TIMEOUT);
//     sys_timeout(LED_STATUS_TIMEOUT_TMR_MS, led_status_timer, net);
//     break;

//   case LED_STATUS_CRC16_ERROR:
//     // led_toggle(3);
//     net->link_state = LED_STATUS_CRC16_ERROR;
//     net->cbf(net, net->net_arg, LED_STATUS_CRC16_ERROR);
//     sys_timeout(LED_STATUS_CRC16_ERROR_TMR_MS, led_status_timer, net);
//     break;

//   default:
//     break;
//   }
// }

void led_status_timer(void *arg)
{
  net_t *net = arg;
  switch (led_status_flag)
  // switch (net->link_state)
  {
  case LED_STATUS_OK:
    // led_toggle(2);
    net->link_state = LED_STATUS_OK;
    net->cbf(net, net->net_arg, LED_STATUS_OK);
    sys_timeout(LED_STATUS_OK_TMR_MS, led_status_timer, net);
    break;

  case LED_STATUS_ERROR:
    // led_toggle(3);
    net->link_state = LED_STATUS_ERROR;
    net->cbf(net, net->net_arg, LED_STATUS_ERROR);
    sys_timeout(LED_STATUS_ERROR_TMR_MS, led_status_timer, net);
    break;

  case LED_STATUS_TIMEOUT:
    // led_toggle(4);
    net->link_state = LED_STATUS_TIMEOUT;
    net->cbf(net, net->net_arg, LED_STATUS_TIMEOUT);
    sys_timeout(LED_STATUS_TIMEOUT_TMR_MS, led_status_timer, net);
    break;

  case LED_STATUS_CRC16_ERROR:
    // led_toggle(3);
    net->link_state = LED_STATUS_CRC16_ERROR;
    net->cbf(net, net->net_arg, LED_STATUS_CRC16_ERROR);
    sys_timeout(LED_STATUS_CRC16_ERROR_TMR_MS, led_status_timer, net);
    break;

  default:
    net->link_state = LED_STATUS_NONE;
    net->cbf(net, net->net_arg, LED_STATUS_NONE);
    sys_timeout(LED_STATUS_NONE_TMR_MS, led_status_timer, net);
    break;
  }
}

void led_status_cb(net_t *net, void *arg, uint8_t status)
{

  switch (status)
  {
  case LED_STATUS_OK:
    at32_led_toggle(LED2);
    // printf("led_status_cb led ON\r\n");
    break;

  case LED_STATUS_ERROR:
    at32_led_toggle(LED3);
    break;

  case LED_STATUS_TIMEOUT:
    at32_led_toggle(LED4);
    break;

  case LED_STATUS_CRC16_ERROR:
    at32_led_toggle(LED3);
    break;

  default:
    break;
  }
}

static void led_new_task(net_t *net, net_link_cb_t cb, void *arg)
{
  net->link_state = LED_STATUS_NONE;
  net->cbf = cb;
  net->net_arg = arg;
  // sys_timeout(1000, led_status_timer, net);
  sys_timeout(LED_STATUS_OK_TMR_MS, led_status_timer, net);
}

void led_status_init(void)
{
  led_new_task(&__led_instance, led_status_cb, NULL);
}

static uint32_t current_timeout_due_time;

/** Function prototype for a stack-internal timer function that has to be
 * called at a defined interval */
typedef void (*lwip_cyclic_timer_handler)(void);

/** This struct contains information about a stack-internal timer function
 that has to be called at a defined interval */
struct lwip_cyclic_timer
{
  uint32_t interval_ms;
  lwip_cyclic_timer_handler handler;
#if 1 //.LWIP_DEBUG_TIMERNAMES
  const char *handler_name;
#endif /* LWIP_DEBUG_TIMERNAMES */
};

static void
lwip_cyclic_timer(void *arg)
{
  static struct sys_timeo *timeo;
  uint32_t now;
  uint32_t next_timeout_time;
  const struct lwip_cyclic_timer *cyclic = (const struct lwip_cyclic_timer *)arg;

  cyclic->handler();

  now = sys_now();
  next_timeout_time = (uint32_t)(current_timeout_due_time + cyclic->interval_ms); /* overflow handled by TIME_LESS_THAN macro */
                                                                                  //...
  if (TIME_LESS_THAN(next_timeout_time, now))
  {
    /* timer would immediately expire again -> "overload" -> restart without any correction */
#if 1 //.LWIP_DEBUG_TIMERNAMES
    timeo = sys_timeout_abs1((uint32_t)(now + cyclic->interval_ms), lwip_cyclic_timer, arg, cyclic->handler_name);
#endif
  }
  else
  {
    /* correct cyclic interval with handler execution delay and sys_check_timeouts jitter */
#if 2 //.LWIP_DEBUG_TIMERNAMES
    timeo = sys_timeout_abs1(next_timeout_time, lwip_cyclic_timer, arg, cyclic->handler_name);
#endif
  }

  if (timeo) // JJ. add "timeo->intvl_msec"
    timeo->intvl_msec = cyclic->interval_ms;
}

/**
 * @ingroup lwip_nosys
 * Handle timeouts for NO_SYS==1 (i.e. without using
 * tcpip_thread/sys_timeouts_mbox_fetch(). Uses sys_now() to call timeout
 * handler functions when timeouts expire.
 *
 * Must be called periodically from your main loop.
 */
void sys_check_timeouts(void)
{
  uint32_t now;

  /* Process only timers expired at the start of the function. */
  now = sys_now();

  do
  {
    struct sys_timeo *tmptimeout;
    sys_timeout_handler handler;
    void *arg;

    tmptimeout = next_timeout;
    if (tmptimeout == NULL)
    {
      return;
    }

    if (TIME_LESS_THAN(now, tmptimeout->time))
    {
      return;
    }

    /* Timeout has expired */
    next_timeout = tmptimeout->next;
    handler = tmptimeout->h;
    arg = tmptimeout->arg;
    current_timeout_due_time = tmptimeout->time;
#if 0  // LWIP_DEBUG_TIMERNAMES
    if (handler != NULL) {
      LWIP_DEBUGF(TIMERS_DEBUG, ("sct calling h=%s t=%"U32_F" arg=%p\n",
      tmptimeout->handler_name, sys_now() - tmptimeout->time, arg));
    }
#endif /* LWIP_DEBUG_TIMERNAMES */
    memp_free(MEMP_SYS_TIMEOUT, tmptimeout);
    if (handler != NULL)
    {
      handler(arg);
    }
    // LWIP_TCPIP_THREAD_ALIVE();

    /* Repeat until all expired timers have been called */
  } while (1);
}

void threads_support(void)
{
  sys_check_timeouts();
}

void testproc_run(void)
{
  // at32_dm9051_tmr_init(); //dm9051_tmr_init(); //old: env_dm9051_tmr_init()
  // printf(":  while(1);\r\n"); //since followed by a empty-while-loop.

  netlink_init();
  led_status_init();
  usart3proc_init();

#if 1
  at32_dm9051_tmr_init(); // dm9051_tmr_init(); //old: env_dm9051_tmr_init()
  printf(":  Connect Detecting...\r\n");
  printf("\r\n");
#endif

  while (1)
  {
    threads_support();
    usart3proc_main();
  }
}

//void testproc_board_initialize(void)
//{
//  printf("\r\n");
//  printf("- dm9051_board_initialize [%d spi board(s), 'devconf' as %s]\r\n", mstep_conf_spi_count(), mstep_conf_type());
//  printf("- dm9051_board_initialize [%d eth device(s)]\r\n", ETHERNET_COUNT);
//  printf("\r\n");
//  dm9051_board_initialize(); // netif_create(&ethernetif_create); //at32_dm9051_init_configuration_all(); //env_dm9051f_system_init();
//  printf("\r\n");
//}
