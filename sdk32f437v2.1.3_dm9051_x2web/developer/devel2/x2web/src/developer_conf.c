/**
 * @file
 * Developer Configuration file
 *
 */

/*
 * Copyright (c) 2023-2025 Davicom Semiconductor, Inc.
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
 * These configuration procedures are for mqtt client application
 * First veryification with : AT32F415
 *
 * Author: Joseph CHANG <joseph_chang@davicom.com.tw>
 * Date: 20230919
 * Date: 20230919 (V0)
 */
#include "dm9051_lw_conf.h"
#include "dm9051_lw.h" //#include "at32f415_board.h" //

//#include "at32_cm4_device_support.h" //#include "at32f435_437_board.h" //""at32f413_board.h" //#include "at32f415_board.h" //"at32f403a_407_board.h"
#define DEVELOPER_CONF
#include "developer_conf.h"

#include <string.h>

/* owner is "mqtt_client.c", MUST put in "developer_conf.c" 
 * So can be sync-updated.
 */
#if 0
const struct mqtt_connect_client_info_t mqtt_connect_info = {
	CLIENT_ID, /** Client identifier, must be set by caller */
	USER_NAME, /** User name, set to NULL if not used */
	USER_PASSWORD, /** Password, set to NULL if not used */
	MQTT_CLIENT_KEEP_ALIVE_SECOND, /** keep alive time in seconds, 0 to disable keep alive functionality*/
	/** will topic, set to NULL if will is not to be used, ...*/
	WILL_TOPIC, /** will topic */ //(Set to NULL ?) 
	WILL_MSG, /** will_msg */ //(First when subscribe)
	WILL_QOS, /** will_qos */
	WILL_RETAIN /** will_retain */
};
#endif


#define MQTT_BROKER_IP					DECLARATION_BROKER_IPADDR
#define MQTT_BROKER_LISTEN_PORT			DECLARATION_BROKER_LISTEN_PORT

char *lwip_get_broker_ipe(void)
{
	return MQTT_BROKER_IP;
}

uint16_t get_broker_listen_port(void)
{
	return MQTT_BROKER_LISTEN_PORT;
}

//..DECLARATION_MAC_ADDR; //'mac_addresse'

//#if (!LWIP_DHCP)
//DECLARATION_LOCAL_IPADDR;
//#endif

#ifdef ETHERNETIF_COPY
void lwip_copy_ip_addresse(uint8_t *ipadr)
{
	int i = mstep_get_net_index();
	memcpy(ipadr, mstep_eth_ip(), ADDR_LENGTH);
}
void lwip_copy_qw_addresse(uint8_t *ipadr)
{
	int i = mstep_get_net_index();
	memcpy(ipadr, mstep_eth_gw(), ADDR_LENGTH);
}
void lwip_copy_mask_addresse(uint8_t *ipadr)
{
	int i = mstep_get_net_index();
	memcpy(ipadr, mstep_eth_mask(), ADDR_LENGTH);
}
#else
const uint8_t *lwip_get_ip_addresse(void) {
	//int i = mstep_get_net_index(); //&cfg_local_ipe[i][0];
	//return mstep_eth_ip();
	//uint8_t *dev;
	//int i = mstep_get_net_index(); //&cfg_local_ipe[i][0];
	//dev = &cfg_local_ipe[i][0];
	//printf("cfg_local_ipe[%d]: %d %d %d %d\r\n", i, dev[0], dev[1], dev[2], dev[3]);
	//dev = mstep_eth_ip();
	//printf("mstep_eth_ip()[%d]: %d %d %d %d\r\n", mstep_get_net_index(), dev[0], dev[1], dev[2], dev[3]);
	
	return mstep_eth_ip();
}
const uint8_t *lwip_get_qw_addresse(void) {
	//int i = mstep_get_net_index(); //&cfg_local_gwe[i][0];
	//return mstep_eth_mask(); -wrong
	//uint8_t *dev;
	//int i = mstep_get_net_index(); //&cfg_local_gwe[i][0];
	//dev = &cfg_local_gwe[i][0];
	//printf("cfg_local_gwe[%d]: %d %d %d %d\r\n", i, dev[0], dev[1], dev[2], dev[3]);
	//dev = mstep_eth_gw();
	//printf("mstep_eth_gw()[%d]: %d %d %d %d\r\n", mstep_get_net_index(), dev[0], dev[1], dev[2], dev[3]);
	
	return mstep_eth_gw();
}
const uint8_t *lwip_get_mask_addresse(void) {
	//int i = mstep_get_net_index(); //&cfg_local_maske[i][0];
	//return mstep_eth_gw(); -wrong
	//uint8_t *dev;
	//int i = mstep_get_net_index(); //&cfg_local_ipe[i][0];
	//dev = &cfg_local_maske[i][0];
	//printf("cfg_local_maske[%d]: %d %d %d %d\r\n", i, dev[0], dev[1], dev[2], dev[3]);
	//dev = mstep_eth_mask();
	//printf("mstep_eth_mask()[%d]: %d %d %d %d\r\n", mstep_get_net_index(), dev[0], dev[1], dev[2], dev[3]);
	
	return mstep_eth_mask();
}
#endif

//void lwip_get_mac_addresse0(uint8_t *adr){
//  memcpy(adr, &mac_addresse[mstep_get_net_index()][0], MAC_ADDR_LENGTH);
//}

