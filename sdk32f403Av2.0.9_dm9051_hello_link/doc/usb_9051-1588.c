/*
 * Set LED - program to control a USB LED device
 * from user space using libusb
 *
 * Copyright (C) 2004
 *     Greg Kroah-Hartman (greg@kroah.com)
 *
 * This program is free software; you can
 * redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the
 * License.
 *
 * 1. 06/02/2009 by JF_2
 *    control-c key
 * 2. 06/12/2009 by JF_3
 *    control-c key for RX
 *    TX checksum enable
 * 3. 07/22/2009 by JF_4
 *    add back-pressure function
 * 4. 07/29/2009 by JF_5
 *    fix tx chksum pkt data location
 * 5. 11/24/2009 by JF_6
 *    to enumerate all DM9601/9620/9621 device
 * 6. 01/07/2009 by JF_7
 *    set/clear feature for wakeup enable
 * 7. 01/14/2009 by JF_8
 *    show RX packet data
 * 8. 02/10/2010 by JF_9
 *    test WOL sample frame event
 * 9. 08/25/2010 by JF_10
 *    USB multi TX packets in FPGA test
 *10. 10/11/2010 by JF_11
 *    Ethernet MAC TX/RX LoopBack test
 *11. 03/22/2012 by JF_12
 *    add check VID only, no PID checking
 *    dev_mode is 5
 *12. 12/26/2012 by JF_13
 *    no RX interrupt in mTX mode
 *13. 12/28/2012 by JF_14
 *    check Bo err in mTX mode
 *14. 01/08/2013 by JF_14B
 *    more parameter for mTX mode
 *15. 01/29/2013 by JF_14C
 *    chk BO err for sTX mode
 *
 * for DM9620 to dm9051
 * 1. 07/08/2014 by JFW_0
 *    use DM9620 GP2.3-0 as SPI_CSB/CK/MO/SO
 *    unsigned char dm_rd_reg (unsigned int regx)
 *    void          dm_wr_reg (unsigned int regx, unsigned datax)
 * 2. 10/24/2014 by JFW_1
 * change to
 * unsigned int dm_rd_reg(struct usb_dev_handle *handle, unsigned char reg, int length, void* data)
 * unsigned int dm_wr_reg(struct usb_dev_handle* handle, unsigned char reg, int length, void *data)
 * 3. 08/09/20165by JFW_2
 *    fix display memory data in rd_mem()
 * change to
 *=================================================
 * for dm9051d
 * 1. 03/03/2023 by JFD_1
 *    for T1S PHY page 1/3/1f registers access
 * 2. 04/11/2023 by JFD_2
 *    add tx_pkt option: no wr TX buf to save SPI access time
 */
#include <stdio.h>
#include <string.h>
#include <usb.h>
#include <unistd.h>
#include <signal.h>

#define SZBUF  100
#define SHOW_ALL  0
#define READ1     0
#define WRITE1    0
#define READ_REG  1
#define WRITE_REG 2
#define WR_MEMORY 3
#define RD_MEMORY 4
#define TX_PACKET 5
#define RX_PACKET 6
#define GET_DESC  7
#define GET_STRING  8
#define GET_INTERRUPT  9
#define TEST_CLR_WAKEUP  10
#define TEST_SET_WAKEUP  11
#define TEST_WOL_SAMPLE  12 /* JF_9 */
#define TEST_EEP_RD   17
#define TEST_EEP_WR   18
#define TEST_PHY_RD   19
#define TEST_PHY_WR   20
#define TEST_LOOPBACK 21
#define TEST_E_FUSE_RD 22
#define TEST_E_FUSE_WR 23
//Stone add for 1588
#define TEST_1588_SET_TIME 24
#define TEST_1588_GET_TIME 25
#define TEST_1588_GET_TX_TIME 26
#define TEST_1588_GET_RX_TIME 27
#define TEST_1588_GPIO_TRIGGER 28
#define TEST_1588_68_REG_R 29
#define TEST_1588_68_REG_W 30
#define TEST_GP_12 31
#define TEST_1588_RATE 32
#define TEST_1588_OFFSET 33
#define TEST_1588_CRYPT 35

//Stone add for TX continued buffer
#define TEST_TX_CON 36

#define LOAD_FIRMWARE   0
#define SHOW_SYNTAX  99
#define SHOW_STATUS  98
#define NO_CLAIM -1
#define INVALID_EP -1
#define MAX_BURST_SIZE 10000
#define MAX_RX_SIZE 27000

int  BULK_SIZE = 512;
int  tx_mode=0;
int  save_mode=0;
int  test_mode=1;
int  ask_lb_size=0;
unsigned char reg_no;
unsigned int  wdata;
unsigned int  wdata2;
unsigned char eep_page = 0;
unsigned char ncr_val = 0x88;
unsigned char tcr_val = 0x00;
unsigned char rcr_val = 0x0b;
unsigned char imr_val = 0x80;
unsigned char fcr_val = 0x30;
unsigned char reg3d_val = 0x00;
unsigned char reg31_val = 0x00;
unsigned char reg32_val = 0x00;
int     phy_adr=0;
int     step_test = 0;
int     exit_test = 0;
int     lb_mode   = 0;
int     skip_data = 0;
int     rand_data = 0;
int     can_tx    = 0;
int     mode_9620 = 0;
int     speed_full = 0;
int     RXSZBUF = 512;
int     tx_chk_rx = 0;
int     use_interrupt = 0;
int    rx_rst = 0;
int    rx_rst_ctr = 0;
int    err1_header9620 = 0;
int    err2_rx_size = 0;
int    err3_rx_1size = 0;
int    err4_bi_size = 0;
int    show_rx_msg  = 0;
int    show_rx_data = 0;
int    rx_header_type = 0; // JF_2
int    fix_data_pat = 0; // JF_2
int    rd_phy_one;       // JFD_1
int    wr_txbuf_en  = 1; // JFD_2
unsigned char mpkt_buf[MAX_BURST_SIZE];
unsigned char pkt2_buf[4100];
unsigned char pkt1_buf[4100];
unsigned char intr_buf[10];
unsigned char rx_buf[MAX_RX_SIZE+520];
// JF_2
unsigned long tx_cnt;
unsigned long rx_pkt_ctr;
unsigned int tx_len;
unsigned int tx_reset_ctr;
unsigned int rx_crc_ctr = 0;
unsigned int rx_over_ctr = 0;
unsigned int ip_sum_err_ctr = 0;
unsigned int tcp_sum_err_ctr = 0;
unsigned int udp_sum_err_ctr = 0;
    // JF_6
    #define MAX_DEV_CNT   8
    int  dev_ctr = 0;
    int  dev_mode[MAX_DEV_CNT];
    int  pid_name[MAX_DEV_CNT]; // JF_12
    int  product_id=0x9601;     // JF_12
    struct usb_device *dev_tab[MAX_DEV_CNT];


unsigned int tx_chksum_en= 0;
unsigned int no_bprs= 0;
unsigned int multi_tx_pkt = 0;

//Stone add for 1588
int read_8 =0;
int write_8 =0;
unsigned char crypt_key;


void check_arg(int argc, char *argv[]);
void exec_test();
void show_status();
void show_syntax();
void show_reg();
void wr_reg();
void tx_pkt();
void rx_pkt();
void show_rx();
void read_desc();
void read_string();
void read_interrupt();
void show_eep();
void test_wr_eep();
void show_phy();
void show_phy_p0();  // JFD_1
void show_phy_p1();  // JFD_1
void show_phy_p3();  // JFD_1
void show_phy_p31(); // JFD_1
void test_wr_phy();
void wr_eep(unsigned char reg_no,unsigned int wdata);
int rd_eep(unsigned char num);
void wr_phy(unsigned int page, unsigned int addr,unsigned int wdata); // JFD_1
int rd_phy(unsigned int page, unsigned int addr); // JFD_1
void wr_phy_fm_nic(unsigned char reg_no,unsigned int wdata);
int rd_phy_fm_nic(unsigned char num);
void wait_phy_end();
void sw_rst_init();
void exit_xx(int exit_codex);
void wr_mem();
void rd_mem();
struct usb_device *show_sel(void);

//Stone add for 9051 sample frame test
void test_WOL_sample();
void sample_frame_mask();

// JFW_0
//void dm_wr_reg (unsigned int regx, unsigned datax);
unsigned int dm_wr_reg(struct usb_dev_handle* handle, unsigned char reg, int length, unsigned char *data); // JFW_1
void wr_gpio_byte(unsigned char regx);
//unsigned char dm_rd_reg (unsigned int regx);
unsigned int dm_rd_reg(struct usb_dev_handle *handle, unsigned char reg, int length, unsigned char *data); // JFW_1
unsigned int dm_wr_spi_mems(struct usb_dev_handle* handle, unsigned int  reg, int length, unsigned char *data); // JFW_1
unsigned int dm_rd_spi_mems(struct usb_dev_handle* handle, unsigned int  reg, int length, unsigned char *data); // JFW_1
unsigned char rd_gpio_byte ();
void make_tx_buf();
void chk_rx();
int    mem_tst      = 0; // JFW_1
unsigned int tx_len_9000_mode = 1;
unsigned int tx_index;
unsigned int rx_rd_mem_adr = 0x2800;
int    chk_rxd      = 0; // HOST_8
int    exit_rx_err  = 0; // HOST_3
int    echo_rx      = 0; // HOST_3
int    tx_chk_isr   = 0; // HOST_5
unsigned long rx_pause_pkt_ctr; // HOST_8B



/* control requests */
#define DM_READ_REGS	0x00
#define DM_WRITE_REGS	0x01
#define DM_READ_MEMS	0x02
#define DM_WRITE_REG	0x03
#define DM_WRITE_MEMS	0x05
#define DM_WRITE_MEM	0x07/* control requests */

#define CLEAR_FEATURE   0x01
#define SET_FEATURE     0x03
#define GET_DESCRIPTOR  0x06

/* registers */
#define DM_NET_CTRL	0x00
#define DM_RX_CTRL	0x05
#define DM_SHARED_CTRL	0x0b
#define DM_SHARED_ADDR	0x0c
#define DM_SHARED_DATA	0x0d	/* low + high */
#define DM_PHY_ADDR	0x10	/* 6 bytes */
#define DM_MCAST_ADDR	0x16	/* 8 bytes */
#define DM_GPR_CTRL	0x1e
#define DM_GPR_DATA	0x1f

#define DM_MAX_MCAST	64
#define DM_MCAST_SIZE	8
#define DM_EEPROM_LEN	256
#define DM_TX_OVERHEAD	2	/* 2 byte header */
#define DM_RX_OVERHEAD	7	/* 3 byte header + 4 byte crc tail */
#define DM_TIMEOUT	1000



/* registers */
#define DM_NET_CTRL	0x00
#define DM_RX_CTRL	0x05
#define DM_SHARED_CTRL	0x0b
#define DM_SHARED_ADDR	0x0c
#define DM_SHARED_DATA	0x0d	/* low + high */
#define DM_PHY_ADDR	0x10	/* 6 bytes */
#define DM_MCAST_ADDR	0x16	/* 8 bytes */
#define DM_GPR_CTRL	0x1e
#define DM_GPR_DATA	0x1f

#define DM_MAX_MCAST	64
#define DM_MCAST_SIZE	8
#define DM_EEPROM_LEN	256
#define DM_TX_OVERHEAD	2	/* 2 byte header */
#define DM_RX_OVERHEAD	7	/* 3 byte header + 4 byte crc tail */
#define DM_TIMEOUT	1000

#define VENDOR_ID   0x0a46
#define P_ID_9601   0x9601
#define P_ID_9620   0x9620
#define P_ID_9621   0x9621


	struct usb_device *usb_dev;
	struct usb_dev_handle *usb_handle;
	int retval = 0;
        int ep_bulk_out = INVALID_EP;
        int ep_bulk_in = INVALID_EP;
        int ep_intr = INVALID_EP;
        int claimed_interface = NO_CLAIM;



void print_endpoint(struct usb_endpoint_descriptor *endpoint)
{
	printf("      bEndpointAddress: %02xh\n", endpoint->bEndpointAddress);
	printf("      bmAttributes:     %02xh\n", endpoint->bmAttributes);
	printf("      wMaxPacketSize:   %d\n", endpoint->wMaxPacketSize);
	printf("      bInterval:        %d\n", endpoint->bInterval);
	printf("      bRefresh:         %d\n", endpoint->bRefresh);
	printf("      bSynchAddress:    %d\n", endpoint->bSynchAddress);
}

