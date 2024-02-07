#include "dm9051opts.h" //driver attributes
//#include "dm9051spiopts.h" //board attributes

#define APPLICATION_NAME							"DM9051 x2web app"
#define APPLICATION_BANNER							"AT32F4 Lw212 web_server_r2312_rc1.0-Avis"
#define APPLICATION_DATE							"2023-12-28-Avis"

#define APPLICATION_LWIP_PRINT_TIMEOUTS				0 //.JJ (combine with "lwipiots.h" 'LWIP_TESTMODE')

/* [Network Config] */
//#define MAC_ADDR_LENGTH                  			(6)
//#define ADDR_LENGTH                      			(4)

/* MAC_5TH= 37 (0x25)
 * // IP_3RD= 37
 * //#define IP_3RD	IP3RD
 */
//#define IP3RD	37

//#define MAC_5TH	IP3RD

/*#define DECLARATION_MAC_ADDR \
uint8_t mac_addresse[ETHERNET_COUNT][MAC_ADDR_LENGTH] = { \
	{0, 0x60, 0x6e, 0x00, 0x01, MAC_5TH}, \
}

#define DECLARATION_LOCAL_IPADDR \
uint8_t cfg_local_ipe[ETHERNET_COUNT][ADDR_LENGTH]   	= { \
	{192, 168, 6, 17}, \
};  \
uint8_t cfg_local_gwe[ETHERNET_COUNT][ADDR_LENGTH]   	= { \
	{192, 168, 6, 1}, \
};  \
uint8_t cfg_local_maske[ETHERNET_COUNT][ADDR_LENGTH] 	= { \
	{255, 255, 255, 0}, \
}*/

/*
#if (ETHERNET_COUNT >= 2)
#undef DECLARATION_MAC_ADDR
#undef DECLARATION_LOCAL_IPADDR

#define DECLARATION_MAC_ADDR \
uint8_t mac_addresse[ETHERNET_COUNT][MAC_ADDR_LENGTH] = { \
	{0, 0x60, 0x6e, 0x00, 0x01, MAC_5TH+1}, \
	{0, 0x60, 0x6e, 0x00, 0x01, MAC_5TH}, \
}

#define DECLARATION_LOCAL_IPADDR \
uint8_t cfg_local_ipe[ETHERNET_COUNT][ADDR_LENGTH]   	= { \
	{192, 168, 6, 7}, \
	{192, 168, 6, 6}, \
}; \
uint8_t cfg_local_gwe[ETHERNET_COUNT][ADDR_LENGTH]   	= { \
	{192, 168, 6, 1}, \
	{192, 168, 6, 1}, \
}; \
uint8_t cfg_local_maske[ETHERNET_COUNT][ADDR_LENGTH] 	= { \
	{255, 255, 255, 0}, \
	{255, 255, 255, 0}, \
}
#endif
*/

/* [Network Config.broker.ip] */
#define DECLARATION_BROKER_IPADDR  					"192.168.1.187"
#define DECLARATION_BROKER_LISTEN_PORT     			1883

#define CLIENT_ID									"davicom_mqtt_9051"
#define USER_NAME									NULL // "davicom"
#define USER_PASSWORD								NULL // "password2023"

#define WILL_TOPIC									"WILL_TOPIC"	//"DeviceTopic"
#define WILL_MSG    								"pls_check"		//"DeviceOffline_pls_check"
#define WILL_QOS    								0
#define WILL_RETAIN									0

#define MQTT_MAX_KEEP_ALIVE_SECOND					120
#define MQTT_MIN_KEEP_ALIVE_SECOND					6

#define MQTT_CLIENT_KEEP_ALIVE_SECOND				60 //60 //20 //10 //20 //95 //.60

//typedef struct topic_cell_st { // reserved.
//	const char *topic_str;
//	uint8_t qos;
//	uint16_t ms;
//} topic_cell_t;
//topic_cell_t topic_cell[MQTT_MAX_PUBLISH_TOPIC_NUM];

#define MQTT_MAX_PUBLISH_TOPIC_NUM					12 //use to allocate space for operation for 'MQTT_CLIENT_PUBLISH_TOPIC_NUM' 
#define MQTT_MAX_SUBSCRIBE_TOPIC_NUM				12 //only const. (when operation, use 'MQTT_CLIENT_SUBSCRIBE_TOPIC_NUM')

#define MQTT_CLIENT_PUBLISH_TOPIC_NUM				12 //12 
#define MQTT_CLIENT_SUBSCRIBE_TOPIC_NUM				12 //12

