#include "lwip/opt.h" // for sys_timeouts_init().
#include "lwip/timeouts.h" // for sys_timeouts_init().

#include "tmr_init.h" //"gadge/dm9051_env_lw.h"
#include "main_malloc.h"
#include "dm9051_lw_conf.h"
#include "netconf.h"
#include "testproc/testproc_lw.h"
int n_verify_id = 0;

void lwip_initialize(void)
{
  char *prefix_str;
  int i;
  /* Initialize the LwIP stack */

  #define PROGRAMMER_TEST_LEN	MMALLOC_MAX_LEN //3KB --> 1kb
  prefix_str = (char *) malloc(PROGRAMMER_TEST_LEN); //3KB
	
  //bannerline();
  sprintf(prefix_str, "%d/%d init!!", 0, 0);
  printf(".###. %s [ %lx = malloc( %d ) for startup test for a reference can according to]\r\n", prefix_str, prefix_str, PROGRAMMER_TEST_LEN);
  
  bannerline();
  printf(": lwip_init()\r\n");
	
  lwip_init(); //.SUIT FOR SHARED
	
  for (i = 0; i < ETHERNET_COUNT; i++)
  {
	mstep_set_net_index(i);
    tcpip_stack_init();
  }
  
  //bannerline();
  sprintf(prefix_str, "%d/%d init!!", 0, 0);
  printf(".###. %s [free( %lx ), for startup test for a reference can according to]\r\n", prefix_str, prefix_str);
  
  free(prefix_str);
  
	#if NON_CHIPID_STOP == 1
	if (n_verify_id != ETHERNET_COUNT) {
		bannerline();
		printf("Chip ID CHECK experiment! Fail!!\r\n");
		printf("while(1)\r\n");
		while(1) ; //Feature attribute experiment!!
	}
	#endif
	#if VER_CHIPID_ONLY
	bannerline();
	printf("Chip ID CHECK experiment! Succeed OK\r\n");
	printf("while(1)\r\n");
	while(1) ; //Feature attribute experiment!!
	#endif
}

void lwip_reinitialize(void)
{
  int i;
 //#if (LWIP_TESTMODE == 1)
  //anythings, others 
  //lwip_init
  //sys_timeouts_init()
  PUT_proc_runs_init_SYS_TIMEOUTS_INIT(); //=sys_timeouts_init();
  //such as httpd_init()
  //... , could put here!
	
  for (i = 0; i < ETHERNET_COUNT; i++)
  {
	mstep_set_net_index(i);
    since_tcpip_stack_init(); //~setarpflag_for_ever_netif_set_up();
  }
  
 //#endif
}