void print_altsetting(struct usb_interface_descriptor *interface)
{
	int i;

	printf("    bInterfaceNumber:   %d\n", interface->bInterfaceNumber);
	printf("    bAlternateSetting:  %d\n", interface->bAlternateSetting);
	printf("    bNumEndpoints:      %d\n", interface->bNumEndpoints);
	printf("    bInterfaceClass:    %d\n", interface->bInterfaceClass);
	printf("    bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
	printf("    bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
	printf("    iInterface:         %d\n", interface->iInterface);

	for (i = 0; i < interface->bNumEndpoints; i++)
		print_endpoint(&interface->endpoint[i]);
}

void print_interface(struct usb_interface *interface)
{
	int i;

	for (i = 0; i < interface->num_altsetting; i++)
		print_altsetting(&interface->altsetting[i]);
}

void print_configuration(struct usb_config_descriptor *config)
{
	int i;

	printf("  wTotalLength:         %d\n", config->wTotalLength);
	printf("  bNumInterfaces:       %d\n", config->bNumInterfaces);
	printf("  bConfigurationValue:  %d\n", config->bConfigurationValue);
	printf("  iConfiguration:       %d\n", config->iConfiguration);
	printf("  bmAttributes:         %02xh\n", config->bmAttributes);
	printf("  MaxPower:             %d\n", config->MaxPower);

	for (i = 0; i < config->bNumInterfaces; i++)
		print_interface(&config->interface[i]);
}



static int get_desc(struct usb_dev_handle *handle, unsigned char reg, int length, void* data)
{

	return usb_control_msg(handle,
			       0x00000080,
			       GET_DESCRIPTOR,
			       (0xff & reg) << 8,
			       0,
			       data,
			       length,
			       5000);

}

static int get_string(struct usb_dev_handle *handle, unsigned char reg, int length, void* data)
{

	return usb_control_msg(handle,
			       0x00000080,
			       GET_DESCRIPTOR,
			       ((0xff & reg) ) + 0x0300,
			       0x0904,
			       data,
			       length,
			       5000);

}
static int dm_read(struct usb_dev_handle *handle, unsigned char reg, int length, void* data)
{

	return usb_control_msg(handle,
			       0x000000c0,
			       DM_READ_REGS,
			       0,
			       0xff & reg,
			       data,
			       length,
			       5000);

}

static int dm_write(struct usb_dev_handle* handle, unsigned char reg, int length, void *data)
{

	return usb_control_msg(handle,
			       0x00000040,
			       DM_WRITE_REGS,
			       0,
			       0xff & reg,
			       data,
			       length,
			       5000);
}
/*
static int dm_wr_mems(struct usb_dev_handle* handle, unsigned int  reg, int length, void *data)
{

	return usb_control_msg(handle,
			       0x00000040,
			       DM_WRITE_MEMS,
			       0,
			       reg,
			       data,
			       length,
			       5000);
}

static int dm_rd_mems(struct usb_dev_handle* handle, unsigned int  reg, int length, void *data)
{

	return usb_control_msg(handle,
			       0x000000C0,
			       DM_READ_MEMS,
			       0,
			       reg,
			       data,
			       length,
			       5000);
}
*/
// JF_7
static int clr_feature(struct usb_dev_handle* handle, unsigned int  reg, int length, void *data)
{

	return usb_control_msg(handle,
			       0x00000000,
			       CLEAR_FEATURE,
			       reg,
			       0,
			       data,
			       length,
			       5000);
}
static int set_feature(struct usb_dev_handle* handle, unsigned int  reg, int length, void *data)
{

	return usb_control_msg(handle,
			       0x00000000,
			       SET_FEATURE,
			       reg,
			       0,
			       data,
			       length,
			       5000);
}
static struct usb_device *dm_device_init(void)
{
	struct usb_bus *usb_bus;
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();
        dev_ctr = 0; // JF_6

	for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next)
        {
	    for (dev = usb_bus->devices; dev; dev = dev->next) 
            {
		if ((dev->descriptor.idVendor == VENDOR_ID) &&
		    (dev->descriptor.idProduct == P_ID_9601))
                {
                //  mode_9620 = 0;
		//  return dev;
                    dev_mode[dev_ctr] = 0;
		    dev_tab[dev_ctr] = dev;
		    dev_ctr++;
		}
		else
		if ((dev->descriptor.idVendor == VENDOR_ID) &&
		    (dev->descriptor.idProduct == P_ID_9620))
                {
                //  mode_9620 = 1;
		//  return dev;
                    dev_mode[dev_ctr] = 1;
		    dev_tab[dev_ctr] = dev;
		    dev_ctr++;
		}
		else
		if ((dev->descriptor.idVendor == VENDOR_ID) &&
		    (dev->descriptor.idProduct == P_ID_9621))
                {
                //  mode_9620 = 2;
		//  return dev;
                    dev_mode[dev_ctr] = 2;
		    dev_tab[dev_ctr] = dev;
		    dev_ctr++;
		}
                else // JF_12
		if ((dev->descriptor.idVendor == VENDOR_ID))
                {
                    dev_mode[dev_ctr] = 5;
                    dev_tab[dev_ctr] = dev;
		    pid_name[dev_ctr] = dev->descriptor.idProduct; // JF_12
                    product_id = dev->descriptor.idProduct; // JF_12
                    dev_ctr++;
                }
	        else
		if(dev_ctr >= MAX_DEV_CNT)
		{
			printf("ERROR: MAX DEV CUNTER\n");
		        exit(0);
		}	
  	    }
	}
	return NULL;
}


unsigned char* mk_raw(unsigned char* buf, int size)
{
	/* make raw data 
	   input : pointer to empty buffer
	   output: pointer to payload data
	*/
	int sz = size;
	int i;
        u_char *p = buf;


	for (i=0; i<sz; i++)
 		p[i] = i;
	
	return buf;
}
// JF_2
//============================================================
// control-C for tx_pkt()
//============================================================
void ctrl_c_tx_pkt(int sig)
{
    printf("end of TX packet %ld size:%d\n",tx_cnt,tx_len);
    printf("rx_ctr=%ld, crc=%d, overflow=%d\n", 
           rx_pkt_ctr, rx_crc_ctr, rx_over_ctr);
    printf("tx_reset_ctr:%d\n",tx_reset_ctr);
    printf("rst_ctr=%d, hd9620=%d, size=%d, 1st_size=%d, bi_size=%d\n",
                       rx_rst_ctr,
                       err1_header9620,
                       err2_rx_size,
                       err3_rx_1size,
                       err4_bi_size
		      );
    printf("RX CHKSUM error IP=%d, TCP=%d, UDP=%d\n",
           ip_sum_err_ctr, tcp_sum_err_ctr, udp_sum_err_ctr);
    exit(1);
}

// JF_3
//============================================================
// control-C for rx_pkt()
//============================================================
void ctrl_c_rx_pkt(int sig)
{
    printf("rx_ctr=%ld, crc=%d, overflow=%d\n", 
           rx_pkt_ctr, rx_crc_ctr, rx_over_ctr);
    printf("rst_ctr=%d, hd9620=%d, size=%d, 1st_size=%d, bi_size=%d\n",
                       rx_rst_ctr,
                       err1_header9620,
                       err2_rx_size,
                       err3_rx_1size,
                       err4_bi_size
		      );
    printf("RX CHKSUM error IP=%d, TCP=%d, UDP=%d\n",
           ip_sum_err_ctr, tcp_sum_err_ctr, udp_sum_err_ctr);
    exit(1);
}

//============================================================
// main program
//============================================================
int main(int argc, char **argv)
{
        int i,j,k,m;
        unsigned char reg;
        unsigned short length;
	unsigned char buf[SZBUF];
	int claimed_interface = NO_CLAIM;

//	usb_dev = dm_device_init();
	dm_device_init();
	memset(buf, 0, SZBUF);
//	if (usb_dev == NULL) {
	if (dev_ctr == 0) {
		fprintf(stderr, "Device not found\n");
		goto exit;
	}
	else
	{
		usb_dev = show_sel();
	}
	
	usb_handle = usb_open(usb_dev);
	if (usb_handle == NULL) {
		fprintf(stderr,
			"Not able to claim the USB device\n");
		goto exit;
	}
	
        /*
         * find b-in , b-out & intr endpoint addresses
         * and claim interface before bulk tests
         */
        {
                struct usb_config_descriptor *config;
                struct usb_interface *interface;
                struct usb_interface_descriptor *alternate;
                struct usb_endpoint_descriptor *endpoint;

                for (i = 0; i < usb_dev->descriptor.bNumConfigurations; i++)
                {
                    config = &usb_dev->config[i];
                    for (j = 0; j < config->bNumInterfaces; j++)
                    {
                        interface = &config->interface[j];
                        for (k = 0; k < interface->num_altsetting; k++)
                        {
                            alternate = &interface->altsetting[k];
                            if (alternate->bNumEndpoints > 0)
                            {
                            /* must claim interface before bulk-in/out,
                            * also detatch the original driver before interface
                            * claimation
                            */
                              usb_detach_kernel_driver_np(usb_handle, k);

                                /* must claim interface before bulk-in/out*/
                                retval = usb_claim_interface(usb_handle , j);
                                if (retval <0) printf("%s\n", usb_strerror());
                                else claimed_interface = j;
                            }
                            for (m = 0; m < alternate->bNumEndpoints; m++)
                            {
                                endpoint = &alternate->endpoint[m];
                                if (endpoint->bEndpointAddress & 0x80)
                                { //direction IN
                                    if ((endpoint->bmAttributes & 0x3) == 0x2) // Bulk
                                        ep_bulk_in = endpoint->bEndpointAddress;
                                    else ep_intr = endpoint->bEndpointAddress;
                                }
                                else
                                { // direction OUT
                                    ep_bulk_out = endpoint->bEndpointAddress;
                                }

                            } // m
                        } // for k
                    } // for j
                } // for i
        }

        /* check USB speed */
        reg = 0x01; length=0x10;
	get_desc(usb_handle, reg, length, buf);
        if(buf[3]==0x02)
            speed_full = 0;
        else
        if(buf[3]==0x01)
            speed_full = 1;
        else
            printf("unknown USB speed\n");

        /* check USB RX header type */
        reg = 0x91; length=1;
	dm_read(usb_handle, reg, length, buf);
	if((buf[0]&0x80)==0x80)
	    rx_header_type = 1;
	else
	    rx_header_type = 0;

	/* 
	 * start our usb test here... 
	 */
        check_arg(argc,argv);
        exec_test();
exit:
	if (claimed_interface != NO_CLAIM) 
		usb_release_interface(usb_handle, claimed_interface);
	if (usb_handle) 
		usb_close(usb_handle);
       
	return retval;
}

// JF_6
/*************************************************************************/
/*      show all DM96XX devices                                          */
/*************************************************************************/
struct usb_device *show_sel(void)
{
	int i;
	int sel_num;

        if(dev_ctr==1)
            return(dev_tab[0]);

	for(i=0;i<dev_ctr;i++)
	{
		if(dev_mode[i]==0)
		{
			printf("%d: DM9601\n",i);
		}
		else
		if(dev_mode[i]==1)
		{
			printf("%d: DM9620\n",i);
		}
		else
		if(dev_mode[i]==2)
		{
			printf("%d: DM9621\n",i);
		}
		else // JF_12
		if(dev_mode[i]==5)
		{
                        product_id = pid_name[i];
			printf("%d: DM%4X\n",i,product_id);
		}
		else
			printf("%d: ??????\n",i);
	}
	printf("SEL:");
	scanf("%d",&sel_num);
        return(dev_tab[sel_num]);
}


/*************************************************************************/
/*      check argument                                                   */
/*************************************************************************/
void check_arg(int argc, char *argv[])
/*
int argc;
char *argv[];
*/
{
int i;

    if(argc < 2)
    {
        test_mode=SHOW_SYNTAX;
    }
    else
    {
        switch (argv[1][0])
        {
        case '?':
                show_syntax();
                exit(1);
                break;
        case '0':
                test_mode=SHOW_STATUS;
                break;
        case '1':
                test_mode=READ_REG;
                break;
        case '2':
                test_mode=WRITE_REG;
                break;
        case '3':
                test_mode=WR_MEMORY;
                if(argv[1][1]=='t')
                    mem_tst = 1;
                break;
        case '4':
                test_mode=RD_MEMORY;
                break;
        case '5':
                test_mode=TX_PACKET;
                for(i=1;i<6;i++)
                {
                    if(argv[1][i]=='m')
                        show_rx_msg = 1;
                    if(argv[1][i]=='z')
                        tx_chk_rx = 1;
                    if(argv[1][i]=='i')
                        tx_chk_isr   = 1; // HOST_5
                    if(argv[1][i]=='c') // HOST_8
                        chk_rxd = 1;
                    if(argv[1][i]=='n') // JFD_2
                        wr_txbuf_en = 0;
                //  if(argv[1][i]=='b')
                //      tx_len_9000_mode = 1;
                }
                break;
        case '6':
                test_mode=RX_PACKET;
                for(i=1;i<6;i++)
                {
                    if(argv[1][i]=='m')
                        show_rx_msg = 1;
                    if(argv[1][i]=='d')
                        show_rx_data = 1;
                    if(argv[1][i]=='e') // HOST_3
                        exit_rx_err = 1;
                    if(argv[1][i]=='h') // HOST_3
                        echo_rx = 1;
                    if(argv[1][i]=='c') // HOST_8
                        chk_rxd = 1;
                }
                break;

#if 0    //Stone add for 1588

        case '7':
                test_mode=GET_DESC;
                break;

        case '8':
                test_mode=GET_STRING;
                break;

        case '9':
                test_mode=GET_INTERRUPT;
                break;

        case 'a': // JF_7
                test_mode=TEST_CLR_WAKEUP;
                break;

        case 'b':
                test_mode=TEST_SET_WAKEUP;
                break;

        case 'c': /* JF_9 */
                test_mode=TEST_WOL_SAMPLE;
                break;
#endif  
        case '7':
                test_mode=TEST_1588_68_REG_R;
                for(i=1;i<6;i++)
                {
                    if(argv[1][i]=='l')
                     read_8 = 1;
                 }   
                break;
        
        case '8':
                test_mode=TEST_1588_68_REG_W;
                for(i=1;i<6;i++)
                {
                    if(argv[1][i]=='l')
                     write_8 = 1;
                 }   
                
                break;
                
        case '9':
                test_mode=TEST_GP_12;
                break;        
                   
        case 'c':
        case 'C':
                test_mode= TEST_TX_CON;
                break;
                     

        case 'e':
        case 'E':
                eep_page = 0;
                test_mode=TEST_EEP_RD;
                for(i=1;i<3;i++)
                {
                    if((argv[1][i]=='W') | (argv[1][i]=='w'))
                        test_mode=TEST_EEP_WR;
                    if((argv[1][i]>='0') && (argv[1][i]<='F'))
                    {
                        if(argv[1][i] < 0x3a)
                            eep_page = argv[1][i]-'0';
                        else
                            eep_page = argv[1][i]-'A'+10;
                    }
                }
                break;

        case 'f':  /* Stone add for e-fuse */
        case 'F':
                 test_mode=TEST_E_FUSE_RD;   
                 if((argv[1][1]=='W') | (argv[1][1]=='w'))
                    test_mode=TEST_E_FUSE_WR;  

                 break;                  
                

        case 'p':
        case 'P':
                if((argv[1][1]=='W') | (argv[1][1]=='w'))
                    test_mode=TEST_PHY_WR;
                else{ // JFD_1
                     test_mode=TEST_PHY_RD;
                     rd_phy_one = 0;
                     if((argv[1][1]=='1'))
                     { //PHY address > 3
			rd_phy_one = 1;
                     }
                    }
                break;
                
        case 's':
        case 'S':
                 if(argv[1][1]=='1')
                    test_mode=TEST_1588_SET_TIME;
                 if(argv[1][1]=='2')
                    test_mode=TEST_1588_GET_TIME;  
                 if(argv[1][1]=='3')
                    test_mode=TEST_1588_GET_TX_TIME;
                 if(argv[1][1]=='5')
                    test_mode=TEST_1588_GET_RX_TIME;   
                 if(argv[1][1]=='6')
                    test_mode=TEST_1588_GPIO_TRIGGER;
                 if(argv[1][1]=='7')
                    test_mode=TEST_1588_RATE;      
                 if(argv[1][1]=='8')
                    test_mode=TEST_1588_OFFSET;   
                 if(argv[1][1]=='9')
                    test_mode=TEST_1588_CRYPT;   

                 break;                
                
	// JF_11
        case 'l':
        case 'L':
                test_mode=TEST_LOOPBACK;
                for(i=1;i<5;i++)
                {
                    if(argv[1][i]=='s')
		        ask_lb_size = 1;
                    if(argv[1][i]=='m')
                        show_rx_msg = 1;
                }
                break;
                
         case 'w':
         case 'W':
                  test_mode=TEST_WOL_SAMPLE;
                  break;                

            default:
                show_syntax();
                exit(1);
        }
    }


}

/*************************************************************************/
/*      exec. test mode                                                  */
/*************************************************************************/
void exec_test()
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
/*
    if(mode_9620==1)
        printf("Product ID is DM9620, ");
    else
    if(mode_9620==2)
        printf("Product ID is DM9621, ");
    else
    if(mode_9620==5) // JF_12
*/
        printf("Product ID is DM%4X, ",product_id);
/*
    else
        printf("Product ID is DM9601, ");
*/

    if(speed_full)
    {
        printf("full speed\n");
        RXSZBUF = 64;
    }
    else
    {
        printf("high speed\n");
        RXSZBUF = 512;
    }

    reg = 0x34; length=1;
    buf[0]=0x0e;
    retval = dm_write(usb_handle, reg, length, buf);

    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    switch (test_mode)
        {
        case SHOW_STATUS:
                show_status();
                break;

        case READ_REG:
                show_reg();
                break;

        case WRITE_REG:
                wr_reg();
                break;

        case WR_MEMORY:
                wr_mem();
                break;

        case RD_MEMORY:
                rd_mem();
                break;

        case TX_PACKET:
                tx_pkt();
                break;

        case RX_PACKET:
             // if(use_interrupt)
                rx_pkt();
                break;
#if 0    //Stone add for 1588
        case GET_DESC:
                read_desc();
                break;

	case GET_STRING:
                read_string();
                break;

	case GET_INTERRUPT:
                read_interrupt();
                break;

	case TEST_CLR_WAKEUP: // JF_7
	clr_feature(usb_handle, 1, 0, 0);
	if (retval<0) printf("ERR:%s\n", usb_strerror());
                break;

	case TEST_SET_WAKEUP: // JF_7
	set_feature(usb_handle, 1, 0, 0);
	if (retval<0) printf("ERR:%s\n", usb_strerror());
                break;
#endif

        //Stone add for 1588
        case TEST_1588_68_REG_R:
                test_1588_reg_68_rd();
                break;
        
        case TEST_1588_68_REG_W:
                test_1588_reg_68_wd();
                break;
                 

        case TEST_EEP_RD:
                show_eep();
                break;

        case TEST_EEP_WR:
                test_wr_eep();
                break;

        case TEST_PHY_RD:
                show_phy();
                break;

        case TEST_PHY_WR:
                test_wr_phy();
                break;
                
       case TEST_E_FUSE_RD:
                show_e_fuse();         
                break;

       case TEST_E_FUSE_WR:
                test_wr_e_fuse();         
                break;
       case TEST_1588_SET_TIME:
                test_1588_set_time();
                break;
                
       case TEST_1588_GET_TIME:    
                test_1588_get_time();
                break;
                    
       case TEST_1588_GET_TX_TIME:
                test_1588_get_tx_time(); 
                break;   

       case TEST_1588_GET_RX_TIME:
                test_1588_get_rx_time(); 
                break;
                
       case TEST_1588_GPIO_TRIGGER:
                test_1588_gpio_trigger(); 
                break;
                
       case TEST_GP_12:
                test_gp_12();
                break;
                
       case TEST_1588_RATE:
                test_1588_rate();
                break;         

       case TEST_1588_OFFSET:
                test_1588_offset(); 
                break;   
                
       case TEST_1588_CRYPT:          
                test_1588_crypt();
                break;
                
       case TEST_TX_CON:
                test_tx_con();         
                break;
                
       case TEST_WOL_SAMPLE:
                test_WOL_sample();
                break;         
                
        case SHOW_SYNTAX:
                show_syntax();
                break;

        }
}
/*********************************************************************/
/*      show syntax                                                  */
/*********************************************************************/
void show_syntax()
{
//  printf("DM9620 GPIO to SPI V1.1  10/24/2014 by JFCHIU\n");
//  printf("DM9620 GPIO to SPI_DM9051D V1.0  03/03/2023 by JFCHIU\n");
    printf("DM9620 GPIO to SPI_DM9051D V1.1  10/20/2023 by Stone\n");
    printf("command syntax: usb_9051 \{1}\n");
    printf(" ? : display this message\n");
    printf(" 0 : Chip status\n");
    printf(" 1 : RD MAC register\n");
    printf(" 2 : WR MAC register\n");
    printf(" 3 : write memory\n");
    printf(" 4 : read memory\n");
//  printf(" 5 : TX packet l:lpbk, z:RX, m:msg, d:d-pat, c:chksum\n");
//  printf(" 6 : RX packet, m:message, e:exit if error, i:use interrupt, d:show data\n");
//  printf(" 5 : TX packet, z:RX, m:msg, i:chk ISR\n");
    printf(" 5 : TX packet, z:RX, m:msg, i:chk ISR, n:no wr buf\n"); // JFD_2
    printf(" 6 : RX packet, m:message, e:exit if error, d:show data, h:echo\n");
/*
    printf(" 7 : get descriptor\n");
    printf(" 8 : get string\n");
    printf(" 9 : get interrupt data\n");
    printf(" a : clear feature of wakeup\n");
    printf(" b : set   feature of wakeup\n");
    printf(" c : test WOL sample event\n");
*/
    printf(" 7 [l] : 1588 register 0x68 read (index 0-7), l: SPI len = 8\n");
    printf(" 8 [l] : 1588 register 0x68 write (index 0-7), l: SPI len = 8\n");
    printf(" 9 : GP 1/2 output/input setup \n");
    printf(" c : test TX buffer continued mode \n");
    printf(" E : show [0~F] pages EEPROM data\n");
    printf(" EW: write EEPROM data\n");
    //Stone add 
    printf(" F : show  E-Fuse data\n");
    printf(" FW: write E-Fuse data\n");
    
    printf(" P [1] : read PHY registers, 1: phy address > 3 \n");
    printf(" PW: write PHY register\n");
    //Stone add for 1588
    printf(" S [1|2|3|5|6|7|8|9]:test 1588 function, 1: setup ptp clock time, 2: get ptp clock time, 3: get TX timestamp 5: get RX timestamp\n");
    printf("                                         6: test GPIO trigger/event test \n");
    printf("                                         7: Update rate test  \n");    
    printf("                                         8: Update offset test  \n");        
    printf("                                         9: Crypt test  \n");            
    
    //Stone add for 9051 WOL test
    printf(" W : test WOL sample event\n");
//  printf(" L : MAC loopback test, s:tx_size, m:message\n");
}
/*************************************************************************/
/*   show chip status                                                    */
/*************************************************************************/
void show_status()
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];

	/* read register , with error report*/
