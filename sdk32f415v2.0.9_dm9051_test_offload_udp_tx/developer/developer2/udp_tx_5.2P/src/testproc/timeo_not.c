#include "lwip/opt.h"
#include "lwip/stats.h"
#include "lwip/mem.h"
#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "testproc/timeo.h"
#include "tmr_init.h"

#if TIMEO_RUN_LWIP_CYCLIC_TIMER //0, in starter stage
static uint32_t current_timeout_due_time; //TIMEO_RUN_LWIP_CYCLIC_TIMER

/** Function prototype for a stack-internal timer function that has to be
 * called at a defined interval */
typedef void (* lwip_cyclic_timer_handler)(void);

/** This struct contains information about a stack-internal timer function
 that has to be called at a defined interval */
struct lwip_cyclic_timer {
  uint32_t interval_ms;
  lwip_cyclic_timer_handler handler;
#if 1 //.LWIP_DEBUG_TIMERNAMES
  const char* handler_name;
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
  next_timeout_time = (uint32_t)(current_timeout_due_time + cyclic->interval_ms);  /* overflow handled by TIME_LESS_THAN macro */
	//...
  if (TIME_LESS_THAN(next_timeout_time, now)) {
    /* timer would immediately expire again -> "overload" -> restart without any correction */
#if 1 //.LWIP_DEBUG_TIMERNAMES
    timeo = sys_timeout_abs1((uint32_t)(now + cyclic->interval_ms), lwip_cyclic_timer, arg, cyclic->handler_name);
#endif
  } else {
    /* correct cyclic interval with handler execution delay and sys_check_timeouts jitter */
#if 2 //.LWIP_DEBUG_TIMERNAMES
    timeo = sys_timeout_abs1(next_timeout_time, lwip_cyclic_timer, arg, cyclic->handler_name);
#endif
  }
  
  if (timeo) //JJ. add "timeo->intvl_msec"
	  timeo->intvl_msec = cyclic->interval_ms;
}
#endif //TIMEO_RUN_LWIP_CYCLIC_TIMER

/** The one and only timeout list */
static struct sys_timeo *next_timeout = NULL;

static struct sys_timeo *
sys_timeout_abs1(uint32_t abs_time, sys_timeout_handler handler, void *arg, const char *handler_name)
{
  struct sys_timeo *timeout, *t;

  timeout = (struct sys_timeo *)memp_malloc(MEMP_SYS_TIMEOUT);
  //if (timeout == NULL) {
	//printf("Erroe: sys_timeout, pool MEMP_SYS_TIMEOUT is empty~\r\n");
    //return NULL;
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

  if (next_timeout == NULL) {
    next_timeout = timeout;
    return timeout;
  }
  if (TIME_LESS_THAN(timeout->time, next_timeout->time)) {
    timeout->next = next_timeout;
    next_timeout = timeout;
  } else {
    for (t = next_timeout; t != NULL; t = t->next) {
      if ((t->next == NULL) || TIME_LESS_THAN(timeout->time, t->next->time)) {
        timeout->next = t->next;
        t->next = timeout;
        break;
      }
    }
  }
  return timeout;
}

void
sys_timeout_debug(uint32_t msecs, sys_timeout_handler handler, void *arg, const char *handler_name)
{
  static struct sys_timeo *timeo;
  uint32_t next_timeout_time;
	
  next_timeout_time = (uint32_t)(sys_now() + msecs); /* overflow handled by TIME_LESS_THAN macro */
  timeo = sys_timeout_abs1(next_timeout_time, handler, arg, handler_name);
  if (timeo)
	  timeo->intvl_msec = msecs;
}

/**
 * @ingroup lwip_nosys
 * Handle timeouts for NO_SYS==1 (i.e. without using
 * tcpip_thread/sys_timeouts_mbox_fetch(). Uses sys_now() to call timeout
 * handler functions when timeouts expire.
 *
 * Must be called periodically from your main loop.
 */
void
sys_check_timeouts(void)
{
  uint32_t now;

  /* Process only timers expired at the start of the function. */
  now = sys_now();

  do {
    struct sys_timeo *tmptimeout;
    sys_timeout_handler handler;
    void *arg;

    tmptimeout = next_timeout;
    if (tmptimeout == NULL) {
      return;
    }

    if (TIME_LESS_THAN(now, tmptimeout->time)) {
      return;
    }

    /* Timeout has expired */
    next_timeout = tmptimeout->next;
    handler = tmptimeout->h;
    arg = tmptimeout->arg;
#if TIMEO_RUN_LWIP_CYCLIC_TIMER
    current_timeout_due_time = tmptimeout->time;
#endif
#if 0 //LWIP_DEBUG_TIMERNAMES
    if (handler != NULL) {
      LWIP_DEBUGF(TIMERS_DEBUG, ("sct calling h=%s t=%"U32_F" arg=%p\n",
                                 tmptimeout->handler_name, sys_now() - tmptimeout->time, arg));
    }
#endif /* LWIP_DEBUG_TIMERNAMES */
    memp_free(MEMP_SYS_TIMEOUT, tmptimeout);
    if (handler != NULL) {
      handler(arg);
    }
    //LWIP_TCPIP_THREAD_ALIVE();

    /* Repeat until all expired timers have been called */
  } while (1);
}