//uint8_t *lwip_get_mac_addresse1(void){
	#if 0
	//uint8_t *p = &mac_addresse[mstep_get_net_index()][0];
	//printf("lwip_get_mac_addresse1 -> mac_addresse[%d][0]\r\n", mstep_get_net_index());
	//printf("ptr is %02x%02x%02x%02x%02x%02x, ", *p++, *p++,*p++,*p++,*p++,*p++);
	//printf("array is %02x%02x%02x%02x%02x%02x\r\n", mac_addresse[mstep_get_net_index()][0],
		//mac_addresse[mstep_get_net_index()][1],mac_addresse[mstep_get_net_index()][2],
		//mac_addresse[mstep_get_net_index()][3],mac_addresse[mstep_get_net_index()][4],
		//mac_addresse[mstep_get_net_index()][5]);
	#endif
	
//	return &mac_addresse[mstep_get_net_index()][0];
//}

#if MQTT_CLIENT_SUPPORT
	int publish_handle_expire(uint32_t expr_time)
	{
	#if DBG_TEST_NO_PUBLISH_EXPIRE
		return 0;
	#else
		//.return ((get_publish_expr_time(i) != 0) && 
		//.		(all_local_time >= get_publish_expr_time(i)));
		return expr_time && (all_local_time >= expr_time);
	#endif
	}

	int get_publish_first(int i, uint16_t newflgs)
	{
		return ((1 << i) & newflgs) ? 1 : 0;
	}

	int clr_publish_first(int i, uint16_t newflgs)
	{
		//newflgs &= ~(1 << i);
		return newflgs & ~(1 << i);
	}

	void publish_handle_new(int i, uint16_t times, int mqtt_connected)
	{
		static uint16_t publishnewflgs = 0xffff;
		
		if (!mqtt_connected) {
			printf("%s, mqtt new plsh .[%d] %s, [set %u ms]\r\n", 
				"not-connt", i, get_publish_topic(i), times); // right flow, to get this print-out line, not happen!
			return;
		}
		
	  #if 1
		#if MQTT_CLIENT_CONNECT_QUICK_PUBLISHS
		//[func.str]
		if (times != get_publish_send_ms(i) && get_publish_first(i, publishnewflgs)) { //only the start.sx publish handle new
			publishnewflgs = clr_publish_first(i, publishnewflgs);
			
			printf("[schl topic%02d %5u ms] qos %d %s, class ranking .[%07d]\r\n", 
				i, times, get_publish_qos(i), get_publish_topic(i), i * 100000);
		}
		#endif
		
		if (times == get_publish_send_ms(i) && get_publish_first(i, publishnewflgs)) {
			publishnewflgs = clr_publish_first(i, publishnewflgs);
			
			printf(".schl .publish %02d.topic len %3d  %5u ms   qos %d %s\r\n", i, strlen(get_publish_topic(i)), times, get_publish_qos(i), get_publish_topic(i)); //" %s", get_publish_topic(i) //", enum-msg [%07d], i * 100000"
		}
		//.printf("db.s %d. publishnewflgs %4x\r\n", i, publishnewflgs);
		//.printf("db.e %d. publishnewflgs %4x\r\n", i, publishnewflgs);
	  #endif
		
		set_publish_disp_time(i, times); //_publish_init_timer[i] = times; //save
		set_publish_expr_time(i, all_local_time + (uint32_t) times); //publish_send_timer[i] = all_local_time + (uint32_t) times;

		//.if (mqtt_connected) printf("set plsh hdlr[%d] time [%u ms]\r\n", i, times);
	}

	void publish_handle_news(char *headstr)
	{
		if (get_mqtt_publish_topic_number()) {
			
			if (headstr)
				printf("+ [%s, start-schl.]\r\n", headstr);
			
			#if MQTT_CLIENT_CONNECT_QUICK_PUBLISHS
			quick_publish_head_str();
			//[head.str]
			#endif
			
			#if MQTT_MAIN_CLIENT_DO_PUBLISH
			#if MQTT_CLIENT_CONNECT_QUICK_PUBLISHS
			//[function.ops]
			do {
			  int i;
			  uint16_t compan, times;
			  for (i = 0; i < get_mqtt_publish_topic_number(); i++) {
				compan = (!i) ? get_link_handler_ms()/2 : 0; //:"LINK_HANDLER_MS /2"
				times = (get_link_handler_ms()*i) + compan; //:"LINK_HANDLER_MS"
				publish_handle_new(i, times, 1);
			  }
			  printf("\r\n");
			} while(0);
			#else
			do {
			  int i;
				
			  /*for (i = 0; i < get_mqtt_publish_topic_number(); i++)
				//if (mqttc_is_connected()) // may disconnect during inside "mqttc_publish_direct(i)"
				publish_handle_new(i, get_publish_send_ms(i), 1);*/
			  //printf("+\r\n");
			  for (i = 0; (i < 2) && (i < get_mqtt_publish_topic_number()); i++) {
				//.if (i == 0)
				//.	printf("+\r\n");
				publish_handle_new(i, get_publish_send_ms(i), 1);
			  }
			  //printf("+\r\n");
			  for (i = 2; i < get_mqtt_publish_topic_number(); i++) {
				//if (i == 2)
				//	printf("+\r\n");
				publish_handle_new(i, get_publish_send_ms(i), 1);
			  }
			  //printf("+\r\n");
			} while(0);
			#endif
			#endif
		}
	}
#endif