/*
	reg = 0x2a; length=1;
	retval = dm_rd_reg(usb_handle, reg, length, buf);
	if (retval<0) printf("%s\n", usb_strerror());
	else printf("retval=%d, reg 0x%02x = 0x%02x\n", retval, reg, buf[0]);
*/
        reg = 0x2a; length=2;
	dm_rd_reg(usb_handle, reg, length, buf);
        printf("REG2B_2A=%2x",buf[1]);
        if(buf[0] > 15)
            printf("%2x",buf[0]);
        else
            printf("0%1x",buf[0]);

        reg = 0x28; length=2;
	dm_rd_reg(usb_handle, reg, length, buf);
        printf("  REG29_28=%2x",buf[1]);
        if(buf[0] > 15)
            printf("%2x",buf[0]);
        else
            printf("0%1x",buf[0]);

        reg = 0x40; length=5;
	dm_rd_reg(usb_handle, reg, length, buf);
        printf("  REG40-44=%2x,%2x,%2x,%2x,%2x",
               buf[0],buf[1],buf[2],buf[3],buf[4]);
        printf("\n");

        reg = 0x1e; length=2;
	dm_rd_reg(usb_handle, reg, length, buf);
        printf("  REG1E=%2x",buf[0]);
        printf("  REG1F=%2x",buf[1]);

        reg = 0x10; length=6;
	dm_rd_reg(usb_handle, reg, length, buf);

    printf("  REG10~15=");
    if(buf[0] < 16)
        printf("0%1x:",buf[0]);
    else
        printf("%2x:",buf[0]);
    if(buf[1] < 16)
        printf("0%1x:",buf[1]);
    else
        printf("%2x:",buf[1]);
    if(buf[2] < 16)
        printf("0%1x:",buf[2]);
    else
        printf("%2x:",buf[2]);
    if(buf[3] < 16)
        printf("0%1x:",buf[3]);
    else
        printf("%2x:",buf[3]);
    if(buf[4] < 16)
        printf("0%1x:",buf[4]);
    else
        printf("%2x:",buf[4]);
    if(buf[5] < 16)
        printf("0%1x",buf[5]);
    else
        printf("%2x",buf[5]);


    printf("\n");

        reg = 0x20; length=2;
	dm_rd_reg(usb_handle, reg, length, buf);
        printf("REG21_20(TX_WR)=%2x",buf[1]);
        if(buf[0] > 15)
            printf("%2x",buf[0]);
        else
            printf("0%1x",buf[0]);

        reg = 0x24; length=2;
	dm_rd_reg(usb_handle, reg, length, buf);
        printf("   REG25_24(RX_WR)=%2x",buf[1]);
        if(buf[0] > 15)
            printf("%2x",buf[0]);
        else
            printf("0%1x",buf[0]);
        printf("\n");

        reg = 0x22; length=2;
	dm_rd_reg(usb_handle, reg, length, buf);
        printf("REG23_22(TX_RD)=%2x",buf[1]);
        if(buf[0] > 15)
            printf("%2x",buf[0]);
        else
            printf("0%1x",buf[0]);

        reg = 0x26; length=2;
	dm_rd_reg(usb_handle, reg, length, buf);
        printf("   REG27_26(RX_RD)=%2x",buf[1]);
        if(buf[0] > 15)
            printf("%2x",buf[0]);
        else
            printf("0%1x",buf[0]);
        printf("\n");
}
/*************************************************************************/
/*      display descriptor                                               */
/*************************************************************************/
void read_desc()
{
        unsigned int i_addrx;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int nread;
	int i,j,loopx,idx,len;

        printf("DESC(Hex):");
        scanf("%x",&i_addrx);

        reg = i_addrx; length=0x40;
	nread = get_desc(usb_handle, reg, length, buf);
    /*  printf("nread=%d\n",nread); */
        loopx=1;
        i=0;
        idx=0;
        while(loopx)
        {
            len = buf[i];
            printf("DESC %2d = ",idx);
            for(j=0;j<len;j++)
                printf("%2X ",buf[i+j]);
            printf("\n");
            idx++;
            i=i+len;
            if(i>=nread) loopx=0;
        }

}
/*************************************************************************/
/*      display desc. string                                             */
/*************************************************************************/
void read_string()
{
        unsigned int i_addrx;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int nread;
	int i;

        printf("STRING:");
        scanf("%x",&i_addrx);

        reg = i_addrx; length=0xff;
	nread = get_string(usb_handle, reg, length, buf);
        printf("STRING= ");
	for(i=0;i<nread;i++)
            printf("%2X ",buf[i]);
        printf("\n");

}
/*************************************************************************/
/*      display interrupt data                                           */
/*************************************************************************/
void read_interrupt()
{
	int i;

        retval = usb_bulk_read(usb_handle, 
			       ep_intr, 
			       (char*)intr_buf, 
			       8, 
			       5000);
        if(retval > 0)
        {
            printf("interrupt data:");
	    for(i=0;i<8;i++)
	        printf(" %2X",intr_buf[i]);
	    printf("\n");
        }
        else
        {
            printf("read interrupt timeout!!!\n");
        }

}
/*************************************************************************/
/*      display MAC register                                             */
/*************************************************************************/
void show_reg()
{
        unsigned int i_addrx;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
    //  int nread;

        printf("REG(Hex):");
        scanf("%x",&i_addrx);

        reg = i_addrx; length=1;
  	dm_rd_reg(usb_handle, reg, length, buf);
        printf("REG[%4x] = %2X\n",i_addrx,buf[0]);

}
/*************************************************************************/
/*      write   MAC register                                             */
/*************************************************************************/
void wr_reg()
{

        unsigned int  i_addrx;
        unsigned int  i_datax;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];

        printf("REG(Hex):");
        scanf("%x",&i_addrx);

        printf("DATA(Hex):");
        scanf("%x",&i_datax);

        reg = i_addrx; length=1;
        buf[0]=i_datax;
	dm_wr_reg(usb_handle, reg, length, buf);
/*
        reg = i_addrx; length=1;
  	dm_rd_reg(usb_handle, reg, length, buf);
        printf("REG[%4x] = %2X\n",i_addrx,buf[0]);
*/
}
/*************************************************************************/
/*      write 9051  register from SPI                                    */
/*************************************************************************/
unsigned int dm_wr_reg(struct usb_dev_handle* handle, unsigned char regx, int lengthx, unsigned char *datax)
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int i;

  for(i=0;i<lengthx;i++)
  {
    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    reg = ((regx+i) & 0xff) | 0x80;
    wr_gpio_byte(reg);

    reg = datax[i];
    wr_gpio_byte(reg);

    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);
  }
  return(1);

}
/*************************************************************************/
/*      read  9051  register from SPI                                    */
/*************************************************************************/
unsigned int dm_rd_reg(struct usb_dev_handle *handle, unsigned char regx, int lengthx, unsigned char *datax)
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        unsigned char so_byte;
        int i;

  for(i=0;i<lengthx;i++)
  {
    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    reg = (regx+i) & 0x7f;
    wr_gpio_byte(reg);

    so_byte = rd_gpio_byte();

    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    datax[i] = so_byte;
  }
    return(1);
}


//Stone add for 1588
/*************************************************************************/
/*      read  9051  register 1588 0x68 from SPI (index 0-7)              */
/*************************************************************************/
unsigned int dm_rd_reg_68h(struct usb_dev_handle *handle, unsigned char regx, int lengthx, unsigned char *datax)
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        unsigned char so_byte;
        int i;

  
    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    reg = (regx) & 0x7f;
    wr_gpio_byte(reg);

for(i=0;i<lengthx;i++)
  {
    so_byte = rd_gpio_byte();
    datax[i] = so_byte;
  }

    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    return(1);
}

//Stone add for 1588
/*************************************************************************/
/*      write 9051  register 1588 0x68 from SPI  (index 0-7)             */
/*************************************************************************/
unsigned int dm_wr_reg_68h(struct usb_dev_handle* handle, unsigned char regx, int lengthx, unsigned char *datax)
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int i;

  
    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);
    
     for(i=0;i<lengthx;i++)
    {

     reg = ((regx) & 0xff) | 0x80;
     wr_gpio_byte(reg);
    
   
     reg = datax[i];
     wr_gpio_byte(reg);
    } 
    
    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);
  
  return(1);

}

// JFW_1
/*********************************************************************/
/*      read  9051  memory   from SPI                                */
/*********************************************************************/
unsigned int dm_rd_spi_mems(struct usb_dev_handle* handle, 
                            unsigned int  regx, 
                            int lengthx, 
                            unsigned char *datax)
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        unsigned char so_byte;
        int i;

    reg = 0x70; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);

    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    reg = 0x72;
    wr_gpio_byte(reg);

    for(i=0;i<lengthx;i++)
    {
        so_byte = rd_gpio_byte();

        datax[i] = so_byte;
    }
    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    return(1);
}
/*********************************************************************/
/*      write 9051  memory   from SPI                                */
/*********************************************************************/
unsigned int dm_wr_spi_mems(struct usb_dev_handle* handle, 
                            unsigned int  regx, 
                            int lengthx, 
                            unsigned char *datax)
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int i;

//printf("DEBUG: wr_spi_mems\n");
    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    reg = 0xf8;
    wr_gpio_byte(reg);

    for(i=0;i<lengthx;i++)
    {

        reg = datax[i];
        wr_gpio_byte(reg);
    }

    reg = 0x35; length=1;
    buf[0]=0x0a;
    retval = dm_write(usb_handle, reg, length, buf);

    return(1);

}
/*************************************************************************/
/*      write byte GPIO to SPI                                           */
/* CSB 0 000    0                                                      */
/* CK  0 010 x8 0                                                      */
/* MO  1 ddd    1                                                      */
/*************************************************************************/
void wr_gpio_byte(unsigned char regx)
{

        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int i;
        int mo_loc;
        unsigned char mo_data;

        reg = 0x35; length=1;
        buf[0]=0x02;
	retval = dm_write(usb_handle, reg, length, buf);

	mo_loc = 0x80;
	for(i=0;i<8;i++)
	{
	    if((mo_loc & regx)!=0)
                mo_data=0x02;
	    else
                mo_data=0x00;

            reg = 0x35; length=1;
            buf[0]=mo_data;
	    retval = dm_write(usb_handle, reg, length, buf);

            reg = 0x35; length=1;
            buf[0]=mo_data | 0x04;
	    retval = dm_write(usb_handle, reg, length, buf);

            reg = 0x35; length=1;
            buf[0]=mo_data;
	    retval = dm_write(usb_handle, reg, length, buf);

            mo_loc = (mo_loc >> 1);
	}

        reg = 0x35; length=1;
        buf[0]=0x02;
	retval = dm_write(usb_handle, reg, length, buf);

}
/*************************************************************************/
/*      read  byte GPIO fm SPI                                           */
/* CSB 0 000    0                                                      */
/* CK  0 010 x8 0                                                      */
/* MO  0 000    1                                                      */
/* SO  - ddd    -                                                      */
/*************************************************************************/
unsigned char rd_gpio_byte()
{

        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int i;
        unsigned char so_data;

        reg = 0x35; length=1;
        buf[0]=0x00;
	retval = dm_write(usb_handle, reg, length, buf);

	so_data = 0x00;
	for(i=0;i<8;i++)
	{
	    so_data = (so_data << 1);
            reg = 0x35; length=1;
            buf[0]=0x04;
	    retval = dm_write(usb_handle, reg, length, buf);

	    dm_read(usb_handle, reg, length, buf);
            so_data = so_data | (buf[0] & 0x01);

            reg = 0x35; length=1;
            buf[0]=0x00;
	    retval = dm_write(usb_handle, reg, length, buf);
	}

        reg = 0x35; length=1;
        buf[0]=0x02;
	retval = dm_write(usb_handle, reg, length, buf);

	return(so_data);
}
/*********************************************************************/
/*      TX packets                                                   */
/*********************************************************************/
void tx_pkt()
{

unsigned long cntx;
unsigned int dly_cnt, dly_ctr;
unsigned int loop;
unsigned int m_tx_size;
unsigned int m_max_tx_size;
unsigned int m_size_gap;
unsigned char reg;
unsigned short length;
unsigned char buf[2];
//Stone add for 1588
unsigned char nanosecond[4];
unsigned char second[4];

        (void) signal (SIGINT,ctrl_c_tx_pkt); // JF_3

    rx_pkt_ctr=0;
    printf("fm. TX SIZE:");
    scanf("%d", &m_tx_size);
    tx_len = m_tx_size;
//  if(tx_len > MAX_TX_SIZE) tx_len=60; // JF_12
    printf("MAX TX SIZE:");
    scanf("%d", &m_max_tx_size);
    printf("SIZE GAP:");
    scanf("%d", &m_size_gap);
    printf("TX CNT:");
    scanf("%ld", &cntx);
    printf("DELAY CNT:");
    scanf("%d", &dly_cnt);

    reg = 0xff; length=1;
    buf[0] = 0x80;
    imr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x05; length=1;
    if(tx_chk_rx)
        buf[0] = 0x4b;
    else
        buf[0] = 0;
    rcr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0a; length=1;
    if(no_bprs)
        buf[0] = 0x20;
    else
        buf[0] = 0x30;
    fcr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x3d; length=1;
    buf[0] = 0x00;
    reg3d_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x02; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    tcr_val = buf[0];

    // clr ISR
    reg = 0x7E; length=1;
    buf[0] = 0xff;
    dm_wr_reg(usb_handle, reg, length, buf);

    // JF2_6
    reg = 0xf4; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    rx_rd_mem_adr = buf[0];
    reg = 0xf5; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    rx_rd_mem_adr = rx_rd_mem_adr + (buf[0]<<8);

    tx_cnt=0;
    can_tx = 1;
    dly_ctr = 0;
    tx_index = 0;

    loop=1;
    while(loop)
    {
        if(can_tx)
        {
            make_tx_buf();
#if 1            
            reg = 0x02; length=1;
            buf[0] = tcr_val | 0x01;
        //  dm_write(usb_handle, reg, length, buf);
            dm_wr_reg(usb_handle, reg, length, buf);
#endif
            tx_cnt++;
            
            //Stone add for 1588
            if (test_mode==TEST_1588_GET_TX_TIME){
            //5. Read TX timestamp, write register 0x62, bit 0 (0x01)  
            reg = 0x62; length=1;
            buf[0] = 0x01;
            dm_wr_reg(usb_handle, reg, length, buf);
            
            
#if 0       //Stone add for 1588, read 0x68, len = ?     
            //6. Read register 0x68 (index 0 ~ index 0x07)
            reg = 0x68; length=1;
            printf("====== pkt[%d]=============================\r\n", tx_cnt); 
            dm_rd_reg(usb_handle, reg, length, buf); 
            printf("nanosecond (00:07) = %d, \n", buf[0]);
    
            dm_rd_reg(usb_handle, reg, length, buf); 
            printf("nanosecond (08:15) = %d, \n", buf[0]);
    
            dm_rd_reg(usb_handle, reg, length, buf); 
            printf("nanosecond (16:23) = %d, \n", buf[0]);
    
            dm_rd_reg(usb_handle, reg, length, buf);  
            printf("nanosecond (24:31) = %d \r\n", buf[0]);
 
            dm_rd_reg(usb_handle, reg, length, buf);  
            printf("second (00:07) = %d, \n", buf[0]);
    
            dm_rd_reg(usb_handle, reg, length, buf); 
            printf("second (08:15) = %d, \n", buf[0]);
    
            dm_rd_reg(usb_handle, reg, length, buf); 
            printf("second (16:23) = %d, \n", buf[0]);
    
            dm_rd_reg(usb_handle, reg, length, buf); 
            printf("second (24:31) = %d \r\n", buf[0]);
            printf("\r\n");
#endif       
            //Stone add for 1588, read 0x68, len = ?
            read_8 = 1;
            test_1588_reg_68_rd();
                  
            }
             
            if(show_rx_msg)
                printf("TX packet:%10ld, size:%4d\n", tx_cnt,tx_len);
            tx_len = tx_len + m_size_gap;
            if(tx_len > m_max_tx_size) tx_len=m_tx_size; // JF_12
            if(cntx!=0)
            {
                if(tx_cnt >= cntx) loop=0;
            }
            dly_ctr=dly_cnt;
            can_tx = 0;
        }

#if 0
       //Stone add for 1588 crypt 
       if (test_mode==TEST_1588_CRYPT){
       tx_chk_rx = 1;
       show_rx_data = 1;
       show_rx_msg = 1;
       } 
#endif


        if(tx_chk_rx)
            chk_rx();

        if(dly_ctr > 0)
        {
            usleep(100);
            dly_ctr--;
        }
        else
        { // HOST_5
            if(tx_chk_isr)
            {
                reg = 0x7E; length=1;
                dm_rd_reg(usb_handle, reg, length, buf);
                if((buf[0]&0x02)==0x02)
                {
                    // clr ISR
                    reg = 0x7E; length=1;
                    buf[0] = 0xff;
                    dm_wr_reg(usb_handle, reg, length, buf);
                    if(tx_index)
                        reg = 0x03;
                    else
                        reg = 0x04;
                    length=1;
                    dm_rd_reg(usb_handle, reg, length, buf);
                    printf("   TSR = %2X",buf[0]);
                    if((buf[0]&0x08)==0x08)
                        printf("   Collision ");
                    if((buf[0]&0x04)==0x04)
                        printf("   excessive collision ");
                    // if((buf[0]&0x0C)!=0x00)
                        printf("\n");
                    can_tx = 1;
                }
                else
                    printf("   TX delayed\n");
            }
            else
            can_tx = 1;
        }
    } // loop

    printf("end of TX packet %ld size:%d\n",tx_cnt,tx_len);
    printf("rx_ctr=%ld, crc=%d, overflow=%d\n",
           rx_pkt_ctr, rx_crc_ctr, rx_over_ctr);
}
/*********************************************************************/
/*      make TX buffer                                               */
/*********************************************************************/
void make_tx_buf()
{
int i;
int buf_offset;
unsigned char buf[2];
unsigned char tx_buf[2048];
unsigned char reg;
unsigned short length;

//Stone add for 1588
int kk,kk_line,kk_remain;

    if(tx_len_9000_mode)
        buf_offset=0;
    else
    {
        buf_offset=2;
        tx_buf[0] = tx_len & 0xff;
        tx_buf[1] = (tx_len>>8) & 0xff;
    }

        if(tx_index)
        {
            for(i=0;i<tx_len;i++)
//              tx_buf[i+buf_offset] = (tx_len-i)&0xff;
                tx_buf[i+buf_offset] = (i)&0xff; // HOST_T4
            tx_index=0;
        }
        else
        {
            for(i=0;i<tx_len;i++)
                tx_buf[i+buf_offset] = i&0xff;
            tx_index=1;
        }
/* HOST_4 mark
// read register before wr memory
reg = 0xfa; length=1;
*/
//printf("DEBUG: make_tx_buf\n");


//Stone add for 1588 crypt tx buf
     
            
     if (test_mode==TEST_1588_CRYPT){
         printf("%x %x %x %x %x %x %x %x %x %x \n", tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[4], tx_buf[5], tx_buf[6], tx_buf[7], tx_buf[8], tx_buf[9]);
         printf("======= Crypt TX data =========\r\n");
#if 1         
         for (i=0; i<tx_len; i++)
         {
          tx_buf[i]=tx_buf[i]^crypt_key; 
          //if (i/16 != 0)
          //printf(" %x", tx_buf[i]);
          }        
#endif          
         kk_line = (tx_len)/16;
            kk_remain =(tx_len) - (kk_line*16);
            for(kk=0;kk<kk_line;kk++)
            {
                printf("%4d: ",kk*16);
                for(i=0;i<16;i++)
                    //printf("%02X ",rx_buf[i+kk*16]);
                    printf("%02X ",tx_buf[i+kk*16]);
                printf("\n");
            }
            if(kk_remain>0)
            {
                printf("%4d: ",kk*16);
                for(i=0;i<kk_remain;i++)
                    //printf("%02X ",rx_buf[i+kk*16]);
                    printf("%02X ",tx_buf[i+kk*16]);
                printf("\n");
            } 
          
            
         printf("======= Crypt TX data end=========\r\n");
         }


    reg = 0x78;
  if(wr_txbuf_en) // JFD_2
    dm_wr_spi_mems(usb_handle, reg, tx_len, tx_buf);
#if 0  
  else
  {
    //  reg = 0xfa; length=3;
        reg = 0xfa; length=2;
        buf[0] = 0;
    //  buf[1] = 0xfb;
    //  buf[2] = 0;
        buf[1] = 0;
        dm_wr_reg(usb_handle, reg, length, buf);
  }
#endif

    if(tx_len_9000_mode)
    {
        reg = 0xfc; length=2;
        buf[0] = tx_len & 0xff;
        buf[1] = (tx_len>>8) & 0xff;
        dm_wr_reg(usb_handle, reg, length, buf);
#if 0        
        reg = 0x02; length=1;
        buf[0] = tcr_val | 0x01;
        //  dm_write(usb_handle, reg, length, buf);
        dm_wr_reg(usb_handle, reg, length, buf);
        
        //reg = 0xfd; length=1;
        //buf[0] = (tx_len>>8) & 0xff;
        //dm_wr_reg(usb_handle, reg, length, buf);
        
        //reg = 0xfc; length=2;
        //buf[0] = tx_len & 0xff;
        //buf[1] = (tx_len>>8) & 0xff;
        //dm_wr_reg(usb_handle, reg, length, buf);
        
        reg = 0x02; length=1;
        buf[0] = tcr_val | 0x01;
        //  dm_write(usb_handle, reg, length, buf);
        dm_wr_reg(usb_handle, reg, length, buf);
#endif        
        
    }
}
/*********************************************************************/
/*      RX packets                                                   */
/*********************************************************************/
void rx_pkt()
{
        unsigned int loop;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[2];


        (void) signal (SIGINT,ctrl_c_rx_pkt); // JF_3

    reg = 0xff; length=1;
    buf[0] = 0x80;
    imr_val = buf[0];
//  dm_write(usb_handle, reg, length, buf);
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x05; length=1;
    buf[0] = 0x0f;
    rcr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0a; length=1;
    buf[0] = 0x20;
    fcr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x3d; length=1;
    buf[0] = 0x00;
    reg3d_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    // JF2_6
    reg = 0xf4; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    rx_rd_mem_adr = buf[0];
    reg = 0xf5; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    rx_rd_mem_adr = rx_rd_mem_adr + (buf[0]<<8);

    loop=1;
    while(loop)
    {
          chk_rx();
    } /* loop */
}
/*********************************************************************/
/*      check RX                                                     */
/*********************************************************************/
void chk_rx()
{
int loop;
unsigned char buf[2];
unsigned char reg;
unsigned short length;

    loop=1;
    if(loop)
    {
        reg = 0x70;
        length=1;
        dm_rd_reg(usb_handle, reg, length, buf);
        reg = 0x70;
        length=1;
        dm_rd_reg(usb_handle, reg, length, buf);
        //printf("chk_rx() ready bit %x \n", buf[0]); //Stone add
        if((buf[0]&0x03)==0x01)
            show_rx();
        else
            loop=0;
    }
}
/*********************************************************************/
/*      parse RX                                                     */
/*********************************************************************/
void show_rx()
{
unsigned char buf[8];
unsigned char reg;
unsigned int  length;
unsigned char rx_flag;
unsigned char rx_status;
unsigned int rx_size;
unsigned int echo_size;
int i;
int kk,kk_line,kk_remain;
unsigned char exp_rxd;
int pause_pkt;

        rx_pkt_ctr++;

        reg = 0x72;
        length=4;
        dm_rd_spi_mems(usb_handle, reg, length, buf);
        rx_flag = buf[0];
        rx_status = buf[1];
        rx_size = (buf[3]<<8) + buf[2];

        if(rx_size > 8000)
        {
            printf("RX size error!!! [%d] \n", rx_size);
            exit(0);
        }

        if(rx_size > 1522)
        {
            if(exit_rx_err) // HOST_3
            {
                printf("ERROR: RX size > 1522\n");
                exit(0);
            }
        }

        if(((rx_status&0x02)==0x02)  && ((rx_status&0x01)!=0x01))
        {
            rx_crc_ctr++;
            if(exit_rx_err) // HOST_3
            {
                printf("ERROR: CRC\n");
                exit(0);
            }
        }
        
        //Stone add for 1588 rx timestamp
        //RX timestamp len and format (8-byte or 4-byte), RX status bit 5 (ready), 2 (len)
        if (test_mode == TEST_1588_GET_RX_TIME)
        {
         if ((rx_status&0x20)==0x20){ //RX timestamp available~
          if ((rx_status&0x04)==0x04){ //RX timestamp 8-byte
            reg = 0x72;
            length=8;
            dm_rd_spi_mems(usb_handle, reg, length, buf); 
            
            printf("====== rx pkt[%d] ====rx_stauts %x =========================\r\n", rx_pkt_ctr, rx_status); 
            printf("nanosecond (00:07) = %d, \n", buf[7]);
    
            printf("nanosecond (08:15) = %d, \n", buf[6]);
    
            printf("nanosecond (16:23) = %d, \n", buf[5]);
    
            printf("nanosecond (24:31) = %d \r\n", buf[4]);
 
            printf("second (00:07) = %d, \n", buf[3]);
    
            printf("second (08:15) = %d, \n", buf[2]);
    
            printf("second (16:23) = %d, \n", buf[1]);
    
            printf("second (24:31) = %d \r\n", buf[0]);
            printf("\r\n");
           
           } //end of RX timestamp 8-byte       
          } //end of RX timestamp available~  
         }else printf("RX timestamp capture enable, but rx packet no rx timestamp!!! \r\n");

        if(show_rx_msg)
        {
            printf(" RX packet %10ld: %2X %2X size:%d\n",
                       rx_pkt_ctr,rx_flag,rx_status,rx_size);
        }
        reg = 0x72;
        length=rx_size;
        dm_rd_spi_mems(usb_handle, reg, length, rx_buf);

        if(show_rx_data)
        {
            kk_line = (rx_size)/16;
            kk_remain =(rx_size) - (kk_line*16);
            for(kk=0;kk<kk_line;kk++)
            {
                printf("%4d: ",kk*16);
                for(i=0;i<16;i++)
                    printf("%02X ",rx_buf[i+kk*16]);
                printf("\n");
            }
            if(kk_remain>0)
            {
                printf("%4d: ",kk*16);
                for(i=0;i<kk_remain;i++)
                    printf("%02X ",rx_buf[i+kk*16]);
                printf("\n");
            }
        }

        if(echo_rx) // HOST_3
        {
/* HOST_4 mark
//
reg = 0xfa; length=1;
dm_rd_reg(usb_handle, reg, length, buf);
*/
            reg = 0x78;
            echo_size = rx_size;
            dm_wr_spi_mems(usb_handle, reg, echo_size, rx_buf);

            reg = 0xfc; length=1;
            buf[0] = echo_size & 0xff;
            dm_wr_reg(usb_handle, reg, length, buf);

            reg = 0xfd; length=1;
            buf[0] = (echo_size>>8) & 0xff;
            dm_wr_reg(usb_handle, reg, length, buf);
            reg = 0x02; length=1;
            buf[0] = 0x1f; // tx cmd without pad/crc
            dm_wr_reg(usb_handle, reg, length, buf);

            if(show_rx_msg)
                printf(" ECHO RX packet size:%d\n", echo_size);
        }

        if(chk_rxd) // HOST_8
        {
            // HOST_8B
            pause_pkt =((rx_buf[0]==0x01) &&
                        (rx_buf[1]==0x80) &&
                        (rx_buf[2]==0xc2) &&
                        (rx_buf[3]==0x00) &&
                        (rx_buf[4]==0x00) &&
                        (rx_buf[5]==0x01));
          if(pause_pkt)
          {
            rx_pause_pkt_ctr++;
            if(show_rx_msg)
                printf(" RX pause packet %ld\n",rx_pause_pkt_ctr);
          }
          else
          {
            for(i=0;i<rx_size-4;i++)
            {
                exp_rxd = i&0xff;
                if(rx_buf[i]!=exp_rxd)
                {
                    printf("RXD error at %d\n",i);
                    reg = 0x05; length=1;
                    buf[0] = 0;
                    dm_wr_reg(usb_handle, reg, length, buf);
                    exit(0);
                }
            }
            if(show_rx_msg)
                printf(" check RXD OK\n");
          }
        }
}

