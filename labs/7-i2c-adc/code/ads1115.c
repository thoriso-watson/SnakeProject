/*
   simple driver for a counterfeit texas inst ads1115 adc 
   datasheet is in docs.

   the quickstart guide on p35 describes how the bytes go 
   back and forth:
        - write high byte first.
        - read returns high byte first.

   given the lab setting, we jam everything into one file --- 
   after your code works you should pull the ads1115 code into
   its own file so you can add it easily to other projects.
 */
#include "rpi.h"
#include "i2c.h"
#include "ads1115.h"
#include "libc/bit-support.h"

// for bitfield checking
#include "libc/helper-macros.h"

// p27: register names
enum { conversion_reg = ??, config_reg = ?? };


// p21 states device will reset itself when you do an
// i2c "general call" reset with 6h (6 hex = 0x6)
// we need to do this b/c  ---- and this is an easy 
// mistake to make --- even though the pi reboots,
// the adc (and all sensors) are still up and running,
// so have the previous configuration.
void ads1115_reset(void) {
    uint8_t rst = 0x6;
    i2c_write(0, &rst,1);
}

// write a 16 bite register
// p23 to read/write registers
//  1. write to the low two bits in the address pointer register. (p27)
//  2. read the 2 bytes that come back.
void ads1115_write16(uint8_t dev_addr, uint8_t reg, uint16_t v) {
    unimplemented();
}

// read a 16-bit register
uint16_t ads1115_read16(uint8_t dev_addr, uint8_t reg) {
    unimplemented();
}

// returns the device addr: hard-coded configuration atm.
uint8_t ads1115_config(void) {
    // dev address: p23
    enum { dev_addr = ?? };
    panic("set dev_addr!\n");

    // p28
    // one way to set fields in a register.
    //   note, these are not clearing them.
#   define PGA(x) ((x)<<(9))
#   define MODE(x) ((x)<<(8))
#   define DR(x) ((x)<<5)
#   define MUX(x) ((x)<<12)

#   define MODE_V(x) bits_get(x,8,8)    // ((x) >> 8)&1)
#   define DR_V(x)   bits_get(x,5,7)  // (((x)>>5)&0b111)
#   define PGA_V(x)  bits_get(x,9,11)  // (((x)>>9)&0b111)
#   define MUX_V(x)  bits_get(x,12,14) // (((x)>>12)&0b111)

    // reset device to known state.
    ads1115_reset();

    // we seem to have to wait for power up.
    delay_us(25);

    // 1. get the initial config
	uint16_t c = ads1115_read16(dev_addr, config_reg);
	printk("initial: config reg = =%x (lo=%b,hi=%b)\n", c,c&0xff,c>>8);

    // 2. sanity check the default default values from p29
    //   mode: 8 is 1
    assert(MODE_V(c) == 1);
    //   dr: 7:5 = 0b100
    assert(DR_V(c) == 0b100);
    //   pg: 11:9 = 010
    assert(PGA_V(c) == 0b010);

    // 3. set the config to:
    //  - PGA gain to +/-4v 
    //  - MODE to continuous.
    //  - DR to 860sps
    // see page 28.
    unimplemented();

	printk("writing config: 0x%x\n", c);
    ads1115_write16(dev_addr, config_reg, c);

    // 4. read back the config and make sure the fields we set
    // are correct.
    unimplemented();

    return dev_addr;
}

void notmain(void) {
	uart_init();
	delay_ms(30);   // allow time for device to boot up.
	i2c_init();
	delay_ms(30);   // allow time to settle after init.

    unsigned dev_addr = ads1115_config();

    // 5. just loop and get readings.
    //  - vary your potentiometer
    //  - should get negative readings for low
    //  - around 20k for high.
    //
    // 
    // make sure: given we set gain to +/- 4v.
    // does the result make sense?
	for(int i = 0; i < 10; i++) {
        short v = ads1115_read16(dev_addr, conversion_reg);
        printk("out=%d\n", v);
		delay_ms(1000);
	}
	clean_reboot();
}

