#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "dm9051_lw_testproc_type.h"
#include "dm9051_lw_testproc.h"
#include "tmr_init.h"

int test_line7_enter_check_setup = 0; //.

int in_time_link_up_found(void)
{
  //uint8_t nsr;
  int i = 0;
  while(i++ < 20) {
	uint16_t bmsr = dm9051_bmsr_update(); //dm9051_link_update(); //nsr = spi_read_reg(DM9051_NSR);
	if (bmsr & PHY_LINKED_BIT) { //(nsr & NSR_LINKST)
		if (i > 1) printf(". link is up\r\n");
		return 1;
		//self_test_tx_and_rx(); //test...
		//break;
	}
	if (!(bmsr & PHY_LINKED_BIT)) { //(nsr & NSR_LINKST)
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
	const uint8_t addr_meanless[6] = { 0 }; //no exactly need.

	for (i = 0; i < ETHERNET_COUNT; i++) { //get_eth_interfaces()
		mstep_set_net_index(i); //set_pin_code(i);
		dm9051_poweron_rst();
		dm_delay_ms(1);
		//id = 
		id = dm9051_init(addr_meanless);
		dm9051_board_irq_enable();
		dm9051_start(addr_meanless);
		//display_verify_chipid("dm9051_init", mstep_spi_conf_name(), id);
	}
	mstep_set_net_index(0);
	return 0;
#elif (ETHERNET_COUNT == 1) 
	//
	// test init dual only need while only 1 ethernet port exist.
	//
	
	//=test_dm9051_init_dual(MACaddr);
	//uint16_t id;

	dm9051_poweron_rst();
	dm_delay_ms(1);
	//id = 
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
  bench_test_tx_and_rx(); //test_ethernetif_txrx();
  printf(": test end\r\n");
  //printf(": while(1);\r\n");
#elif (ETHERNET_COUNT == 1)
  //
  // my debug test!
  //
	
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

  if (in_time_link_up_found())
	  self_test_tx_and_rx(); //test...
	
  cspi_write_reg(DM9051_RCR, rcr);
  rcr = cspi_read_reg(DM9051_RCR);
  printf(": .e bench_test rcr %02x to %02x\r\n", rcrn, rcr);
  //restore.done

  printf(": test end\r\n");
  //printf(": while(1);\r\n");
#endif
}

void testproc_run(void)
{
  //at32_dm9051_tmr_init(); //dm9051_tmr_init(); //old: env_dm9051_tmr_init()
	
  printf(": while(1);\r\n"); //since followed by a empty-while-loop.
  while(1)
  {
  }
}

void testproc_board_initialize(void)
{
  printf("\r\n");
  printf("- dm9051_board_initialize [%d spi board(s), 'devconf' as %s]\r\n", mstep_conf_spi_count(), mstep_conf_type());
  printf("- dm9051_board_initialize [%d eth device(s)]\r\n", ETHERNET_COUNT);
  printf("\r\n");
  dm9051_board_initialize(); //netif_create(&ethernetif_create); //at32_dm9051_init_configuration_all(); //env_dm9051f_system_init();
  printf("\r\n");
}