/* 2023-12-20 Stone add for WOL Sample frame & mask */
/*************************************************************************/
/*      test WOL sample event                                            */
/*************************************************************************/

void sample_frame_mask()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[64], sample_data[256], sample_mask[256];
 int nread, i;
 unsigned int pat_type, sample_len, user_pat, sample_data_base, sample_mask_base;
 
 //Sample frame data prepare
 printf("Sample frame length (2-255):");
 scanf("%d",&sample_len);
 if (sample_len <2)
   sample_len = 0x02;
 if (sample_len > 256)
   sample_len = 256;  
 
 printf("DATA pattern(0:0x55, 1:0xAA, 2:INC, 3:DEC, 4:User input):");
 scanf("%d",&pat_type);
 
 switch(pat_type)
 {
  case 0:
         for (i=0; i<sample_len; i++)
          sample_data[i]= 0x55;
         break; 

  case 1:
         for (i=0; i<sample_len; i++)
          sample_data[i]= 0xAA;
         break; 
         
   case 2:
         for (i=0; i<sample_len; i++)
          sample_data[i]= i & 0xff;
         break; 
         
   case 3:
         for (i=0; i<sample_len; i++)
          sample_data[i]= 0xff - (i&0xff);
         break;      

  case 4:
         printf("User input data (hex)\r\n");
         scanf("%x",&user_pat);
         for (i=0; i<sample_len; i++)
          sample_data[i]= user_pat;
         break;      
 default:
         for (i=0; i<sample_len; i++)
          sample_data[i]= 0x55;
         break;            
 }
 
 //Sample frame mask prepare
 for (i=0; i<sample_len; i++)
  sample_mask[i]= 0x01;
 
 sample_mask[0]=0xfd; //Stone add for other 
 sample_mask[sample_len-1]=0x03; 
 
 //Write TX address 0x0000 memory (sample frame 0 ) & TX address 0x2000 memory (sample frame 0 mask)
 
 sample_data_base = 0x0000; 
 sample_mask_base = 0x2000;
 
  for (i=0; i<sample_len; i++)
  {
   //Write sample frame data
   reg = 0x7A; length=1;
   buf[0] = sample_data_base & 0xff;
   dm_wr_reg(usb_handle, reg, length, buf);

   reg = 0x7B; length=1;
   buf[0] = (sample_data_base >> 8) & 0xff;
   dm_wr_reg(usb_handle, reg, length, buf);
  
   reg = 0x78; length=1;
   buf[0] = sample_data[i];
   dm_wr_reg(usb_handle, reg, length, buf);
   
   //Write sample frame mask 
   reg = 0x7A; length=1;
   buf[0] = sample_mask_base & 0xff;
   dm_wr_reg(usb_handle, reg, length, buf);

   reg = 0x7B; length=1;
   buf[0] = (sample_mask_base >> 8) & 0xff;
   dm_wr_reg(usb_handle, reg, length, buf);
  
   reg = 0x78; length=1;
   buf[0] = sample_mask[i];
   dm_wr_reg(usb_handle, reg, length, buf);
   
   if (i<10)
    printf("- sample_data_base = 0x%x, sample_mask_base = 0x%x, sample_data[%d] = 0x%x, sample_mask[%d] = 0x%x \r\n", sample_data_base, sample_mask_base, i, sample_data[i], i, sample_mask[i]);
   
   if (i == (sample_len-1))
    printf("- sample_data_base = 0x%x, sample_mask_base = 0x%x, sample_data[%d] = 0x%x, sample_mask[%d] = 0x%x \r\n", sample_data_base, sample_mask_base, i, sample_data[i], i, sample_mask[i]);
   
   
   sample_data_base = sample_data_base + 4;			
   sample_mask_base = sample_mask_base + 1;

  } 

}

/* JF_9, 2023-12-20 Stone add for WOL Sample frame test */
/*************************************************************************/
/*      test WOL sample event                                            */
/*************************************************************************/
void test_WOL_sample()
{
unsigned char reg;
unsigned short length;
unsigned char buf[64];
//unsigned char mmm[16];
//unsigned char ddd[16];
int nread;
unsigned int fred;

#if 0
    //Write Memorry enable 
    reg = 0xf4; length=1;
    buf[0]=0x01;
    nread = dm_write(usb_handle, reg, length, buf);
  //Stone add for check DM9620 E5 or E6
    
  if (mode_9620 == 3)
   {
    if (mode_file == 1) //use file to test sample frame
    {
     fred = read_file(); //Read *.txt
//     printf("fred = %x \n",fred); 
     if (fred)  
      write_sample(); //Write data to Memory
     }
    else
     sample_E6();
    }//End of (mode_9620 == 3)
  else
   {
    if (mode_file == 1) //use file to test sample frame
    {
     fred = read_file();
     if (fred) 
      write_sample(); //Write data to Memory
     }
    else
     sample_E5();
   } //End of (mode_9620 !=3 )
  
    //Enable wakeup function
    reg = 0x00; length=1;
    buf[0]=0x40;
    nread = dm_write(usb_handle, reg, length, buf);

    //Discard log, CRC error, packet; Pass all multicast, Runt packet; 
    //Enable Promiscuous Mode; RX enable;  
    reg = 0x05; length=1;
    buf[0]=0x3f;
    nread = dm_write(usb_handle, reg, length, buf);

    //Sample Frame match event enable 
    reg = 0x0f; length=1;
    buf[0]=0x10;
    nread = dm_write(usb_handle, reg, length, buf);
    
    //Write Memorry disable 
    reg = 0xf4; length=1;
    buf[0]=0x00;
    nread = dm_write(usb_handle, reg, length, buf);
    
#endif
   //1. Start register init 
   reg = 0x7f; length=1;
   buf[0] = 0x00;
   dm_wr_reg(usb_handle, reg, length, buf);

   reg = 0x00; length=1;
   buf[0] = 0x00;
   dm_wr_reg(usb_handle, reg, length, buf);

   //2. Sample frame data pattern input / Sample frame mask init
   sample_frame_mask();

   //3. WOL register init
   reg = 0x05; length=1;
   buf[0] = 0x0f;
   dm_wr_reg(usb_handle, reg, length, buf);

   reg = 0x00; length=1;
   buf[0] = 0x48;
   dm_wr_reg(usb_handle, reg, length, buf);
   
   reg = 0x0f; length=1;
   buf[0] = 0x18;
   dm_wr_reg(usb_handle, reg, length, buf);

   reg = 0x7f; length=1;
   buf[0] = 0x00;
   dm_wr_reg(usb_handle, reg, length, buf);
   
   //Stone add for read back register data
   reg = 0x05;
   length=1;
   dm_rd_reg(usb_handle, reg, length, buf);
   printf("register 0x05 = 0x%x \r\n", buf[0]);

   reg = 0x00;
   length=1;
   dm_rd_reg(usb_handle, reg, length, buf);
   printf("register 0x00 = 0x%x \r\n", buf[0]);
   
   reg = 0x0F;
   length=1;
   dm_rd_reg(usb_handle, reg, length, buf);
   printf("register 0x0F = 0x%x \r\n", buf[0]);
   
   reg = 0x7F;
   length=1;
   dm_rd_reg(usb_handle, reg, length, buf);
   printf("register 0x7F = 0x%x \r\n", buf[0]);

   reg = 0x1F;
   length=1;
   dm_rd_reg(usb_handle, reg, length, buf);
   printf("register 0x1F = 0x%x \r\n", buf[0]);

   printf("Please check the register 0x88 bit 0-7 the result of WOL sample frame status!!!!\n");

}



/*************************************************************************/
/*      display RX counter before exit                                   */
/*************************************************************************/
void exit_xx(exit_codex)
int exit_codex;
{
    printf(" rx_ctr=%ld\n", rx_pkt_ctr);
    printf("rst_ctr=%d, hd9620=%d, size=%d, 1st_size=%d, bi_size=%d\n",
           rx_rst_ctr,
           err1_header9620,
           err2_rx_size,
           err3_rx_1size,
           err4_bi_size
           );
    exit(exit_codex);
}
/*************************************************************************/
/*      display all EEPROM registers                                     */
/*************************************************************************/
void show_eep()
{
unsigned char i;

    for(i=0;i<32;i=i+2)
    {
        wdata =rd_eep(eep_page*32 +i);
        wdata2=rd_eep(eep_page*32 +i+1);
        printf("EEP[%x]=%x \t EEP[%x]=%x\n",
                eep_page*32 +i,wdata,eep_page*32 +i+1,wdata2);
    }
}
/*************************************************************************/
/* write EEPROM register data                                           */
/*************************************************************************/
void test_wr_eep()
{
unsigned char reg_no;
unsigned int  wdata;
unsigned int  reg_nox;
/*
int i;
*/

    printf("REG:");
    scanf("%x",&reg_nox);
    printf("DATA:");
    scanf("%x",&wdata);

    reg_no = reg_nox;
    wr_eep(reg_no,wdata);
/*  delayx(2); */
    usleep(10000);
/*
    printf("wait write...");
    for(i=0;i<50;i++) printf(".");
    printf("\n");
*/
    wdata2=rd_eep(reg_no);

    printf("RD EEP[%x]=%x\n",reg_no,wdata2);
}
// JFD_1
/*************************************************************************/
/*      display all PHY    registers                                     */
/*************************************************************************/
void show_phy()
{
    unsigned int wphy_addr; 
    unsigned int wphy_reg; 
    unsigned int wdata;
    
    unsigned char reg;
    unsigned short length;
    unsigned char buf[SZBUF];
    
    int i;

#if 0 //Stone add for 1588 FPGA
    printf("PHY MMD (0,1,3,31):");
    scanf("%d",&wphy_page);
    if(rd_phy_one)
    {
        if((wphy_page!=1) &&
           (wphy_page!=3) &&
           (wphy_page!=31))
            wphy_page=0;
        printf("PHY Addr:");
        scanf("%x",&wphy_addr);
        wdata =rd_phy(wphy_page,wphy_addr);
        printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);
    }
    else
    {
        if(wphy_page==0)  show_phy_p0();
        if(wphy_page==1)  show_phy_p1();
        if(wphy_page==3)  show_phy_p3();
        if(wphy_page==31) show_phy_p31();
    }
#endif

   //Default PHY address 0x01 
   wphy_addr = 0x01;
   
   if(rd_phy_one)
   {
    printf("PHY Addr:");
    scanf("%x",&wphy_addr);
    
    //Write register 0x33, bit 0-4, bit 7 => 1 (0x80) external PHY address enable
    reg = 0x33; length=1;
    buf[0] = wphy_addr | 0x80;
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    //printf("PHY register:");
    //scanf("%x",&wphy_reg);
    
    
    for (i=0; i<0x20; i++){ 
      wdata = rd_phy_fm_nic(i);
   
      printf("PHY[%02X][%04X]=%04X \n", wphy_addr, i, wdata);
    }
    //Write register 0x33, bit 0-4, bit 7 => 1 (0x80) external PHY address enable
    reg = 0x33; length=1;
    buf[0] = 0x00;  //Clear external PHY address enable
    dm_wr_reg(usb_handle, reg, length, buf); 
   }else{
   
    //PHY address 0-3
    for (i=0; i<0x20; i++){ 
    wdata = rd_phy_fm_nic(i);
    printf("PHY[%02X][%04X]=%04X \n", wphy_addr, i, wdata);
    }
   } 
}
// JFD_1
/*************************************************************************/
/*      display  PHY page 0  registers                                   */
/*************************************************************************/
void show_phy_p0()
{
    unsigned int wphy_addr; 
    unsigned int wphy_page; 
    unsigned int wdata;

    wphy_page = 0; wphy_addr = 0x0000;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 0; wphy_addr = 0x0001;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 0; wphy_addr = 0x0002;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 0; wphy_addr = 0x0003;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 0; wphy_addr = 0x000d;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 0; wphy_addr = 0x000e;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 0; wphy_addr = 0x0012;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

}
// JFD_1
/*************************************************************************/
/*      display  PHY page 1  registers                                   */
/*************************************************************************/
void show_phy_p1()
{
    unsigned int wphy_addr; 
    unsigned int wphy_page; 
    unsigned int wdata;

    wphy_page = 1; wphy_addr = 0x0012;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 1; wphy_addr = 0x0834;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 1; wphy_addr = 0x08f9;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 1; wphy_addr = 0x08fa;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 1; wphy_addr = 0x08fb;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

}
// JFD_1
/*************************************************************************/
/*      display  PHY page 3  registers                                   */
/*************************************************************************/
void show_phy_p3()
{
    unsigned int wphy_addr; 
    unsigned int wphy_page; 
    unsigned int wdata;

    wphy_page = 3; wphy_addr = 0x08f3;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 3; wphy_addr = 0x08f4;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 3; wphy_addr = 0x08f5;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 3; wphy_addr = 0x08f6;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

}
// JFD_1
/*************************************************************************/
/*      display  PHY page 31 registers                                   */
/*************************************************************************/
void show_phy_p31()
{
    unsigned int wphy_addr; 
    unsigned int wphy_page; 
    unsigned int wdata;

    wphy_page = 31; wphy_addr = 0x0010;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x0018;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x0019;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x001a;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x001c;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x001d;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x0020;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);
    
    wphy_page = 31; wphy_addr = 0x0022;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);
    
    wphy_page = 31; wphy_addr = 0x0023;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x0024;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x0025;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x0026;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x0027;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0x00cb;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0xca00;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0xca01;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0xca02;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0xca03;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0xca04;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

    wphy_page = 31; wphy_addr = 0xca05;
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);

}
// JFD_1
/*************************************************************************/
/* write PHY    register data                                           */
/*************************************************************************/
void test_wr_phy()
{
 unsigned int  wphy_reg;
 unsigned int  wphy_addr;
 unsigned int  wphy_data;
 unsigned int  wdata;

 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF];
 