/* sanity check */
#if MQTT_CLIENT_KEEP_ALIVE_SECOND > MQTT_MAX_KEEP_ALIVE_SECOND
#error "Error! MQTT_CLIENT_KEEP_ALIVE_SECOND large than MQTT_MAX_KEEP_ALIVE_SECOND!"
#endif
#if MQTT_CLIENT_KEEP_ALIVE_SECOND < MQTT_MIN_KEEP_ALIVE_SECOND
#error "Error! MQTT_MIN_KEEP_ALIVE_SECOND less than MQTT_MIN_KEEP_ALIVE_SECOND!"
#endif
#if MQTT_CLIENT_PUBLISH_TOPIC_NUM > MQTT_MAX_PUBLISH_TOPIC_NUM
#error "Error! MQTT_CLIENT_PUBLISH_TOPIC_NUM large than MQTT_MAX_PUBLISH_TOPIC_NUM!"
#endif
#if MQTT_CLIENT_SUBSCRIBE_TOPIC_NUM > MQTT_MAX_SUBSCRIBE_TOPIC_NUM
#error "Error! MQTT_CLIENT_SUBSCRIBE_TOPIC_NUM large than MQTT_MAX_SUBSCRIBE_TOPIC_NUM!"
#endif
 
//typedef enum {
//  TYPE_TICKS =0,
//  TYPE_TIMEROUTS =1
//} timretype_t;
#define TYPE_TICKS									0
#define TYPE_TIMEROUTS								1
typedef unsigned short 								timretype_t;
 
#define MQTT_CLIENT_LINK_TMR_MS						200
#define MQTT_CLIENT_LINK_TMR_TYPE					TYPE_TIMEROUTS
#define MQTT_CLIENT_PUBLISH_TMR_MS					300 //10000 //300 (300 ms)
#define MQTT_CLIENT_PUBLISH_TMR_TYPE				TYPE_TIMEROUTS

//.
//. [main_obj]
//.

#define MQTT_CLIENT_CONNECT_QUICK_PUBLISHS			0				// 0 or 1, Quick-publish while connected or link-up ..

//.
//. [app_type declaration]
//.

typedef struct link_model_st {
	uint16_t hdlr_ms;
	timretype_t timer_type;
} handler_info;

typedef enum {
  MQTT_SENDING_AVAIL =				0,
  MQTT_SENDING_RUNNING_PINGREQ =	1
} mqtt_sending_t;

typedef struct topic_cell_info_st {
	int num;
	int max_num;
	const char *topic[MQTT_MAX_PUBLISH_TOPIC_NUM];
	uint8_t qos[MQTT_MAX_PUBLISH_TOPIC_NUM];
	uint16_t ms[MQTT_MAX_PUBLISH_TOPIC_NUM];
} topic_cell_info_t;


//.
//. [link and publish handlers configure options]
//.

#define LINK_HANDLER_PAIR \
		/*.hdlr_ms =								MQTT_CLIENT_LINK_TMR_MS, */ \
		/*.timer_type =								MQTT_CLIENT_LINK_TMR_TYPE */ \
		/*{ MQTT_CLIENT_LINK_TMR_MS, MQTT_CLIENT_LINK_TMR_TYPE } */ \
		MQTT_CLIENT_LINK_TMR_MS, MQTT_CLIENT_LINK_TMR_TYPE

#define PUBLISH_HANDLER_PAIR \
		/*.hdlr_ms =								MQTT_CLIENT_PUBLISH_TMR_MS, */ \
		/*.timer_type =								MQTT_CLIENT_PUBLISH_TMR_TYPE */ \
		/*{ MQTT_CLIENT_PUBLISH_TMR_MS, MQTT_CLIENT_PUBLISH_TMR_TYPE } */ \
		MQTT_CLIENT_PUBLISH_TMR_MS, MQTT_CLIENT_PUBLISH_TMR_TYPE

#include "main.h"													//included by "developer_conf.h"

#ifdef DEVELOPER_CONF
/*const handler_info linkhandler_info[] = {
		//MQTT_CLIENT_LINK_TMR_MS, 
		//MQTT_CLIENT_LINK_TMR_TYPE,
		LINK_HANDLER_PAIR,
};
const handler_info publishhandler_info = {
		//MQTT_CLIENT_PUBLISH_TMR_MS, 
		//MQTT_CLIENT_PUBLISH_TMR_TYPE,
		PUBLISH_HANDLER_PAIR,
};*/

char *get_application_name(void) {
	return APPLICATION_NAME;
}

