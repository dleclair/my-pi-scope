
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/i2c.h>

static ktime_t interval;
static struct hrtimer ads1015_timer;

/**********************************************************************
 * i2c setup
 **********************************************************************/

#define I2C_BUS 1	      // wired I2C bus on RASPI
#define ADS1015_ADDRESS 0x48  // Default I2C address for ADS1015

struct i2c_adapter* i2c_adapter;
struct i2c_client* i2c_client;

//static struct timer_list i2c_timer;

static struct i2c_board_info board_info = {
	I2C_BOARD_INFO("foodev", ADS1015_ADDRESS),
};


/**********************************************************************
 * ADS1015 setup
 **********************************************************************/
// 3.0 mV per step @ +/- 6.144V dynamic range
#define FSR_LSB 3000

#define CONFIG_REG 0x01       // Configuration register
#define CONVERSION_REG 0x00   // Conversion register


long read_ads1015_sample(void) {
    // Read the conversion result
    int raw_adc = i2c_smbus_read_word_data(i2c_client, CONVERSION_REG);
    //char read_data[2];
    //if (i2cReadI2CBlockData(i2c_handle, CONVERSION_REG, read_data, 2) != 2) {
    //    pr_info("Failed to read conversion result\n");
    //    return -1.0;
    //}

    // Combine the two bytes into a single 12-bit value
    //int16_t raw_adc = ((read_data[0] << 4) | (read_data[1] >> 4));

    // Handle negative values (ADS1015 output is in 2's complement)
    //if (raw_adc > 0x07FF) {
    //    raw_adc |= 0xF000;
    //}
    long sample_voltage = raw_adc * FSR_LSB;

    // Return the sample voltage
    return sample_voltage;
}


enum hrtimer_restart timer_event(struct hrtimer *t) {
	printk("in timer_event()\n");

	u64 t1, t2, elapsed_time;
	t1 = ktime_get_ns();
	long sample_voltage = 0;
	//sample_voltage = read_ads1015_sample();
	t2 = ktime_get_ns();
	elapsed_time = t2 - t1;
	pr_info("sample: %ld V, elapsed_time = %llu usecs\n", sample_voltage, elapsed_time);
	long skips = hrtimer_forward_now(t, interval);
	if (skips > 1) {
		printk("skipped %ld intervals.", skips);
	}
	return HRTIMER_RESTART;
}


void ads1015_sampler_init(void) {
	printk("in ads1015_sampler_init()\n");
	//pr_info("opening GPIO access\n");
    	// Initialize the pigpio library
    	//if (gpioInitialise() < 0) {
	//	pr_info("pigpio initialization failed\n");
        //	return;
    	//}

	pr_info("connecting to i2c bus\n");
	// Setup device
	i2c_adapter = i2c_get_adapter(I2C_BUS);
	i2c_client = i2c_new_client_device(i2c_dev, board_info);

	pr_info("turning on ADS1015 in continuous mode, AIN0, 3300 SPS, +/-6.144V\n");
    	// Configure ADS1015 for single-ended input on AIN0 with default setting
	// MSB: OS: start, MUX: AINP=AIN0 / AINN=GND, PGA = ±6.144V, MODE = Continuous
	// LSB: DR = 3300 SPS, COMP_MODE = traditional, COMP_POL = active low, COMP_LAT = non-latching, COMP_QUE = disable
	u16 config_data = 0xC0E3;
	int result = i2c_smbus_write_word_data(i2c_client, CONFIG_REG, config_data);
	pr_info("config write returned %d.\n", result);

    	//char config_data[3];
    	//config_data[0] = CONFIG_REG;
    	//config_data[1] = 0xC0;  // MSB: OS: start, MUX: AINP=AIN0 / AINN=GND, PGA = ±6.144V, MODE = Continuous
    	//config_data[2] = 0xE3;  // LSB: DR = 3300 SPS, COMP_MODE = traditional, COMP_POL = active low, COMP_LAT = non-latching, COMP_QUE = disable

    	//if (i2cWriteDevice(handle, config_data, 3) != 0) {
        //	pr_info("Failed to write configuration\n");
        //	return;
   	//}

	pr_info("initializing sample timer\n");
	hrtimer_init(&ads1015_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ads1015_timer.function = &timer_event;

	interval = ktime_set(2, 0);
	printk("interval = %lld\n", interval);
	//hrtimer_start(&ads1015_timer, interval, HRTIMER_MODE_REL);
	//pr_info("ads1015_samplier_init() complete.\n");
}

void ads1015_sampler_exit(void) {
	hrtimer_cancel(&ads1015_timer);
	pr_info("stopped ads1015_timer\n");
    	
	// Configure ADS1015 for single-ended input on AIN0 with default settings
    	//char config_data[3];
    	//config_data[0] = CONFIG_REG;
    	//config_data[1] = 0xC1;  // MSB: OS: start, MUX: AINP=AIN0 / AINN=GND, PGA = ±6.144V, MODE = Single Shot
    	//config_data[2] = 0xE3;  // LSB: DR = 3300 SPS, COMP_MODE = traditional, COMP_POL = active low, COMP_LAT = non-latching, COMP_QUE = disable

    	//if (i2cWriteDevice(handle, config_data, 3) != 0) {
        //	pr_info("Failed to write configuration\n");
   	//}
	pr_info("powered down ADS1015\n");

	//i2cClose(i2c_handle);
	i2c_unregister_device(i2c_client);
	pr_info("disconnected from i2c bus\n");
	//gpioTerminate();
	//pr_info("released GPIO access\n");
}

