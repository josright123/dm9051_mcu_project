/* mode configure settings */ 
#define APPLICATION_NAME_CONST						"Davicom basic app"

//#undef APPLICATION_NAME
//#define APPLICATION_NAME							APPLICATION_NAME_CONST

//.#if 0 != c_strcmp(APPLICATION_NAME, APPLICATION_NAME_CONST)
//.#error "Error! APPLICATION_NAME can not modify!"
//.#endif

#define MQTT_CLIENT_CAST_LWIP_IGMP					0

#if LWIP_IGMP != MQTT_CLIENT_CAST_LWIP_IGMP
//; #error "Error! We ask to use as LWIP_IGMP 0 (check your lwipopts.h or some where else) !"
#endif
		
/* ----------------- main.h start ----------------------------- */

#define	WEB_SERVER_SUPPORT							1
#define	MQTT_CLIENT_SUPPORT							0 //1 //0  //application fixed.

#define TOPIC_MAXLEN_SUBSCRIBE						128 //32 //fixed data len, to change - pls contact to davicom software support team.

//.
//. [OPTIONS (DBG)]
//.
#define DBG_TEST_NO_PUBLISH_EXPIRE					0 //1

//.
//. [OPTIONS (DBG LOG)]
//.

#define TCP_C_DEBUG_ADV								1  //version fixed.
#define TCP_C_DEBUG									0

#define TCP_IN_C_DEBUG								1 // implemented in "tcp_in.c"

#define MQTT_C_DEBUG								0 //(1) // implemented in "mqtt.c"
	 
extern struct netif netif;

extern int timeouts_init_num_cyclic_timers;
extern volatile uint32_t all_local_time;
//extern mqtt_sending_t mqttpingreq_sending_cyclictmr;

/* extern */
timretype_t linkhandler_type(void);
timretype_t publishhandler_type(void);

void set_mqtt_sending_state(mqtt_sending_t state);
mqtt_sending_t mqtt_sending_state(void);
	 
 /* (0) or (1) */
#define MQTT_DEBUG_SYS_TIMEOUTS_SHOW			 	(1) //(0) //(1)	/* Disable/Enable, the options */
#define MQTT_CONNECT_WILL_TOPIC_SHOW				(1)
#define MQTT_CONNECT_PUBLISH_TOPIC_SHOW				(0)
#define MQTT_CONNECT_SUBSCRIBE_TOPIC_SHOW			(1)

//[ops]
#define MQTT_MAIN_CLIENT_DO_PUBLISH					1
#define MQTT_MAIN_CLIENT_DO_PUBLISH_LF				1

#define MQTT_DEBUG_TIME								(1) //0  //version fixed.
#define MQTT_DEBUG_BASIC							0  //version fixed.

void time_update(void);
void time_update_support(void);
void threads_support(void);

int min_score_times(uint16_t times, int chkindex);
int max_score_times(uint16_t times, int chkindex);

//["netconf - COMM2.c"] / ("head2.c")
int weight_support(uint16_t sec, uint16_t keep_alive);
int timeouts_keep_alive(uint16_t sec, uint16_t keep_alive);
uint16_t calc_calc_pingreg1(uint16_t keep_alive);

#include "lwip/timeouts.h"

#if MQTT_CLIENT_SUPPORT
//#include "mqtt_client.h"
int mqttc_is_connected(void);
void set_incoming_subscribe_topic(char *str, u16_t topic_len, u8_t qos);
#else
#define mqttc_is_connected()						0
#define set_incoming_subscribe_topic(str, topic_len, qos)
#endif

void quick_publish_head_str(void);
void quick_publish_tail_str(int i);

//[added, "timeouts.c"]
int sys_count_timeouts(void);
void sys_print_timeouts_times0(int n);
void sys_print_timeouts_times1(const char *headstr, int n);

//["log", "netconf2.h"]
void mqtt_client_init_log(void);

void BannerDisplay(void);
void NetifDisplay(int i);
void EepromDisplay(int pin);
void ParamDisplay(void);

//void print_calc_pingreg_watchdog(u16_t keep_alive);
void print_will_topic_info(void);
void print_publish_info(int publish_topic_num);
void print_subscribe_info(void);

void reset_change_timeouts(void);
void onchange_timeouts_log(const char *headstr);

int netconf_is_link_up(void);

void publish_handle_init(void);