#if 0
    printf("PHY MMD (0,1,3,31):");
    scanf("%d",&wphy_page);
    printf("PHY Addr:");
    scanf("%x",&wphy_addr);
    printf("PHY data:");
    scanf("%x",&wphy_data);

    wr_phy(wphy_page, wphy_addr, wphy_data);
    wdata =rd_phy(wphy_page,wphy_addr);
    printf("PHY[%02X][%04X]=%04X\n", wphy_page, wphy_addr,wdata);
#endif

    printf("PHY Addr:");
    scanf("%x",&wphy_addr);
    
    printf("PHY register:");
    scanf("%x",&wphy_reg);
    
    printf("PHY data:");
    scanf("%x",&wphy_data);
    
    if (wphy_addr != 1) //Write register 0x33 to right PHY address
    {
     //Write register 0x33, bit 0-4, bit 7 => 1 (0x80) external PHY address enable
     reg = 0x33; length=1;
     buf[0] = wphy_addr | 0x80;
     dm_wr_reg(usb_handle, reg, length, buf); 
    }
     
    wr_phy_fm_nic(wphy_reg, wphy_data); 
    
    wdata = rd_phy_fm_nic(wphy_reg);
    printf("PHY[%02X][%04X]=%04X \n", wphy_addr, wphy_reg, wdata);
    
    if (wphy_addr != 1) //Write register 0x33 to right PHY address
    {
     //Write register 0x33, bit 0-4, bit 7 => 1 (0x80) external PHY address enable
     reg = 0x33; length=1;
     buf[0] = 0x00;
     dm_wr_reg(usb_handle, reg, length, buf); 
    }
    
}
/*************************************************************************/
/*      write EEPROM                                                     */
/*************************************************************************/
void wr_eep(reg_no,wdata)
unsigned char reg_no;
unsigned int  wdata;
{
unsigned char valx;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];


    valx=0x00 | reg_no;
    reg = 0x0c; length=1;
    buf[0]=valx;
    dm_wr_reg(usb_handle, reg, length, buf);

    valx=wdata&0xff;
    reg = 0x0d; length=1;
    buf[0]=valx;
    dm_wr_reg(usb_handle, reg, length, buf);

    valx=(wdata>>8)&0xff;
    reg = 0x0e; length=1;
    buf[0]=valx;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x10;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x12;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x10;
    dm_wr_reg(usb_handle, reg, length, buf);

    wait_phy_end();

}
/*************************************************************************/
/*      read  EEPROM                                                     */
/*************************************************************************/
int rd_eep(num)
unsigned char num;
{
unsigned char valx;
unsigned int  datax;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];

    valx=0x00 | num;
    reg = 0x0c; length=1;
    buf[0]=valx;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x00;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x04;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x00;
    dm_wr_reg(usb_handle, reg, length, buf);

    wait_phy_end();

    reg = 0x0d; length=2;
    dm_rd_reg(usb_handle, reg, length, buf);
    datax=(buf[1]<<8) + buf[0];

    return(datax);

}
// JFD_1
/*************************************************************************/
/*      write PHY                                                        */
/*************************************************************************/
void wr_phy(page, addr, wdata)
unsigned int  page;
unsigned int  addr;
unsigned int  wdata;
{
//unsigned int  reg;
//unsigned int  addrx;
//unsigned short length;
//unsigned char buf[SZBUF];
unsigned char reg_num;
unsigned int  dpage;

    if(page==0)
    {
        reg_num = addr & 0xff;
        wr_phy_fm_nic(reg_num,wdata);
    }
    else
    {
	//------------------------------------
	// write ADDRESS function PHY reg_0x0d
	//------------------------------------
        reg_num = 0x0d;
        dpage = 0x0000 | (0xff & page);
        wr_phy_fm_nic(reg_num,dpage);

	//------------------------------------
	// write address value    PHY reg_0x0e
	//------------------------------------
        reg_num = 0x0e;
        wr_phy_fm_nic(reg_num,addr);

	//------------------------------------
	// write DATA    function PHY reg_0x0d
	//------------------------------------
        reg_num = 0x0d;
        dpage = 0x4000 | (0xff & page);
        wr_phy_fm_nic(reg_num,dpage);

	//------------------------------------
	// write DATA    value    PHY reg_0x0e
	//------------------------------------
        reg_num = 0x0e;
        wr_phy_fm_nic(reg_num,wdata);
    }
}
/*************************************************************************/
/*      write PHY                                                        */
/*************************************************************************/
void wr_phy_fm_nic(reg_no,wdata)
unsigned char reg_no;
unsigned int  wdata;
{
unsigned char valx;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];

    valx=0x40 | reg_no;
    reg = 0x0c; length=1;
    buf[0]=valx;
    dm_wr_reg(usb_handle, reg, length, buf);


    valx=(wdata)&0xff;
    reg = 0x0d; length=1;
    buf[0]=valx;
    dm_wr_reg(usb_handle, reg, length, buf);

    valx=(wdata>>8)&0xff;
    reg = 0x0e; length=1;
    buf[0]=valx;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x08;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x0a;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x08;
    dm_wr_reg(usb_handle, reg, length, buf);

    wait_phy_end();

}
// JFD_1
/*************************************************************************/
/*      read  PHY                                                        */
/*************************************************************************/
int rd_phy(page,addr)
unsigned int  page;
unsigned int  addr;
{
//unsigned int  addrx;
unsigned int  datax;
//unsigned int  reg;
//unsigned short length;
//unsigned char buf[SZBUF];
unsigned char num;

    if(page==0)
    {
        num = addr & 0xff;
        datax = rd_phy_fm_nic(num);
    }
    else
    {
        wr_phy(0x0000, 0x000d, page);
        wr_phy(0x0000, 0x000e, addr);

        wr_phy(0x0000, 0x000d, (page | 0x4000)); // DATA command
	// command to read PHY 0x0e
        num = 0x0e;
        datax = rd_phy_fm_nic(num);
    }


    return(datax);

}

/*************************************************************************/
/*      read  PHY                                                        */
/*************************************************************************/
int rd_phy_fm_nic(num)
unsigned char num;
{
unsigned char valx;
unsigned int  datax;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];

    valx=0x40 | num;
    reg = 0x0c; length=1;
    buf[0]=valx;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x08;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x0c;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0b; length=1;
    buf[0]=0x08;
    dm_wr_reg(usb_handle, reg, length, buf);

    wait_phy_end();

    reg = 0x0d; length=2;
    dm_rd_reg(usb_handle, reg, length, buf);
    datax=(buf[1]<<8) + buf[0];

    return(datax);

}
/*************************************************************************/
/*      wait end of PHY operation                                        */
/*************************************************************************/
void wait_phy_end()
{
int loop;
unsigned long tst_ctr;
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];

    tst_ctr=0;
    loop=1;
    while(loop)
    {
        reg = 0x0b; length=1;
        dm_rd_reg(usb_handle, reg, length, buf);
        if(buf[0]&0x01)
        {
            /* printf("EEP status=%x\n",buf[0]); */
            tst_ctr++;
            if(tst_ctr > 0x7fff)
            {
                printf("EEP: TIMEOUT!!!\n\n");
                exit(0);
            }
        }
        else
        {
            /* printf("EEP status=%x\n",buf[0]); */
            loop=0;
        }
    }
}


/*************************************************************************/
/*      read e-fuse data (MAC Address)                                   */
/*************************************************************************/
void show_e_fuse()
{
 int i;
 unsigned int  datax;
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF]; 
 
  
  reg = 0x58; length=1;
  buf[0]=0x80;
  dm_wr_reg(usb_handle, reg, length, buf);
  
  for (i=0; i<24; i++){
   datax = rd_eep(i);
   printf("e-fuse[%x] = %x \r\n", i, datax);
   }
   
#if 0
  reg = 0x58; length=1;
  buf[0]=0x00;
  dm_wr_reg(usb_handle, reg, length, buf);
#endif

}

/*************************************************************************/
/*      write e-fuse data (MAC Address)                                   */
/*************************************************************************/
void test_wr_e_fuse()
{
 int i;
 unsigned int  datax;
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF];
 unsigned char reg_num;
 unsigned int addr, wdata;
  
  printf("e-fuse addr:");
    scanf("%x",&addr);
    printf("e-fuse data:");
    scanf("%x",&wdata);
  
  //reg = 0x1f; length=1;
  //buf[0]=0x01;
  //dm_wr_reg(usb_handle, reg, length, buf);
  
  reg = 0x58; length=1;
  buf[0]=0x88;
  dm_wr_reg(usb_handle, reg, length, buf);
  
  reg_num = addr & 0xff;
  wr_eep(reg_num,wdata);
  
  reg = 0x58; length=1;
  buf[0]=0x80;
  dm_wr_reg(usb_handle, reg, length, buf);
  
  datax = rd_eep(reg_num);
  printf("e-fuse[%x] = %x \r\n", addr, datax);
  
#if 0   
  reg = 0x58; length=1;
  buf[0]=0x00;
  dm_wr_reg(usb_handle, reg, length, buf);
#endif
  
  //reg = 0x1f; length=1;
  //buf[0]=0x00;
  //dm_wr_reg(usb_handle, reg, length, buf);
  
} 

/*************************************************************************/
/*      Setup 1588 PTP Clock time                                        */
/*************************************************************************/
void test_1588_set_time()
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int i;
        unsigned char nanosecond[4];
        unsigned char second[4];

    //reg = 0x1f; length=1;
    //buf[0] = 0x00;
    //dm_wr_reg(usb_handle, reg, length, buf); 
#if 1 
   //4. Setup register 0x61 to 0x01, PTP Clock read
    reg = 0x61; length=1;
    buf[0] = 0x80;
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    
    //Stone add for 1588, write register 0x68 len = ?
    write_8 = 1;
    test_1588_reg_68_wd();
    
    
#if 0
 //1. Setup register 0x68 to 0xXX, PTP Clock write data (index 0x0 - index 0x7)
    reg = 0x68; length=1;
 
    printf("nanosecond (00:07) :");
    scanf("%x",&nanosecond[0]);
    buf[0] = nanosecond[0];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("nanosecond (08:15) :");
    scanf("%x",&nanosecond[1]);
    buf[0] = nanosecond[1];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("nanosecond (16:23) :");
    scanf("%x",&nanosecond[2]);
    buf[0] = nanosecond[2];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    
    printf("nanosecond (24:31) :");
    scanf("%x",&nanosecond[3]);
    buf[0] = nanosecond[3];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    //========================================================================
    
    printf("second (00:07) :");
    scanf("%x",&second[0]);
    buf[0] = second[0];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (08:15) :");
    scanf("%x",&second[1]);
    buf[0] = second[1];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (16:23) :");
    scanf("%x",&second[2]);
    buf[0] = second[2];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (24:31) :");
    scanf("%x",&second[3]);
    buf[0] = second[3];
    dm_wr_reg(usb_handle, reg, length, buf); 
#endif    
    
 //2. Setup register 0x61 to 0x08, PTP Clock write
    reg = 0x61; length=1;
    buf[0] = 0x08;
    dm_wr_reg(usb_handle, reg, length, buf); 
#endif