char *get_application_banner(void)
{
	return APPLICATION_BANNER;
}
char *get_application_date(void)
{
	return APPLICATION_DATE;
}

/* owner is head2.c */

timretype_t linkhandler_type(void) //get
{
	return linkhandler_info[mstep_get_net_index()].timer_type;
}

timretype_t publishhandler_type(void) //get
{
	return publishhandler_info[mstep_get_net_index()].timer_type;
}

#if 0
/*void set_mqtt_sending_state(mqtt_sending_t state)
{
	mqttpingreq_sending_cyclictmr = state;
}

mqtt_sending_t mqtt_sending_state(void) //get
{
	return mqttpingreq_sending_cyclictmr;
}*/
#endif

//. volatile uint32_t publish_init_timer1;// = 0;
//. volatile uint32_t publish_send_timer1;// = MQTT_CLIENT_PUBLISH0_MS; //get_publish_send_ms(0); //test_pub_ms[0]; //MQTT_CLIENT_PUBLISH0_MS;
//. volatile uint32_t publish_init_timer2;// = 0;
//. volatile uint32_t publish_send_timer2;// = MQTT_CLIENT_PUBLISH1_MS; //get_publish_send_ms(1); //test_pub_ms[1]; //MQTT_CLIENT_PUBLISH1_MS;

#if MQTT_CLIENT_SUPPORT
	volatile uint16_t publish_init_and_disp_timer[MQTT_MAX_PUBLISH_TOPIC_NUM]; //uint32_t
	static volatile uint32_t publish_send_timer[MQTT_MAX_PUBLISH_TOPIC_NUM];

	uint16_t get_publish_disp_time(int i) {
		return publish_init_and_disp_timer[i];
	}
	void set_publish_disp_time(int i, uint16_t val) {
		publish_init_and_disp_timer[i] = val;
	}

	uint32_t get_publish_expr_time(int i) {
		return publish_send_timer[i];
	}
	static void set_publish_expr_time(int i, uint32_t valbit32) {
		publish_send_timer[i] = valbit32;
	}

	void publish_handle_init(void)
	{
		int i; 
		for (i = 0; i < get_mqtt_publish_topic_number(); i++) // MQTT_CLIENT_PUBLISH_TOPIC_NUM v.s. MQTT_MAX_PUBLISH_TOPIC_NUM
			set_publish_expr_time(i, 0);
	}
#endif

uint16_t get_link_handler_ms(void) {
	return linkhandler_info[mstep_get_net_index()].hdlr_ms; //MQTT_CLIENT_LINK_TMR_MS;
}
uint16_t get_publish_handler_ms(void) {
	return publishhandler_info[mstep_get_net_index()].hdlr_ms; //MQTT_CLIENT_PUBLISH_TMR_MS;
}

#if MQTT_CLIENT_SUPPORT
int get_mqtt_max_publish_num(void) {
	return ptc.max_num; //MQTT_MAX_PUBLISH_TOPIC_NUM;
}

int get_mqtt_max_subscribe_num(void) { //only const. (for info display)
	return stc.max_num; //MQTT_MAX_SUBSCRIBE_TOPIC_NUM;
}
	
int get_mqtt_publish_topic_number(void) {
	return ptc.num; //MQTT_CLIENT_PUBLISH_TOPIC_NUME;
}
int get_mqtt_subscribe_topic_number(void) {
	return stc.num; //MQTT_CLIENT_SUBSCRIBE_TOPIC_NUME;
}
#endif

//#if 0
//DECLARATION_PUB_TOPICS;
//DECLARATION_PUB_QOSS;
//DECLARATION_PUB_MS;
//#endif

#if MQTT_CLIENT_SUPPORT
const char *get_publish_topic(int i)
{
	return ptc.topic[i]; //return demo_pub_topics[i];
}

uint8_t get_publish_qos(int i)
{
	return ptc.qos[i]; //return demo_pub_qos[i]; //MQTT_CLIENT_PUBLISH_TOPIC0_QOS;
}

uint16_t get_publish_send_ms(int i)
{
	return ptc.ms[i]; //return demo_pub_ms[i]; //MQTT_CLIENT_PUBLISH0_MS;
}
#endif

//#if 0
//DECLARATION_SUB_QOSS;
//DECLARATION_SUB_TOPICS;
//#endif

#if MQTT_CLIENT_SUPPORT
const char *get_subscribe_topic(int i)
{
	return stc.topic[i]; //demo_sub_topic[i];
}

uint8_t get_subscribe_qos(int i)
{
	return stc.qos[i]; //demo_sub_qos[i];
}
#endif

#endif
