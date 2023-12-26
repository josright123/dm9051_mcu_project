#ifndef __TIMEO_H
#define __TIMEO_H

//
// timeo contain lwip cyclic timer
//
#define TIMEO_RUN_LWIP_CYCLIC_TIMER		0 

//
// "sys_timeout.c" (timeouts.c)
//
#define LWIP_MAX_TIMEOUT  0x7fffffff

/* Check if timer's expiry time is greater than time and care about u32_t wraparounds */
#define TIME_LESS_THAN(t, compare_to) ( (((uint32_t)((t)-(compare_to))) > LWIP_MAX_TIMEOUT) ? 1 : 0 )

/** Function prototype for a timeout callback function. Register such a function
 * using sys_timeout().
 *
 * @param arg Additional argument to pass to the function - set up by sys_timeout()
 */
typedef void (* sys_timeout_handler)(void *arg);

struct sys_timeo {
  struct sys_timeo *next;
  uint32_t time;
  uint32_t intvl_msec;
  sys_timeout_handler h;
  void *arg;
#if 1 //.LWIP_DEBUG_TIMERNAMES
  const char* handler_name;
#endif // .LWIP_DEBUG_TIMERNAMES
};

void netif_set_link_up1(void);
void netif_set_link_down1(void);

void sys_timeout_debug(uint32_t msecs, sys_timeout_handler handler, void *arg, const char *handler_name);
void sys_check_timeouts(void);

#endif //__TIMEO_H