#if 0
 //3. Setup register 0x61 to 0x01, PTP Clock Enable
    reg = 0x61; length=1;
    //buf[0] = 0x01;
    dm_rd_reg(usb_handle, reg, length, buf); 
    
    if (buf[0] == 0x02){
      reg = 0x61; length=1;
      buf[0] = 0x01;
      dm_wr_reg(usb_handle, reg, length, buf); 
    }

    for (i=0; i<100; i++){
     printf(". ");
     usleep(10000);
     }
    printf("\n");


 
 //4. Setup register 0x61 to 0x01, PTP Clock read
    reg = 0x61; length=1;
    buf[0] = 0x84;
    dm_wr_reg(usb_handle, reg, length, buf); 


 
 //5. read register 0x68, PTP Clock read data (index 0x0 - index 0x7)
    reg = 0x68; length=1;

    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("nanosecond (00:07) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("nanosecond (08:15) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("nanosecond (16:23) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf);  
    printf("nanosecond (24:31) = %d \r\n", buf[0]);
 
    dm_rd_reg(usb_handle, reg, length, buf);  
    printf("second (00:07) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("second (08:15) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("second (16:23) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("second (24:31) = %d \r\n", buf[0]);

#endif
}


/*************************************************************************/
/*      Get 1588 PTP Clock time                                          */
/*************************************************************************/
void test_1588_get_time()
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];
        int i;
        unsigned char nanosecond[4];
        unsigned char second[4];

#if 0 
    reg = 0x1f; length=1;
    buf[0] = 0x00;
    dm_wr_reg(usb_handle, reg, length, buf); 

    //1. Setup register 0x68 to 0xXX, PTP Clock write data (index 0x0 - index 0x7)
    reg = 0x68; length=1;
 
    printf("nanosecond (00:07) :");
    scanf("%d",&nanosecond[0]);
    buf[0] = nanosecond[0];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("nanosecond (08:15) :");
    scanf("%d",&nanosecond[1]);
    buf[0] = nanosecond[1];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("nanosecond (16:23) :");
    scanf("%d",&nanosecond[2]);
    buf[0] = nanosecond[2];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    
    printf("nanosecond (24:31) :");
    scanf("%d",&nanosecond[3]);
    buf[0] = nanosecond[3];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    //========================================================================
    
    printf("second (00:07) :");
    scanf("%d",&second[0]);
    buf[0] = second[0];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (08:15) :");
    scanf("%d",&second[1]);
    buf[0] = second[1];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (16:23) :");
    scanf("%d",&second[2]);
    buf[0] = second[2];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (24:31) :");
    scanf("%d",&second[3]);
    buf[0] = second[3];
    dm_wr_reg(usb_handle, reg, length, buf); 
    
    
 //2. Setup register 0x61 to 0x08, PTP Clock write
    reg = 0x61; length=1;
    buf[0] = 0x08;
    dm_wr_reg(usb_handle, reg, length, buf); 
#endif
 //3. Setup register 0x61 to 0x01, PTP Clock Enable
    reg = 0x61; length=1;
    //buf[0] = 0x01;
    dm_rd_reg(usb_handle, reg, length, buf); 
    
    if (buf[0] == 0x02){
      reg = 0x61; length=1;
      buf[0] = 0x01;
      dm_wr_reg(usb_handle, reg, length, buf); 
    }
    
#if 0
    for (i=0; i<100; i++){
     printf(". ");
     usleep(10000);
     }
    printf("\n");
#endif
 
 //4. Setup register 0x61 to 0x01, PTP Clock read
    reg = 0x61; length=1;
    buf[0] = 0x84;
    dm_wr_reg(usb_handle, reg, length, buf); 


#if 0 
 //5. read register 0x68, PTP Clock read data (index 0x0 - index 0x7)
    reg = 0x68; length=1;

    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("nanosecond (00:07) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("nanosecond (08:15) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("nanosecond (16:23) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf);  
    printf("nanosecond (24:31) = %d \r\n", buf[0]);
 
    dm_rd_reg(usb_handle, reg, length, buf);  
    printf("second (00:07) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("second (08:15) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("second (16:23) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    printf("second (24:31) = %d \r\n", buf[0]);
#endif

    //Stone add for 1588, read len = 8
    read_8 = 1;
    test_1588_reg_68_rd();    

}

/*************************************************************************/
/*      Get 1588 TX timestamp                                            */
/*************************************************************************/
void test_1588_get_tx_time()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF];
 //unsigned char nanosecond[4];
 //unsigned char second[4];
 
 //1. PTP Enable
 reg = 0x61; length=1;
 dm_rd_reg(usb_handle, reg, length, buf); 
    
 if (buf[0] == 0x02){
      reg = 0x61; length=1;
      buf[0] = 0x01;
      dm_wr_reg(usb_handle, reg, length, buf); 
 }
 
 
 //2. Enable TX Timestamp Capture, Register 0x02 bit 7 (0x80)
      reg = 0x02; length=1;
      buf[0] = 0x80;
      dm_wr_reg(usb_handle, reg, length, buf); 
 
 //3. TX a packet
      tx_pkt();
 //4. Check TX completely, register 0x02, bit 0 (0x00) (no need!)
 
 //5. Read TX timestamp, register 0x62, bit 0 (0x01)  (code in tx_pkt())


//6. Read register 0x68 (index 0 ~ index 0x07) (code in tx_pkt())

}


/*************************************************************************/
/*      Get 1588 RX timestamp                                            */
/*************************************************************************/
void test_1588_get_rx_time()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF];
 //unsigned char nanosecond[4];
 //unsigned char second[4];

 //1. PTP Enable
  reg = 0x61; length=1;
  dm_rd_reg(usb_handle, reg, length, buf); 
    
  if (buf[0] == 0x02){
      reg = 0x61; length=1;
      buf[0] = 0x01;
      dm_wr_reg(usb_handle, reg, length, buf); 
   }
 
 //2. Enable RX Timestamp Capture, Register 0x64 bit 4 (0x10),
      reg = 0x64; length=1;
      buf[0] = 0x10;
      dm_wr_reg(usb_handle, reg, length, buf); 
  

 //3. RX a packet,  RX timestamp len and format (8-byte or 4-byte), RX status bit 5 (ready), 2 (len)
     rx_pkt();   //show_rx() to check rx status!
 
}


/*************************************************************************/
/*      Test 1588 GPIO 0/1 trigger                                       */
/*************************************************************************/
void test_1588_gpio_trigger()
{
 unsigned char reg, GPIO_01, GPIO_01_inout, GPIO_01_inout_rf, GPIO_01_inout_trigger_mode, 
               GPIO_01_inout_trigger_mode_width, GPIO_01_inout_trigger_mode_width_l, GPIO_01_inout_trigger_mode_width_h;
 unsigned short length;
 unsigned char buf[SZBUF];
 unsigned int tmp;
 unsigned char nanosecond[4];
 unsigned char second[4];
 
 //1. PTP Enable
  reg = 0x61; length=1;
  dm_rd_reg(usb_handle, reg, length, buf); 
    
  if (buf[0] == 0x02){
      reg = 0x61; length=1;
      buf[0] = 0x01;
      dm_wr_reg(usb_handle, reg, length, buf); 
   }
 
 //2. Select GPIO 0/1, register 0x60 bit 1: 0=> GPIO 0(0x01), 1=> GPIO 1(0x3) 
  printf("Setup GPIO 0 or 1 (0/1):");
  scanf("%d",&GPIO_01);
  
  buf[0] = 0x01; //default value
   
  reg = 0x60; length=1; 
  if (GPIO_01 == 1)
  {
   buf[0] = 0x02;
  }
  if (GPIO_01 == 0)
  {
   buf[0] = 0x00;
  }
  
  dm_wr_reg(usb_handle, reg, length, buf);
  
  //debug
  reg = 0x60; length=1;
  dm_rd_reg(usb_handle, reg, length, buf);  
  printf("register 0x60 = %x \r\n", buf[0]);
  
 //3. Setup GPIO Control mode, register 0x6A, bit 0: 0=>trigger output(0x00), 1=> event input(0x01); bit 1: trigger/event ebable (0x02) 
  printf("Setup GPIO trigger output(0) or event input (1) (0/1):");
  scanf("%d",&GPIO_01_inout);
  
  buf[0] = 0x00; //default value
  
  reg = 0x6A; length=1;
  if (GPIO_01_inout == 1)
    buf[0] = 0x07; //trigger/event ebable (0x02) + event input(0x01) + GPIO interrupt Control enable (0x04)
  if (GPIO_01_inout == 0)
    buf[0] = 0x06; //trigger/event ebable (0x02) + trigger output(0x00) + GPIO interrupt Control enable (0x04)
    
  dm_wr_reg(usb_handle, reg, length, buf);  
  
  //debug
  reg = 0x6A; length=1;
  dm_rd_reg(usb_handle, reg, length, buf);  
  printf("register 0x6A = %x \r\n", buf[0]); 
  
 //5-1. setup GPIO trigger/event mode, register 0x6B, bit 4-6: GPIO event timestamp lock(6) + GPIO event Rise Detect(5) + GPIO event Fall Detect (4)
  if (GPIO_01_inout == 1){
    printf("Setup GPIO event mode, Rise/Fall Detect (0/1):");
    scanf("%d",&GPIO_01_inout_rf);
    
    if (GPIO_01_inout_rf == 0) //Rise
     {
      reg = 0x6B; length=1;
      buf[0] = 0x60;
      dm_wr_reg(usb_handle, reg, length, buf);   
     }
     
    if (GPIO_01_inout_rf == 1) //Fall
     {
      reg = 0x6B; length=1;
      buf[0] = 0x50;
      dm_wr_reg(usb_handle, reg, length, buf);   
     }
    
    //debug
    reg = 0x6B; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);  
    printf("register 0x6B = %x \r\n", buf[0]);
    printf("Read register 0x60 1588 interrupt status.\r\n"); 
     
 } //end of GPIO event mode setup
  
 
 //5-2.setup GPIO trigger/event mode, register 0x6B, bit 1-3: GPIO trigger output type(2:3) + GPIO trigger Polarity(1)
 if (GPIO_01_inout == 0)
 {
    printf("Setup GPIO trigger mode (active high), Output type: edge(0)/edge toggle(1)/single pulse(2)/periodic pulse(3)  (0/1/2/3):");
    scanf("%d",&GPIO_01_inout_trigger_mode);
    switch (GPIO_01_inout_trigger_mode)
    {
     case 0: //edge trigger
           reg = 0x6B; length=1;
           buf[0] = 0x02;
           dm_wr_reg(usb_handle, reg, length, buf);
           break;
     case 1: //edge toggle
           reg = 0x6B; length=1;
           buf[0] = 0x06;
           dm_wr_reg(usb_handle, reg, length, buf);
           break;
     case 2: //single pulse
           reg = 0x6B; length=1;
           buf[0] = 0x0A;
           dm_wr_reg(usb_handle, reg, length, buf);
           break;
     case 3: //periodic pulse
           reg = 0x6B; length=1;
           buf[0] = 0x0E;
           dm_wr_reg(usb_handle, reg, length, buf);
           break;
     default:
           break; 
    } //end of switch
    
    //debug
    reg = 0x6B; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);  
    printf("register 0x6B = %x \r\n", buf[0]); 
    
    
    if (GPIO_01_inout_trigger_mode == 3)
    {
     printf("Setup GPIO trigger mode (active high), periodic pulse Asserted width unit => 0: 120ns, 1: 1.024us, 2: 1ms   (0/1/2):");
     scanf("%d",&GPIO_01_inout_trigger_mode_width);
     
     switch (GPIO_01_inout_trigger_mode_width)
      {
       case 0: //120ns
             tmp = 0x00;
             break;
       case 1: //1.024us
             tmp = 0x40;
             break;
       case 2: //1ms
             tmp = 0x80;
             break;
       default:
             break;      
       }
       
     printf("Setup GPIO trigger mode (active high), periodic pulse Asserted width (High) 0x00 ~ 0x3f:");  
     scanf("%d",&GPIO_01_inout_trigger_mode_width_h);
     
     reg = 0x6D; length=1;
     buf[0] = tmp | GPIO_01_inout_trigger_mode_width_h;
     dm_wr_reg(usb_handle, reg, length, buf);
     
     //printf("buf[0] = %x , tmp = %x trigger mode = %x \n", buf[0], tmp, GPIO_01_inout_trigger_mode_width);
     
     //debug
     reg = 0x6D; length=1;
     dm_rd_reg(usb_handle, reg, length, buf);  
     printf("register 0x6D = %x \r\n", buf[0]); 
    
     
     
     printf("Setup GPIO trigger mode (active high), periodic pulse Asserted width (Low) 0x00 ~ 0xff:");  
     scanf("%d",&GPIO_01_inout_trigger_mode_width_l);
     reg = 0x6C; length=1;
     buf[0] = GPIO_01_inout_trigger_mode_width_l;
     dm_wr_reg(usb_handle, reg, length, buf);
     
     //debug
     reg = 0x6C; length=1;
     dm_rd_reg(usb_handle, reg, length, buf);  
     printf("register 0x6C = %x \r\n", buf[0]);
     
     //=========================================================================================================================
     printf("Setup GPIO trigger mode (active high), periodic pulse De-asserted width unit => 0: 120ns, 1: 1.024us, 2: 1ms   (0/1/2):");
     scanf("%d",&GPIO_01_inout_trigger_mode_width);
     
     switch (GPIO_01_inout_trigger_mode_width)
      {
       case 0: //120ns
             tmp = 0x00;
             break;
       case 1: //1.024us
             tmp = 0x40;
             break;
       case 2: //1ms
             tmp = 0x80;
             break;
       default:
             break;      
       }
       
     printf("Setup GPIO trigger mode (active high), periodic pulse De-asserted width (High) 0x00 ~ 0x3f:");  
     scanf("%d",&GPIO_01_inout_trigger_mode_width_h);
     
     reg = 0x6F; length=1;
     buf[0] = tmp | GPIO_01_inout_trigger_mode_width_h;
     
     dm_wr_reg(usb_handle, reg, length, buf);
     
     //debug
     reg = 0x6F; length=1;
     dm_rd_reg(usb_handle, reg, length, buf);  
     printf("register 0x6F = %x \r\n", buf[0]);
     
     printf("Setup GPIO trigger mode (active high), periodic pulse De-asserted width (Low) 0x00 ~ 0xff:");  
     scanf("%d",&GPIO_01_inout_trigger_mode_width_l);
     reg = 0x6E; length=1;
     buf[0] = GPIO_01_inout_trigger_mode_width_l;
     dm_wr_reg(usb_handle, reg, length, buf);
     
     //debug
     reg = 0x6E; length=1;
     dm_rd_reg(usb_handle, reg, length, buf);  
     printf("register 0x6E = %x \r\n", buf[0]);
     
    }
    
    //GPIO trigger mode, get PTP clock time and add 10 sec to trigger!
    //Setup register 0x61 to 0x01, PTP Clock read
    reg = 0x61; length=1;
    buf[0] = 0x84;
    dm_wr_reg(usb_handle, reg, length, buf); 

#if 0

   printf("====================Read PTP Clock===================================\r\n");
   //Read register 0x68, PTP Clock read data (index 0x0 - index 0x7)
    reg = 0x68; length=1;

    dm_rd_reg(usb_handle, reg, length, buf); 
    nanosecond[0] = buf[0];
    printf("nanosecond (00:07) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    nanosecond[1] = buf[0];
    printf("nanosecond (08:15) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    nanosecond[2] = buf[0];
    printf("nanosecond (16:23) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf);  
    nanosecond[3] = buf[0];
    printf("nanosecond (24:31) = %d \r\n", buf[0]);
 
    dm_rd_reg(usb_handle, reg, length, buf);  
    second[0] = buf[0];
    printf("second (00:07) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    second[1] = buf[0];
    printf("second (08:15) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    second[2] = buf[0];
    printf("second (16:23) = %d, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    second[3] = buf[0];
    printf("second (24:31) = %d \r\n", buf[0]);
    printf("=======================================================\r\n");
#endif
 
    //Stone add for 1588, read register 0x68, len = 8   
    read_8 = 1;
    test_1588_reg_68_rd();
    
#if 0    
    //Write register 0x68 (index 0-7)
    //index 0
    reg = 0x68; length=1;
    buf[0] = nanosecond[0];
    dm_wr_reg(usb_handle, reg, length, buf);
    
    //index 1
    reg = 0x68; length=1;
    buf[0] = nanosecond[1];
    dm_wr_reg(usb_handle, reg, length, buf);
    
    //index 2
    reg = 0x68; length=1;
    buf[0] = nanosecond[2];
    dm_wr_reg(usb_handle, reg, length, buf);
    
    //index 3
    reg = 0x68; length=1;
    buf[0] = nanosecond[3];
    dm_wr_reg(usb_handle, reg, length, buf);
    
    //index 4
    reg = 0x68; length=1;
    buf[0] = second[0] + 10;
    dm_wr_reg(usb_handle, reg, length, buf);
    
    //index 5
    reg = 0x68; length=1;
    buf[0] = second[1];
    dm_wr_reg(usb_handle, reg, length, buf);
    
    //index 6
    reg = 0x68; length=1;
    buf[0] = second[2];
    dm_wr_reg(usb_handle, reg, length, buf);
    
    //index 7
    reg = 0x68; length=1;
    buf[0] = second[3];
    dm_wr_reg(usb_handle, reg, length, buf);
#endif

    printf("Please setup trigger time.\r\n");
    //Stone add for 1588, read register 0x68, len = 8   
    write_8 = 1;
    test_1588_reg_68_wd();
    
    
    //Write register 0x62 to GPIO 0/1 trigger load or event read.
    
    if (GPIO_01 == 0){ //Write register 0x62 to GPIO0 (0x10)
      reg = 0x62; length=1;
      buf[0] = 0x10;
      dm_wr_reg(usb_handle, reg, length, buf);  
    }
     
    if (GPIO_01 == 1){ //Write register 0x62 to GPIO1 (0x20)
       reg = 0x62; length=1;
       buf[0] = 0x20;
       dm_wr_reg(usb_handle, reg, length, buf);  

    }
    
    reg = 0x62; length=1;
     dm_rd_reg(usb_handle, reg, length, buf);  
     printf("register 0x62 = %x \r\n", buf[0]);
    
 }//end of GPIO_01_inout == 0            
 

}


/*************************************************************************/
/*      1588 register 0x68 read                                          */
/*************************************************************************/
void test_1588_reg_68_rd()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF];
   printf("=================Read register 0x68 index 0-7================\r\n");
   //Read register 0x68, PTP Clock read data (index 0x0 - index 0x7)
   
   reg = 0x61; length=1;
    buf[0] = 0x80;
    dm_wr_reg(usb_handle, reg, length, buf);


    
    if (read_8 == 1)
     {
      reg = 0x68; length=8;
      //reg = 0x68; length=3;
      dm_rd_reg_68h(usb_handle, reg, length, buf); 
      
      printf("nanosecond (00:07) = %x, \n", buf[0]);
      printf("nanosecond (08:15) = %x, \n", buf[1]);
      printf("nanosecond (16:23) = %x, \n", buf[2]);
      printf("nanosecond (24:31) = %x, \n", buf[3]);
    
      printf("second (00:07) = %x, \n", buf[4]);
      printf("second (08:15) = %x, \n", buf[5]);
      printf("second (16:23) = %x, \n", buf[6]);
      printf("second (24:31) = %x, \n", buf[7]);
    
     }else{
     
#if 1
    reg = 0x68; length=1; 
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    //nanosecond[0] = buf[0];
    printf("nanosecond (00:07) = %x, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    //nanosecond[1] = buf[0];
    printf("nanosecond (08:15) = %x, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    //nanosecond[2] = buf[0];
    printf("nanosecond (16:23) = %x, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf);  
    //nanosecond[3] = buf[0];
    printf("nanosecond (24:31) = %x \r\n", buf[0]);
 
    dm_rd_reg(usb_handle, reg, length, buf);  
    //second[0] = buf[0];
    printf("second (00:07) = %x, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    //second[1] = buf[0];
    printf("second (08:15) = %x, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    //second[2] = buf[0];
    printf("second (16:23) = %x, \n", buf[0]);
    
    dm_rd_reg(usb_handle, reg, length, buf); 
    //second[3] = buf[0];
    printf("second (24:31) = %x \r\n", buf[0]);
#endif
     
     }
     
     
    printf("=============================================================\r\n");


}


/*************************************************************************/
/*      1588 register 0x68 write                                         */
/*************************************************************************/
void test_1588_reg_68_wd()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF];
 unsigned char nanosecond[4];
 unsigned char second[4];
 
 //1. Setup register 0x68 to 0xXX, PTP Clock write data (index 0x0 - index 0x7)
 
    reg = 0x61; length=1;
    buf[0] = 0x80;
    dm_wr_reg(usb_handle, reg, length, buf);
    
    
 
    printf("nanosecond (00:07) :");
    scanf("%x",&nanosecond[0]);
    
    
    //buf[0] = nanosecond[0];
    //dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("nanosecond (08:15) :");
    scanf("%x",&nanosecond[1]);
    //buf[0] = nanosecond[1];
    //dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("nanosecond (16:23) :");
    scanf("%x",&nanosecond[2]);
    //buf[0] = nanosecond[2];
    //dm_wr_reg(usb_handle, reg, length, buf); 
    
    
    printf("nanosecond (24:31) :");
    scanf("%x",&nanosecond[3]);
    //buf[0] = nanosecond[3];
    //dm_wr_reg(usb_handle, reg, length, buf); 
    
    //========================================================================
    
    printf("second (00:07) :");
    scanf("%x",&second[0]);
    //buf[0] = second[0];
    //dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (08:15) :");
    scanf("%x",&second[1]);
    //buf[0] = second[1];
    //dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (16:23) :");
    scanf("%x",&second[2]);
    //buf[0] = second[2];
    //dm_wr_reg(usb_handle, reg, length, buf); 
    
    printf("second (24:31) :");
    scanf("%x",&second[3]);
    //buf[0] = second[3];
    //dm_wr_reg(usb_handle, reg, length, buf); 

#if 0
    printf("nanosecond 0: %x \n", nanosecond[0]);
    printf("nanosecond 1: %x \n", nanosecond[1]);
    printf("nanosecond 2: %x \n", nanosecond[2]);
    printf("nanosecond 3: %x \n", nanosecond[3]);
    
    printf("second 0: %x \n", second[0]);
    printf("second 1: %x \n", second[1]);
    printf("second 2: %x \n", second[2]);
    printf("second 3: %x \n", second[3]);
#endif

if (write_8 == 1)
   {
    reg = 0x68; length=8;
    //reg = 0x68; length=3;
    buf[0]=nanosecond[0];
    buf[1]=nanosecond[1];
    buf[2]=nanosecond[2];    
    buf[3]=nanosecond[3];

    buf[4]=second[0];
    buf[5]=second[1];
    buf[6]=second[2];    
    buf[7]=second[3];

    dm_wr_reg_68h(usb_handle, reg, length, buf);     
    
    }else{
    
#if 1       
    reg = 0x68; length=1;
    //buf[0]=0x00;
    dm_wr_reg(usb_handle, reg, length, &nanosecond[0]);
    //dm_wr_reg(usb_handle, reg, length, buf); 
    dm_wr_reg(usb_handle, reg, length, &nanosecond[1]); 
    //dm_wr_reg(usb_handle, reg, length, buf); 
    dm_wr_reg(usb_handle, reg, length, &nanosecond[2]); 
    //dm_wr_reg(usb_handle, reg, length, buf);     
    dm_wr_reg(usb_handle, reg, length, &nanosecond[3]); 
    //dm_wr_reg(usb_handle, reg, length, buf);     
    dm_wr_reg(usb_handle, reg, length, &second[0]); 
    //dm_wr_reg(usb_handle, reg, length, buf);     
    dm_wr_reg(usb_handle, reg, length, &second[1]); 
    //dm_wr_reg(usb_handle, reg, length, buf);     
    dm_wr_reg(usb_handle, reg, length, &second[2]); 
    //dm_wr_reg(usb_handle, reg, length, buf);     
    dm_wr_reg(usb_handle, reg, length, &second[3]);
    //dm_wr_reg(usb_handle, reg, length, buf);     
#endif    
    
    }

#if 0
    //index 0
    reg = 0x68; length=1;
    dm_wr_reg(usb_handle, reg, length, &nanosecond[0]);
   
    reg = 0x10; length=1;
    buf[0]=0x00;
    
    dm_wr_reg(usb_handle, reg, length, buf);
    //dm_wr_reg(usb_handle, reg, length, &second[0]); 
    
    //index 1
    reg = 0x68; length=1;
    
    dm_wr_reg(usb_handle, reg, length, &nanosecond[1]); 
    
    reg = 0x11; length=1;
    buf[0]=0x60;
    
    dm_wr_reg(usb_handle, reg, length, buf);
    //dm_wr_reg(usb_handle, reg, length, &second[1]); 
    
    //index 2
    reg = 0x68; length=1;
    
    dm_wr_reg(usb_handle, reg, length, &nanosecond[2]); 

    reg = 0x12; length=1;
    buf[0]=0x6e;
    
    dm_wr_reg(usb_handle, reg, length, buf);
    //dm_wr_reg(usb_handle, reg, length, &second[2]);     
    
    //index 3
    reg = 0x68; length=1;
    
    dm_wr_reg(usb_handle, reg, length, &nanosecond[3]); 

    reg = 0x13; length=1;
    buf[0]=0x12;
    
    dm_wr_reg(usb_handle, reg, length, buf);    
    //dm_wr_reg(usb_handle, reg, length, &second[3]);
    
    //index 4
    reg = 0x68; length=1;
         
    dm_wr_reg(usb_handle, reg, length, &second[0]); 
    
    reg = 0x14; length=1;
    buf[0]=0x34;

    dm_wr_reg(usb_handle, reg, length, buf);     
    
    //index 5
    reg = 0x68; length=1;
    
    dm_wr_reg(usb_handle, reg, length, &second[1]); 

    reg = 0x15; length=1;
    buf[0]=0x56;

    dm_wr_reg(usb_handle, reg, length, buf);     
    
    //index 6
    reg = 0x68; length=1;
    
    dm_wr_reg(usb_handle, reg, length, &second[2]); 
    
    reg = 0x10; length=1;
    buf[0]=0x00;

    dm_wr_reg(usb_handle, reg, length, buf);     

    //index 7
    reg = 0x68; length=1;
    
    dm_wr_reg(usb_handle, reg, length, &second[3]);
    
    reg = 0x11; length=1;
    buf[0]=0x60;

    dm_wr_reg(usb_handle, reg, length, buf);       
#endif

}    
    


/*************************************************************************/
/*      GP 1/2 input/ouput test                                          */
/*************************************************************************/
void test_gp_12()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF]; 
 unsigned char GP_12, GP_12_in_out, GP_12_out_data;
 
 //1. disable PTP enable
 reg = 0x61; length=1;
 dm_rd_reg(usb_handle, reg, length, buf); 
    
  if (buf[0] != 0x02){
      reg = 0x61; length=1;
      buf[0] = 0x02;
      dm_wr_reg(usb_handle, reg, length, buf); 
   }
 
 //2. setup GP 1 or 2 be input or output, register 0x1e 
    printf("Setup GP 1 or 2 (1/2):");
    scanf("%x",&GP_12);
    
    printf("Setup input or output (1/2):");
    scanf("%x",&GP_12_in_out);
    
    //Setup register 0x1e, bit 1, 2
    if (GP_12 == 1) //GP 1
    {
     if (GP_12_in_out == 1) //0x1e bit 1 to input (0x00) 
     {
      reg = 0x1e; length=1;
      buf[0] = 0x00;
      dm_wr_reg(usb_handle, reg, length, buf); 
      }else if (GP_12_in_out == 2) //0x1e bit 1 to output (0x02)
       {
        reg = 0x1e; length=1;
        buf[0] = 0x02;
        dm_wr_reg(usb_handle, reg, length, buf); 
        }else {
        printf("GP 1 input or output select error ! please try again! \r\n");
        }
    }else if (GP_12 == 2) //GP 2 
    {
     if (GP_12_in_out == 1) //0x1e bit 2 to input (0x00) 
     {
      reg = 0x1e; length=1;
      buf[0] = 0x00;
      dm_wr_reg(usb_handle, reg, length, buf); 
      }else if (GP_12_in_out == 2) //0x1e bit 1 to output (0x04)
       {
        reg = 0x1e; length=1;
        buf[0] = 0x04;
        dm_wr_reg(usb_handle, reg, length, buf); 
        }else {
        printf("GP 2 input or output select error ! please try again! \r\n");
        }

    }else {
    printf("GP 1 or 2 select error!! Please try again! \r\n");
    }
    
 
 //3. If output need to setup data value
    if (GP_12_in_out == 2) //output
    {
     printf("Setup output data (0/1):");
     scanf("%x",&GP_12_out_data);
     
     if (GP_12_out_data == 1)
     {
      switch (GP_12)
      {
       case 1: //GP 1, output 1
              buf[0] = 0x02;
              printf("register 0x1f bit 1 should be 1 ! \r\n");
              break;
       case 2: //GP 2, output 1
              buf[0] = 0x04;
              printf("register 0x1f bit 2 should be 1 ! \r\n");
              break;
       //default:
       //       break;       
       }
       
       reg = 0x1f; length=1;
       //buf[0] = 0x00;
       dm_wr_reg(usb_handle, reg, length, buf); 
      
     }else if (GP_12_out_data == 0)
     {
     
      switch (GP_12)
      {
       case 1: //GP 1, output 0
              buf[0] = 0x00;
              printf("register 0x1f bit 1 should be 0 ! \r\n");
              break;
       case 2: //GP 2, output 0
              buf[0] = 0x00;
              printf("register 0x1f bit 2 should be 0 ! \r\n");
              break;
       //default:
       //       break;       
       }

        
       reg = 0x1f; length=1;
       //buf[0] = 0x00;
       dm_wr_reg(usb_handle, reg, length, buf); 
      
      
     }else {
      printf("Output data error! , please try again!\r\n");
     }
    
    }else if (GP_12_in_out == 1) //input
     printf("Please check register 0x1f value!\r\n");
    
 
 //4. check result!
 
}



/*************************************************************************/
/*      1588 Rate update test  (32 bits)                                 */
/*************************************************************************/
void test_1588_rate()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF]; 
 unsigned long rate;
 int neg_adj;
 

  printf("=== update Rate value ===\r\n");
  printf("add (0) or subtract (1), (0/1):");
  scanf("%x",&neg_adj);
  printf("update Rate value (32 bits):");
  scanf("%x",&rate);
  printf("Rate %8x \r\n", rate);

  reg = 0x61; length=1;
  buf[0] = 0x80;
  dm_wr_reg(usb_handle, reg, length, buf);
  
  //Update register 0x68, index 0 - index 3
  buf[0] =  rate & 0xff;
  buf[1] =  (rate & 0xff00)>>8;
  buf[2] =  (rate & 0xff0000)>>16;
  buf[3] =  (rate & 0xff000000)>>24;
  
  //printf("%x %x %x %x \n");
   reg = 0x68; length=4;
   dm_wr_reg_68h(usb_handle, reg, length, buf); 
   
  
  //register 0x61, bit 6, 0: add(0x00), 1:subtract (0x40); bit 5, Write PTP Rate Clock (0x20)
  
  if (neg_adj == 1) //subtract rate
  {
   reg = 0x61; length=1;
   buf[0] = 0x60;
   dm_wr_reg(usb_handle, reg, length, buf); 
   
  }else if (neg_adj == 0) //addt rate
   {
    reg = 0x61; length=1;
    buf[0] = 0x20;
    dm_wr_reg(usb_handle, reg, length, buf); 
   
   }
   
#if 1  //Read rate
   reg = 0x69; length=1;
   buf[0] = 0x01;
   dm_wr_reg(usb_handle, reg, length, buf); 
   
   reg = 0x61; length=1;
   buf[0] = 0x80;
   dm_wr_reg(usb_handle, reg, length, buf);  
    

   reg = 0x68; length=4;
   dm_rd_reg_68h(usb_handle, reg, length, buf);
   printf("After update rate value :\n");
   printf(" %x  %x  %x  %x \n", buf[0], buf[1], buf[2], buf[3]);
   
#endif   

}



/*************************************************************************/
/*      1588 offset update test  (idex 0 - index 7)                      */
/*************************************************************************/
void test_1588_offset()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF]; 
 unsigned long offset_nanosecond;
 unsigned long offset_second; 
 int neg_adj;
 
  printf("=== update Rate value ===\r\n");
  printf("add (0) or subtract (1), (0/1):");
  scanf("%x",&neg_adj);

  printf("update offset value (nanosecond):");
  scanf("%x",&offset_nanosecond);
  //printf("offset nanosecond %x \r\n", offset_nanosecond);

  printf("update offset value (second):");
  scanf("%x",&offset_second);
  //printf("offset second %x \r\n", offset_second);
  
  printf("1 => offset_nanosecond %x , offset_second %x \r\n", offset_nanosecond, offset_second); 

  if (neg_adj == 1)
    { 
     if (offset_second == 0)
           offset_second++;
     offset_second  = (0x100000000-offset_second);
     offset_nanosecond = (1000000000 - offset_nanosecond);
     
     }
     
    printf("2 => offset_nanosecond %x , offset_second %x \r\n", offset_nanosecond, offset_second); 
  //Update register 0x68 (index 0 - index 7)
    
    printf("second => buf [4] %x, [5] %x, [6] %x, [7] %x  \r\n", buf[4], buf [5], buf [6], buf[7]);
    
    test_1588_get_time();

    reg = 0x61; length=1;
    buf[0] = 0x80;
    dm_wr_reg(usb_handle, reg, length, buf);
    
    reg = 0x68; length=8;
    //reg = 0x68; length=3;
    buf[0]=offset_nanosecond & 0xff;
    buf[1]=(offset_nanosecond & 0xff00) >> 8;
    buf[2]=(offset_nanosecond & 0xff0000) >> 16;    
    buf[3]=offset_nanosecond  >> 24;

    buf[4]=offset_second & 0xff;
    buf[5]=(offset_second & 0xff00) >> 8;
    buf[6]=(offset_second & 0xff0000) >> 16;        
    buf[7]=offset_second  >> 24;
  
    
    dm_wr_reg_68h(usb_handle, reg, length, buf);  
  
  //Write register 0x61 bit 4 to 0x10   
   reg = 0x61; length=1;
   buf[0] = 0x10;
   dm_wr_reg(usb_handle, reg, length, buf);   
   
   test_1588_get_time();  
}


