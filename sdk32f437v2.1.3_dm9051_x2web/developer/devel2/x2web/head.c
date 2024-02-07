#include "lwip/timeouts.h"
//#include "lwip/apps/mqtt.h"
//#include "lwip/apps/mqtt_priv.h"
#if 0
#include "netconf.h"
#endif
//#include "main.h" //#include "developer_conf.h"
#include "developer_conf.h" //#include "main.h" //#include "developer_conf.h"

#define SYSTEMTICK_PERIOD_MS             	10

//.void to_time_publish(void);
//.void to_time_link(void);

extern volatile uint32_t all_local_time;
extern u32_t lwip_sys_now;

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
	sys_check_timeouts();
}

#if 0
.#define NZEND_NUM_MS					  	500
.volatile uint32_t publish_send_timer_x0 = MQTT_CLIENT_PUBLISH_MS;
//.volatile uint32_t lnk_local_ctime = 0;
//.#define LINK_TIME() lnk_local_ctime
//.#define SET_LINK_TIME(time) lnk_local_ctime = time

.void event_linkup_time_setup(void)
.{
.	publish_send_timer_x0 =  all_local_time + NZEND_NUM_MS; //."linkup_process"
.	//SET_LINK_TIME(0); //."linkup_process"
.	if (all_local_time >= MQTT_CLIENT_PUBLISH_MS) { //."linkup_process" for only not during startup.
.		//SET_LINK_TIME(all_local_time - MQTT_CLIENT_PUBLISH_MS + NZEND_NUM_MS);
.		ethernetif_reset_proc();
.	}
.}
#endif

//.int event_linkup_time_handel(uint32_t DistnTime)
//.{
//.}

/*.publishing
 */
//.void publish_handle(void) //(uint32_t DistnTime)
//.{
  ///static volatile uint32_t publish_send_timer_x0 = 0;
//.  if (all_local_time >= publish_send_timer_x0)
//.  {  
//.	publish_send_timer_x0 =  all_local_time + MQTT_CLIENT_PUBLISH_MS;
//.	to_time_publish();
//.  }
//.}

#if 0
/* linkDetectFunc (are to phase-out)
 */
.void link_handle(void) // (are to phase-out)
.{
.  static volatile uint32_t link_timer = 0;
.  if (!link_timer)
.	link_timer = get_link_handler_ms(); //LINK_HANDLER_MS; // (are to phase-out)
.
.  if (all_local_time >= link_timer)
.  //if (all_local_time - link_timer >= eachtime || all_local_time < link_timer)
.  {
.    link_timer = all_local_time + get_link_handler_ms(); //LINK_HANDLER_MS;
.    //link_timer = all_local_time;
.
.	to_time_link();
.  }
.}
#endif

//[Timer][mqtt][sustain]

//Primary : ((a)*1.5)
//#define raising_weight(a)	(((a)*3)/2)

//#define weight_compare() \
//	raising_weight(MQTT_CYCLIC_TIMER_INTERVAL + 3)

//u16_t weight_sec_dbg(u16_t sec) { // for -dbg
//	return (weight_compare() + sec);
//}

#if 0
int weight_support(u16_t sec, u16_t keep_alive)
{
	//return ((weight_compare() + sec) >= keep_alive) ? 1 : 0;
	return (sec + MQTT_CYCLIC_TIMER_INTERVAL) >= keep_alive ? 1 : 0;
}
#endif

/*int timeouts_linkdown_process(void)
{
	extern int netconf_is_link_up(void);
	static u8_t lnkdwn_count = 0;
	if (netconf_is_link_up())
		lnkdwn_count = 0;
	else {
		if (lnkdwn_count < 1) {
			lnkdwn_count ++;
			//.printf(" %us, LINKDOWN found in mqtt_cyclic_timer\r\n", client->cyclic_tick * MQTT_CYCLIC_TIMER_INTERVAL);
		} else {
			lnkdwn_count  =0;
			//printf(" %us, LINKDOWN do disconnect in mqtt_cyclic_timer\r\n", client->cyclic_tick * MQTT_CYCLIC_TIMER_INTERVAL);
			return 1;
		}
	}
	return 0;
}*/

//[Driver][suppliment]

//.void dm9051_delay_in_core_process(uint16_t nms) //finally, dm9051_lw.c
//.{
  //.#if 1
	//.printf("dm9051_driver setup delay %u ms..\r\n", nms);
	//.dm_delay_ms(nms); //from James' advice! to be determined with a reproduced test cases!!
  //.#endif
//.}