void publish_handle_new(int i, uint16_t times, int mqtt_connected);
int publish_handle_expire(uint32_t expr_time);
void publish_handle_news(char *headstr);
//void event_linkup_time_setup(void);
//u8_t get_incoming_msg_qos(void);
char *get_incoming_subscribe_topic(void);

void mqttc_close(void);

//[part of developer_conf.h]
extern const struct mqtt_connect_client_info_t mqtt_connect_info; // to for "mqtt_client.c"

/* externs */
uint16_t get_link_handler_ms(void);
uint16_t get_publish_handler_ms(void);

uint16_t get_publish_disp_time(int i);
void set_publish_disp_time(int i, uint16_t val);

uint32_t get_publish_expr_time(int i);
//static void set_publish_expr_time(int i, uint32_t valbit32);

#define	lwip_get_mac_addresse1()	mstep_eth_mac()
//uint8_t *lwip_get_mac_addresse1(void); //void lwip_get_mac_addresse0(uint8_t *adr);

char *get_application_name(void);
char *get_application_banner(void);
char *get_application_date(void);

char *lwip_get_broker_ipe(void);
uint16_t get_broker_listen_port(void);

int get_mqtt_max_publish_num(void);
int get_mqtt_max_subscribe_num(void);

int get_mqtt_publish_topic_number(void);
int get_mqtt_subscribe_topic_number(void);

const char *get_publish_topic(int i);
uint8_t get_publish_qos(int i);
uint16_t get_publish_send_ms(int i);

const char *get_subscribe_topic(int i);
uint8_t get_subscribe_qos(int i);

//#define ETHERNETIF_COPY	//for iosolate global ipv4 data.

#ifdef ETHERNETIF_COPY
void lwip_copy_ip_addresse(uint8_t *ipadr);
void lwip_copy_qw_addresse(uint8_t *ipadr);
void lwip_copy_mask_addresse(uint8_t *ipadr);
#else
const uint8_t *lwip_get_ip_addresse(void);
const uint8_t *lwip_get_qw_addresse(void);
const uint8_t *lwip_get_mask_addresse(void);
#endif

void debug_dynamic_page(void);

/* [implement section code] */

/*
 * #define LINK_DETECTION_TCK               		(1)
 * #define LINK_DETECTION_NET               		(1)
 * #define LINK_DETECTION_TCK_IMPL          		(1)
 * #define LINK_DETECTION_NET_IMPL          		(1)
 */

//LINK_HANDLER_MS,
//#define LINK_HANDLER_MS \
//		.hdlr_ms =									MQTT_CLIENT_LINK_TMR_MS // e.g. 200 / 300 ms
//PUBLISH_HANDLER_MS,
//#define PUBLISH_HANDLER_MS \
//		.hdlr_ms =									MQTT_CLIENT_PUBLISH_TMR_MS

#ifdef DEVELOPER_CONF
#if (ETHERNET_COUNT == 1)
const handler_info linkhandler_info[ETHERNET_COUNT] = {
	{
		LINK_HANDLER_PAIR, //LINK_HANDLER_TYPE,
		//.hdlr_ms =									MQTT_CLIENT_LINK_TMR_MS, // e.g. 200 / 300 ms
		//.timer_type =									TYPE_TICKS/TYPE_TIMEROUTS, // project develop configuration.
	},
};
const handler_info publishhandler_info[ETHERNET_COUNT] = {
	{
		PUBLISH_HANDLER_PAIR, //PUBLISH_HANDLER_TYPE,
	},
};
#elif (ETHERNET_COUNT >= 2)
const handler_info linkhandler_info[ETHERNET_COUNT] = {
	{
		MQTT_CLIENT_LINK_TMR_MS, 
		MQTT_CLIENT_LINK_TMR_TYPE,
	},
	{
		MQTT_CLIENT_LINK_TMR_MS, 
		MQTT_CLIENT_LINK_TMR_TYPE,
	},
};
const handler_info publishhandler_info[ETHERNET_COUNT] = {
	{
		PUBLISH_HANDLER_PAIR, //PUBLISH_HANDLER_TYPE,
	},
	{
		PUBLISH_HANDLER_PAIR, //PUBLISH_HANDLER_TYPE,
	},
};
#endif
#else //DEVELOPER_CONF
extern const handler_info linkhandler_info[ETHERNET_COUNT];
extern const handler_info publishhandler_info[ETHERNET_COUNT];
#endif //DEVELOPER_CONF

//void line7(void);