/*************************************************************************/
/*      1588 crpt test                                                   */
/*************************************************************************/
void test_1588_crypt()
{
 unsigned char reg;
 unsigned short length;
 unsigned char buf[SZBUF];
 unsigned char crypt_word, crypt_1, crypt_2;
 
 #if 1
 reg = 0x2a; length=1;
 dm_rd_reg(usb_handle, reg, length, buf);
 crypt_1 = buf[0];
 printf("%x \n", buf[0]);
 
 reg = 0x2b; length=1;
 dm_rd_reg(usb_handle, reg, length, buf);
 crypt_2 = buf[0];
 printf("%x \n", buf[0]);
#endif

   //Stone add for power on PHY
   reg = 0x1f; length=1;
   buf[0] = 0x0;
   dm_wr_reg(usb_handle, reg, length, buf);  



#if 1   //write 1588 Crypt code register 0x49 => 0x51 + 0x90 + 0xXX
   reg = 0x49; length=1;
   buf[0] = crypt_1;
   dm_wr_reg(usb_handle, reg, length, buf);     

   printf("reg1 %x \n", reg);
   reg = 0x49; length=1;
   buf[0] = crypt_2;
   //buf[0] = 0x4b;
   dm_wr_reg(usb_handle, reg, length, buf);     
   printf("reg2 %x \n", reg);


   printf("Crypt code (byte hex):");
   scanf("%hhx",&crypt_word);
   buf[0] = crypt_word;
   
   //printf("0-%x \n", buf[0]);   

   printf("reg3 %x \n", reg);
   reg = 0x49; length=1;
   dm_wr_reg(usb_handle, reg, length, buf);  
   printf("reg4 %x \n", reg);
   
   //================================================================

#if 1   
   reg = 0x49; length=1;
   buf[0] = crypt_1;
   dm_wr_reg(usb_handle, reg, length, buf);     

   printf("reg5 %x \n", reg);
   reg = 0x49; length=1;
   buf[0] = crypt_2;
   //buf[0] = 0x4b;
   dm_wr_reg(usb_handle, reg, length, buf);     
   printf("reg6 %x \n", reg);
#endif   

   reg = 0x49; length=1;
   dm_rd_reg(usb_handle, reg, length, buf);
   printf("49 key-%x \n", buf[0]); 
   printf("reg8 %x \n", reg);  

#endif


#if 0   //read 1588 Crypt code register write 0x49 => 0x51 + 0x90 then read 0x49 
   reg = 0x49; length=1;
   buf[0] = crypt_1;
   dm_wr_reg(usb_handle, reg, length, buf);     

   //buf[0] = crypt_2;
   buf[0] = 0x4b;
   dm_wr_reg(usb_handle, reg, length, buf);     


   dm_rd_reg(usb_handle, reg, length, buf);
   printf("Crypt key is %x \n", buf[0]);     

#endif
   
   crypt_key = crypt_word;
   //crypt_key = buf[0];   
   tx_pkt();
   
   

}


/*************************************************************************/
/*      TX buffer continued mode test                                    */
/*************************************************************************/

void test_tx_con(void)
{
 unsigned char reg;
 unsigned short length;
 unsigned char tx_buf[3100], buf[100];
 int i, j, k;
 
 unsigned int m_tx_size, m_max_tx_size, m_size_gap, cntx, tx_len, tx_total, tx_3b_max, tx_total_temp;
 unsigned char imr_val, rcr_val, fcr_val, reg3d_val, tcr_val, rx_rd_mem_adr; 
 
 
 //0. check register 0x3B, free buffer number (64*#)
 
 reg = 0x3B; length=1;
 dm_rd_reg(usb_handle, reg, length, buf);
 printf("1.0 Register 0x3B = %d \n", buf[0]); 

 
 //1. write register 0x30 setup to 0x10 (enable TX buffer continued mode)
 
 reg = 0x30; length=1;
 buf[0] = 0x10;
 dm_wr_reg(usb_handle, reg, length, buf); 
 
 
 //========================================
 
   reg = 0xff; length=1;
    buf[0] = 0x80;
    imr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x05; length=1;
    //if(tx_chk_rx)
        buf[0] = 0x4b;
    //else
    //    buf[0] = 0;
    rcr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0a; length=1;
    //if(no_bprs)
    //    buf[0] = 0x20;
    //else
        buf[0] = 0x30;
    fcr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x3d; length=1;
    buf[0] = 0x00;
    reg3d_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x02; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    tcr_val = buf[0];

    // clr ISR
    reg = 0x7E; length=1;
    buf[0] = 0xff;
    dm_wr_reg(usb_handle, reg, length, buf);

    // JF2_6
    reg = 0xf4; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    rx_rd_mem_adr = buf[0];
    reg = 0xf5; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
 
 
 
 
 //=============================================
 
 
 //2. prepare TX buffer data : 0-1 => TX size low/high, 
 //                            2   => bit 7: TX timesamp capture enable, bit 6: TX one-step enable, bit 5-0: reserved
 //                            3   => TX buffer data available = 0x80 
 printf("fm. TX SIZE:");
 scanf("%d", &m_tx_size);
 tx_len = m_tx_size;
//  if(tx_len > MAX_TX_SIZE) tx_len=60; // JF_12
 printf("MAX TX SIZE:");
 scanf("%d", &m_max_tx_size);
 printf("SIZE GAP:");
 scanf("%d", &m_size_gap);
 printf("TX CNT:");
 scanf("%d", &cntx); 
 
 tx_len = m_tx_size;
 
 //k => continued count; j => tx count; i => tx packet len;
 tx_total =0;
 
 
 
#if 1 //Stone add for 2 packet continued TX
 for (j=0; j<cntx; j++){
   
  reg = 0x3B; length=1;
  dm_rd_reg(usb_handle, reg, length, buf);
  printf("buf[0]*64 = %d \r\n", buf[0]*64);
  
  tx_3b_max = buf[0]*64;
  
   tx_total =0;
     
   printf("%d TX packet len = %d, register 0x3b*64 = %d \r\n", j, tx_len, tx_3b_max); 
  //printf("0. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  //TX headler 4 bytes
    tx_buf[0] = tx_len & 0xff;
    tx_buf[1] = (tx_len >> 8) & 0xff;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[4+i] = i;
     } //end of i
     
  j++;
  
  
  //======================================   
  
  tx_total = ((tx_total+tx_len+4+3)/4)*4;
  printf("%d - tx_total = %d \r\n", j-1, tx_total);
  printf("----------------------------------\r\n");
  if (j>cntx) //Stone add for tx counter !
     goto tx_end;
     
  if (tx_3b_max > tx_total){ //Stone add for check 0x3B*64 > tx_total
  
tx_add_1:  
  
   tx_len = tx_len + m_size_gap;  
  
   if (tx_len > m_max_tx_size)
      tx_len = m_tx_size;
   
   tx_total_temp = tx_total+tx_len+4;
   printf("tx_total_temp = %d \r\n", tx_total_temp);
   
   if (tx_3b_max > ((tx_total_temp+3)/4)*4){
  //printf("1. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  
    //TX headler 4 bytes
    tx_buf[tx_total] = tx_len & 0xff;
    tx_buf[tx_total+1] = (tx_len >> 8) & 0xff;
    tx_buf[tx_total+2] = 0x00;
    tx_buf[tx_total+3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[tx_total+4+i] = i;
     } //end of i
  
    printf("%d TX packet len = %d \r\n", j, tx_len);
     
    
    
    //Stone add for tx_total update!!
    tx_total = ((tx_total+tx_len+4+3)/4)*4;
    printf("%d - tx_total = %d \r\n", j, tx_total);
    printf("----------------------------------\r\n");
    
    if (j>cntx) //Stone add for tx counter !
     goto tx_end;
    
    j++;
    goto tx_add_1;
    
   }else{ //Stone add for "end of add packet to send!"
    
    tx_len = tx_len - m_size_gap;
    j--;
   }
   
   
  } //end of 0x3B*64 > tx_total
  
  //printf("2. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  
  //} end of k
  
tx_end:  
  //Stone add for check TX free space size > TX packet count ? 
  //(register 0x3B * 64 > tx_total)
  reg = 0x3B; length=1;
  dm_rd_reg(usb_handle, reg, length, buf);
  printf("buf[0]*64 = %d, ((tx_total+3)/4)*4 = %d \r\n", buf[0]*64, ((tx_total+3)/4)*4);
  
  do 
  {
    if ((buf[0]*64) > (((tx_total+3)/4)*4)){
      //3. Write TX memorry
      reg = 0x78;
      dm_wr_spi_mems(usb_handle, reg, ((tx_total+3)/4)*4, tx_buf); 
  
      reg = 0x02; length=1;
      buf[0] = tcr_val | 0x01;
      
      dm_wr_reg(usb_handle, reg, length, buf);
      goto tx_ok;
      
      }else {
      
      reg = 0x3B; length=1;
      dm_rd_reg(usb_handle, reg, length, buf);
      printf("3.0 Register 0x3B = %d , buf[0]*64 = %d \r\n", buf[0], buf[0]*64);
      }
      
   }while((buf[0]*64) <= (((tx_total+3)/4)*4));
  
tx_ok:
   
#if 0   
  reg = 0x3B; length=1;
  dm_rd_reg(usb_handle, reg, length, buf);
 
 printf("2.0 Register 0x3B = %d total len = %d\n", buf[0], ((tx_total+3)/4)*4);
 printf("==============================================\r\n");
#endif
   
  printf("==============================================\r\n"); 
  //======================================= 
  tx_len = tx_len + m_size_gap;
  
  if (tx_len > m_max_tx_size)
     tx_len = m_tx_size;
  
  
  } //end of j
  
#endif

