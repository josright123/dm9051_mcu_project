//#include "lwip/timeouts.h"
#if 0
//#include "netconf.h"
#endif
//#include "developer_conf.h" //#include "main.h" //#include "developer_conf.h"

#include <stdint.h>	 

#define SYSTEMTICK_PERIOD_MS             	10

//.void to_time_publish(void);
//.void to_time_link(void);

extern volatile uint32_t all_local_time;
extern uint32_t lwip_sys_now;

//[Timer][Basic](lwip and mqtt)
/**
  * @brief  updates the system local time
  * @param  none
  * @retval none
  */
void time_update_support(void)
{
  all_local_time += SYSTEMTICK_PERIOD_MS;
  lwip_sys_now += SYSTEMTICK_PERIOD_MS;
}

//[Timer][Dispatcher]

void threads_support(void)
{
#if 0
  //#include "lwip/timeouts.h"
	sys_check_timeouts();
#endif
}

//[Timer][mqtt][sustain]

//[Driver][suppliment]

//.void dm9051_delay_in_core_process(uint16_t nms) //finally, dm9051_lw.c
//.{
  //.#if 1
	//.printf("dm9051_driver setup delay %u ms..\r\n", nms);
	//.dm_delay_ms(nms); //from James' advice! to be determined with a reproduced test cases!!
  //.#endif
//.}

void time_update(void)
{
  time_update_support();
}
