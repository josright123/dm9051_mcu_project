#include "dm9051_lw.h" //#include "dm9051opts.h"
#include "dm9051_lw_conf.h" //#include "dm9051_lw_decl.h"
#include "dm9051_lw_sync.h"

//int board_printf(const char *format, args...) { return 0; }
//.#define	board_printf(format, args...)

//.void dm_delay_us(uint32_t nus) {
//.	void delay_us(uint32_t nus);
//.	board_printf("test %d ,because rxb %02x (is %d times)\r\n", rstccc, rxbyteee, timesss);
//.	delay_us(nus);
//.}
//.void dm_delay_ms(uint16_t nms) {
//.	void delay_ms(uint16_t nms);
//.	delay_ms(nms);
//.}

//#include "dm9051_at32_porting.s.c"
//#include "dm9051_at32_porting.e.c"

//.static void spi_cs_low(void) {
//.	gpio_bits_reset(devconf.spi_pack->wire_cs.gpport, devconf.spi_pack->wire_cs.pin); //cs.gpport->clr = cs.pin;
//.}
//.static void spi_cs_high(void) {
//.	gpio_bits_set(devconf.spi_pack->wire_cs.gpport, devconf.spi_pack->wire_cs.pin); //cs.gpport->scr = cs.pin;
//.}

//.static uint8_t spi_exc_data(uint8_t byte) {
//.    while(spi_i2s_flag_get(spi_no(), SPI_I2S_TDBE_FLAG) == RESET);	//while(spi_i2s_flag_get(SPI2, SPI_I2S_TDBE_FLAG) == RESET);
//.    spi_i2s_data_transmit(spi_no(), byte);							//spi_i2s_data_transmit(SPI2, byte); //spi2 tx	
//.    while(spi_i2s_flag_get(spi_no(), SPI_I2S_RDBF_FLAG) == RESET);	//while(spi_i2s_flag_get(SPI2, SPI_I2S_RDBF_FLAG) == RESET);
//.    return (uint8_t) spi_i2s_data_receive(spi_no());				//return (uint8_t) spi_i2s_data_receive(SPI2); //spi2 rx (rx_pad)
//.}

//void dm9051_interface_register_all(const dm9051_interface_t *p_intface);
//..........................................................................................

typedef struct{
    void (*delay_us)(uint32_t time); // Delay, unit: us
	int (*rstb_exist)(void);	// RSTB exist!
    void (*rst_pulse)(void);      // RSTB pin
    void (*cs_low)(void);    // SCS pin
    void (*cs_high)(void);    // SCS pin
    uint8_t (*exc_data)(uint8_t data);	// Exchange 1 byte data by write and read spi
} dm9051_interface_t;

//.void dm9051_register_spi(void) {
//.  dm9051_interface_register_all(&dm9051_interface);
//.}

static const dm9051_interface_t dm9051_interface = {
	dm_delay_us, //delay_us,
	rst_pin_exist,
	rst_pin_pulse,
	spi_cs_low,
	spi_cs_high,
	spi_exc_data,
};

//.extern const dm9051_interface_t *dm9051_if;
const dm9051_interface_t *dm9051_if = &dm9051_interface;

#define dm9051_rstb_exist() dm9051_if->rstb_exist()
#define dm9051_reset_pulse() dm9051_if->rst_pulse()
#define dm9051_spi_cs_lo() dm9051_if->cs_low()
#define dm9051_spi_command_write(rd) dm9051_if->exc_data(rd)
#define dm9051_spi_dummy_read() dm9051_if->exc_data(0)
#define dm9051_spi_cs_hi() dm9051_if->cs_high()

void pin_poweron_reset(void)
{
	if (!dm9051_rstb_exist())
		return;
	dm9051_reset_pulse();
}

uint8_t cspi_read_reg(uint8_t reg) //static (todo)
{
	uint8_t val;
	dm9051_spi_cs_lo();
	dm9051_spi_command_write(reg | OPC_REG_R);
	val = dm9051_spi_dummy_read();
	dm9051_spi_cs_hi();
	return val; //printf("[..][%02x]\r\n", val); //from dm9051
}
void cspi_write_reg(uint8_t reg, uint8_t val)
{
	dm9051_spi_cs_lo();
	dm9051_spi_command_write(reg | OPC_REG_W);
	dm9051_spi_command_write(val);
	dm9051_spi_cs_hi();
}
uint8_t spi_read_mem2x(void)
{
	int rxb;
	dm9051_spi_cs_lo();
	dm9051_spi_command_write(DM9051_MRCMDX | OPC_REG_R);
	rxb = dm9051_spi_dummy_read();
	rxb = dm9051_spi_dummy_read();
	dm9051_spi_cs_hi();
	return rxb;
}
void spi_read_mem(u8 *buf, u16 len)
{
	int i;
	dm9051_spi_cs_lo();
	dm9051_spi_command_write(DM9051_MRCMD | OPC_REG_R);
	if (len & 1)
		len++;
	for(i=0; i<len; i++)
		buf[i] = dm9051_spi_dummy_read();
	dm9051_spi_cs_hi();
}
void spi_write_mem(u8 *buf, u16 len)
{
	int i;
	dm9051_spi_cs_lo();
	dm9051_spi_command_write(DM9051_MWCMD | OPC_REG_W);
	if (len & 1)
		len++;
	for(i=0; i<len; i++)
		dm9051_spi_command_write(buf[i]);
	dm9051_spi_cs_hi();
}