#if 0  //Stone add for 1532 + 1500 + 1501 + 1502 loop

 for (j=0; j<cntx; j++){
 
   tx_total =0;
   tx_len = 1532;
   printf("%d TX packet len = %d \r\n", j, tx_len); 
  //printf("0. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  //TX headler 4 bytes
    tx_buf[0] = tx_len & 0xff;
    tx_buf[1] = (tx_len >> 8) & 0xff;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[4+i] = i;
     } //end of i
  
  tx_total = ((tx_total+tx_len+4+3)/4)*4;
  
  //tx_len = tx_len + m_size_gap;  
  
  //if (tx_len > m_max_tx_size)
  //   tx_len = m_tx_size;
  
  tx_len = 1500;
  j++;
  //printf("1. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  printf("%d TX packet len = %d \r\n", j, tx_len);
  
  
  //for (k=1; k<2; k++){
  
    //TX headler 4 bytes
    tx_buf[tx_total] = tx_len & 0xff;
    tx_buf[tx_total+1] = (tx_len >> 8) & 0xff;
    tx_buf[tx_total+2] = 0x00;
    tx_buf[tx_total+3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[tx_total+4+i] = i;
     } //end of i
     
  tx_total = tx_total+tx_len+4;
  j++;
  
  //printf("2. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  
  //} end of k
   
  //3. Write TX memorry
  reg = 0x78;
  dm_wr_spi_mems(usb_handle, reg, ((tx_total+3)/4)*4, tx_buf); 
  
  reg = 0x02; length=1;
  buf[0] = tcr_val | 0x01;
  dm_wr_reg(usb_handle, reg, length, buf);
  
  reg = 0x3B; length=1;
 dm_rd_reg(usb_handle, reg, length, buf);
 
 printf("2.0 Register 0x3B = %x total len = %d\n", buf[0], ((tx_total+3)/4)*4);
 printf("==============================================\r\n");



 tx_total =0;
   tx_len = 1501;
   printf("%d TX packet len = %d \r\n", j, tx_len); 
  //printf("0. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  //TX headler 4 bytes
    tx_buf[0] = tx_len & 0xff;
    tx_buf[1] = (tx_len >> 8) & 0xff;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[4+i] = i;
     } //end of i
  
  tx_total = ((tx_total+tx_len+4+3)/4)*4;
  
  //tx_len = tx_len + m_size_gap;  
  
  //if (tx_len > m_max_tx_size)
  //   tx_len = m_tx_size;
  
  tx_len = 1502;
  j++;
  //printf("1. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  printf("%d TX packet len = %d \r\n", j, tx_len);
  
  
  //for (k=1; k<2; k++){
  
    //TX headler 4 bytes
    tx_buf[tx_total] = tx_len & 0xff;
    tx_buf[tx_total+1] = (tx_len >> 8) & 0xff;
    tx_buf[tx_total+2] = 0x00;
    tx_buf[tx_total+3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[tx_total+4+i] = i;
     } //end of i
     
  tx_total = tx_total+tx_len+4;
  
  //printf("2. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  
  //} end of k
   
  //3. Write TX memorry
  reg = 0x78;
  dm_wr_spi_mems(usb_handle, reg, ((tx_total+3)/4)*4, tx_buf); 
  
  reg = 0x02; length=1;
  buf[0] = tcr_val | 0x01;
  dm_wr_reg(usb_handle, reg, length, buf);
  
  reg = 0x3B; length=1;
 dm_rd_reg(usb_handle, reg, length, buf);
 
 printf("2.0 Register 0x3B = %x total len = %d\n", buf[0], ((tx_total+3)/4)*4);
 printf("==============================================\r\n");

   
  //======================================= 
  //tx_len = tx_len + m_size_gap;
  
  //if (tx_len > m_max_tx_size)
  //   tx_len = m_tx_size;
  
  
  } //end of j
 
#endif  
  printf("total TX packet count = %d \r\n", j);

}





#if 0 //Stone add for debug TX free space = 3072! (free space only 3071)

/*************************************************************************/
/*      TX buffer continued mode test                                    */
/*************************************************************************/

void test_tx_con(void)
{
 unsigned char reg;
 unsigned short length;
 unsigned char tx_buf[3100], buf[100];
 int i, j, k;
 
 unsigned int m_tx_size, m_max_tx_size, m_size_gap, cntx, tx_len, tx_total;
 unsigned char imr_val, rcr_val, fcr_val, reg3d_val, tcr_val, rx_rd_mem_adr; 
 
 
 //0. check register 0x3B, free buffer number (64*#)
 
 reg = 0x3B; length=1;
 dm_rd_reg(usb_handle, reg, length, buf);
 printf("1.0 Register 0x3B = %d \n", buf[0]); 

 
 //1. write register 0x30 setup to 0x10 (enable TX buffer continued mode)
 
 reg = 0x30; length=1;
 buf[0] = 0x10;
 dm_wr_reg(usb_handle, reg, length, buf); 
 
 
 //========================================
 
   reg = 0xff; length=1;
    buf[0] = 0x80;
    imr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x05; length=1;
    //if(tx_chk_rx)
        buf[0] = 0x4b;
    //else
    //    buf[0] = 0;
    rcr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0a; length=1;
    //if(no_bprs)
    //    buf[0] = 0x20;
    //else
        buf[0] = 0x30;
    fcr_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x3d; length=1;
    buf[0] = 0x00;
    reg3d_val = buf[0];
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x02; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    tcr_val = buf[0];

    // clr ISR
    reg = 0x7E; length=1;
    buf[0] = 0xff;
    dm_wr_reg(usb_handle, reg, length, buf);

    // JF2_6
    reg = 0xf4; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
    rx_rd_mem_adr = buf[0];
    reg = 0xf5; length=1;
    dm_rd_reg(usb_handle, reg, length, buf);
 
 
 
 
 //=============================================
 
 
 //2. prepare TX buffer data : 0-1 => TX size low/high, 
 //                            2   => bit 7: TX timesamp capture enable, bit 6: TX one-step enable, bit 5-0: reserved
 //                            3   => TX buffer data available = 0x80 
 printf("fm. TX SIZE:");
 scanf("%d", &m_tx_size);
 tx_len = m_tx_size;
//  if(tx_len > MAX_TX_SIZE) tx_len=60; // JF_12
 printf("MAX TX SIZE:");
 scanf("%d", &m_max_tx_size);
 printf("SIZE GAP:");
 scanf("%d", &m_size_gap);
 printf("TX CNT:");
 scanf("%d", &cntx); 
 
 tx_len = m_tx_size;
 
 //k => continued count; j => tx count; i => tx packet len;
 tx_total =0;
 
 
 
#if 1 //Stone add for 2 packet continued TX
 for (j=0; j<cntx; j++){
 
   tx_total =0;
  
   printf("%d TX packet len = %d \r\n", j, tx_len); 
  //printf("0. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  //TX headler 4 bytes
    tx_buf[0] = tx_len & 0xff;
    tx_buf[1] = (tx_len >> 8) & 0xff;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[4+i] = i;
     } //end of i
  
  tx_total = ((tx_total+tx_len+4+3)/4)*4;
  
  tx_len = tx_len + m_size_gap;  
  
  if (tx_len > m_max_tx_size)
     tx_len = m_tx_size;
  
  j++;
  //printf("1. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  printf("%d TX packet len = %d \r\n", j, tx_len);
  
  
  //for (k=1; k<2; k++){
  
    //TX headler 4 bytes
    tx_buf[tx_total] = tx_len & 0xff;
    tx_buf[tx_total+1] = (tx_len >> 8) & 0xff;
    tx_buf[tx_total+2] = 0x00;
    tx_buf[tx_total+3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[tx_total+4+i] = i;
     } //end of i
     
  tx_total = tx_total+tx_len+4;
  
  //printf("2. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  
  //} end of k
  
  //Stone add for check TX free space size > TX packet count ? 
  //(register 0x3B * 64 > tx_total)
  reg = 0x3B; length=1;
  dm_rd_reg(usb_handle, reg, length, buf);
  printf("buf[0]*64 = %d, ((tx_total+3)/4)*4 = %d \r\n", buf[0]*64, ((tx_total+3)/4)*4);
  
  do 
  {
    if ((buf[0]*64) > (((tx_total+3)/4)*4)){
      //3. Write TX memorry
      reg = 0x78;
      dm_wr_spi_mems(usb_handle, reg, ((tx_total+3)/4)*4, tx_buf); 
  
      reg = 0x02; length=1;
      buf[0] = tcr_val | 0x01;
      
      dm_wr_reg(usb_handle, reg, length, buf);
      goto tx_ok;
      
      }else {
      
      reg = 0x3B; length=1;
      dm_rd_reg(usb_handle, reg, length, buf);
      printf("3.0 Register 0x3B = %d , buf[0]*64 = %d \r\n", buf[0], buf[0]*64);
      }
      
   }while((buf[0]*64) <= (((tx_total+3)/4)*4));
  
tx_ok:
   
#if 0   
  reg = 0x3B; length=1;
  dm_rd_reg(usb_handle, reg, length, buf);
 
 printf("2.0 Register 0x3B = %d total len = %d\n", buf[0], ((tx_total+3)/4)*4);
 printf("==============================================\r\n");
#endif
   
  printf("==============================================\r\n"); 
  //======================================= 
  tx_len = tx_len + m_size_gap;
  
  if (tx_len > m_max_tx_size)
     tx_len = m_tx_size;
  
  
  } //end of j
  
#endif

#if 0  //Stone add for 1532 + 1500 + 1501 + 1502 loop

 for (j=0; j<cntx; j++){
 
   tx_total =0;
   tx_len = 1532;
   printf("%d TX packet len = %d \r\n", j, tx_len); 
  //printf("0. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  //TX headler 4 bytes
    tx_buf[0] = tx_len & 0xff;
    tx_buf[1] = (tx_len >> 8) & 0xff;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[4+i] = i;
     } //end of i
  
  tx_total = ((tx_total+tx_len+4+3)/4)*4;
  
  //tx_len = tx_len + m_size_gap;  
  
  //if (tx_len > m_max_tx_size)
  //   tx_len = m_tx_size;
  
  tx_len = 1500;
  j++;
  //printf("1. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  printf("%d TX packet len = %d \r\n", j, tx_len);
  
  
  //for (k=1; k<2; k++){
  
    //TX headler 4 bytes
    tx_buf[tx_total] = tx_len & 0xff;
    tx_buf[tx_total+1] = (tx_len >> 8) & 0xff;
    tx_buf[tx_total+2] = 0x00;
    tx_buf[tx_total+3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[tx_total+4+i] = i;
     } //end of i
     
  tx_total = tx_total+tx_len+4;
  j++;
  
  //printf("2. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  
  //} end of k
   
  //3. Write TX memorry
  reg = 0x78;
  dm_wr_spi_mems(usb_handle, reg, ((tx_total+3)/4)*4, tx_buf); 
  
  reg = 0x02; length=1;
  buf[0] = tcr_val | 0x01;
  dm_wr_reg(usb_handle, reg, length, buf);
  
  reg = 0x3B; length=1;
 dm_rd_reg(usb_handle, reg, length, buf);
 
 printf("2.0 Register 0x3B = %x total len = %d\n", buf[0], ((tx_total+3)/4)*4);
 printf("==============================================\r\n");



 tx_total =0;
   tx_len = 1501;
   printf("%d TX packet len = %d \r\n", j, tx_len); 
  //printf("0. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  //TX headler 4 bytes
    tx_buf[0] = tx_len & 0xff;
    tx_buf[1] = (tx_len >> 8) & 0xff;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[4+i] = i;
     } //end of i
  
  tx_total = ((tx_total+tx_len+4+3)/4)*4;
  
  //tx_len = tx_len + m_size_gap;  
  
  //if (tx_len > m_max_tx_size)
  //   tx_len = m_tx_size;
  
  tx_len = 1502;
  j++;
  //printf("1. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  printf("%d TX packet len = %d \r\n", j, tx_len);
  
  
  //for (k=1; k<2; k++){
  
    //TX headler 4 bytes
    tx_buf[tx_total] = tx_len & 0xff;
    tx_buf[tx_total+1] = (tx_len >> 8) & 0xff;
    tx_buf[tx_total+2] = 0x00;
    tx_buf[tx_total+3] = 0x80;
 
    for (i=0; i<tx_len ; i++)
    {
     tx_buf[tx_total+4+i] = i;
     } //end of i
     
  tx_total = tx_total+tx_len+4;
  
  //printf("2. tx_total = %x, tx_len = %x, j = %x \r\n", tx_total, tx_len, j);
  
  //} end of k
   
  //3. Write TX memorry
  reg = 0x78;
  dm_wr_spi_mems(usb_handle, reg, ((tx_total+3)/4)*4, tx_buf); 
  
  reg = 0x02; length=1;
  buf[0] = tcr_val | 0x01;
  dm_wr_reg(usb_handle, reg, length, buf);
  
  reg = 0x3B; length=1;
 dm_rd_reg(usb_handle, reg, length, buf);
 
 printf("2.0 Register 0x3B = %x total len = %d\n", buf[0], ((tx_total+3)/4)*4);
 printf("==============================================\r\n");

   
  //======================================= 
  //tx_len = tx_len + m_size_gap;
  
  //if (tx_len > m_max_tx_size)
  //   tx_len = m_tx_size;
  
  
  } //end of j
 
#endif  
  printf("total TX packet count = %d \r\n", j);

}

#endif







/*************************************************************************/
/*      do software reset and init. registers                            */
/*************************************************************************/
void sw_rst_init()
{
        unsigned char reg;
        unsigned short length;
        unsigned char buf[SZBUF];

    reg = 0x00; length=1;
    buf[0] = 0x01;
    dm_wr_reg(usb_handle, reg, length, buf);
    usleep(100);

    reg = 0x00; length=1;
    buf[0] = ncr_val;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0xff; length=1;
    buf[0] = imr_val;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x0a; length=1;
    buf[0] = fcr_val;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x05; length=1;
    buf[0] = rcr_val;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x31; length=1;
    buf[0] = reg31_val;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x32; length=1;
    buf[0] = reg32_val;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x3d; length=1;
    buf[0] = reg3d_val;
    dm_wr_reg(usb_handle, reg, length, buf);
}
/*************************************************************************/
/*      write MAC memory                                                 */
/*************************************************************************/
void wr_mem()
{
unsigned char reg;
unsigned short length;
unsigned char buf[16400];
unsigned char bufx[2];
unsigned int i;
unsigned int   mem_bgn, mem_len, pat_type;
unsigned char  bdatax;
unsigned char  exp_bval0, exp_bval1;
unsigned char  tst_result;


    printf("START ADDR(Hex 0 or C00~3FFF):");
    scanf("%x",&mem_bgn);
    printf("BYTE count(DEC 1~3072~16384):");
    scanf("%d",&mem_len);
    if(mem_len > 16384) mem_len=16384;
    printf("DATA pattern(0:0, 1:55, 2:AA, 3:FF, 4:INC, 5:DEC):");
    scanf("%d",&pat_type);

    reg = 0x7F;
    length=1;
    buf[0] = 0x00;
    dm_wr_reg(usb_handle, reg, length, buf);

    reg = 0x7A;
    length=1;
    buf[0] = mem_bgn & 0x00ff;
    dm_wr_reg(usb_handle, reg, length, buf);
    reg = 0x7B;
    length=1;
    buf[0] = (mem_bgn & 0xff00) >> 8;
    dm_wr_reg(usb_handle, reg, length, buf);

    switch (pat_type)
    {
    case 0: bdatax = 0;
            break;
    case 1: bdatax = 0x55;
            break;
    case 2: bdatax = 0xAA;
            break;
    case 3: bdatax = 0xFF;
            break;
        default:
            bdatax = 0x55;
    }

    for(i=0;i<(mem_len+1)/2;i++)
    {
        if(pat_type == 4)
        {
            buf[i*2] = (i*2)&0xff;
            buf[i*2+1] = (i*2+1)&0xff;
        }
        else
        if(pat_type == 5)
        {
            buf[i*2] = (0x100-i*2)&0xff;
            buf[i*2+1] = (0x100-i*2-1)&0xff;
        }
        else
        {
            buf[i*2] = bdatax;
            buf[i*2+1] = bdatax;
        }
    }
    reg = 0x78;
    length=mem_len;
    dm_wr_spi_mems(usb_handle, reg, length, buf);
    printf("end of write memory\n");


    if(mem_tst)
    {
        reg = 0x74;
        length=1;
        buf[0] = mem_bgn & 0x00ff;
        dm_wr_reg(usb_handle, reg, length, buf);

        reg = 0x75;
        length=1;
        buf[0] = (mem_bgn & 0xff00) >> 8;
        dm_wr_reg(usb_handle, reg, length, buf);

        reg = 0x70;
        length=1;
        dm_rd_reg(usb_handle, reg, length, buf);

        tst_result=1;
        reg = 0x72;
        length=mem_len;
        dm_rd_spi_mems(usb_handle, reg, length, buf);

        for(i=0;i<(mem_len+1)/2;i++)
        {
            bufx[0] = buf[i*2];
            bufx[1] = buf[i*2+1];
            if(pat_type==4)
            {
                exp_bval0 = (i*2)&0xff;
                exp_bval1 = (i*2+1)&0xff;
            }
            else
            if(pat_type==5)
            {
                exp_bval0 = (0x100-i*2)&0xff;
                exp_bval1 = (0x100-i*2-1)&0xff;
            }
            else
            {
                exp_bval0 = bdatax;
                exp_bval1 = bdatax;
            }

            if(bufx[0]!=exp_bval0)
            {
                tst_result=0;
                printf("error at %4X, rd=%2X, wr=%2X\n",
                      mem_bgn+i*2,bufx[0], exp_bval0);
            }
            if(bufx[1]!=exp_bval1)
            {
                tst_result=0;
                printf("error at %4X, rd=%2X, wr=%2X\n",
                      mem_bgn+i*2+1,bufx[1], exp_bval1);
            }
        }
        if(tst_result)
        {
            printf("memory test PASS\n");
        }
   }
}
/********************************************************************/
/*      read  MAC memory                                            */
/********************************************************************/
void rd_mem()
{
unsigned char reg;
unsigned short length;
unsigned char buf[16400];
//unsigned char bufx[2];
//int nread;
int j,k;
unsigned int   mem_bgn, mem_len;
unsigned int   line_cnt, remain_xx;

    printf("START ADDR(Hex 0 or C00~3FFF):");
    scanf("%x",&mem_bgn);
    printf("BYTE count(DEC 1~3072~16384):");
    scanf("%d",&mem_len);
    if(mem_len > 16384) mem_len=16384;

        reg = 0x7F;
        length=1;
        buf[0] = 0x00;
        dm_rd_reg(usb_handle, reg, length, buf);

        reg = 0x74;
        length=1;
        buf[0] = mem_bgn & 0x00ff;
        dm_wr_reg(usb_handle, reg, length, buf);

        reg = 0x75;
        length=1;
        buf[0] = (mem_bgn & 0xff00) >> 8;
        dm_wr_reg(usb_handle, reg, length, buf);

        reg = 0x70;
        length=1;
        dm_rd_reg(usb_handle, reg, length, buf);

        reg = 0x72;
        length=mem_len;
        dm_rd_spi_mems(usb_handle, reg, length, buf);

    line_cnt = mem_len/16;
    remain_xx = mem_len - line_cnt * 16;

    for(k=0;k<line_cnt;k++)
    {
        printf("%4X: ",mem_bgn+k*16);
        for(j=0;j<8;j++)
        {
            // JFW_2
            printf("%2X %2X ",
                buf[k*16+j*2 + 0], 
                buf[k*16+j*2 + 1]);
        }
        printf("\n");
    }

    printf("%4X: ",mem_bgn+k*16);
    for(j=0;j<remain_xx;j++)
    {
        // JFW_2
    //  printf("%2X ", buf[mem_bgn+k*16+j]);
        printf("%2X ", buf[k*16+j]);
    }
    printf("\n");
    printf("end of read memory\n");

}



