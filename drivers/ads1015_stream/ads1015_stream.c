
#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("DDL");
MODULE_DESCRIPTION("streaming ADS1015 samples via I2C");

/* ADS1015 definitions */
#define CONFIG_REG 0x01
#define CONV_REG 0x00
#define FSR_LSB 3000


/* I2C parameters */
#define I2C_BUS 1
#define ADS1015_DEVICE_NAME "ADS1015-stream"
#define ADS1015_ADDR 0x48


static struct i2c_adapter *i2c_adapter = NULL;
static struct i2c_client *i2c_client = NULL;

static struct i2c_board_info ads1015_board_info = {
	I2C_BOARD_INFO(ADS1015_DEVICE_NAME, ADS1015_ADDR)
};


/* device IDs */
static const struct i2c_device_id ads1015_device_ids[] = {
	{ ADS1015_DEVICE_NAME, 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, ads1015_device_ids);


/* I2C driver structure */
/* handler prototypes */
static int ads1015_stream_probe(struct i2c_client *client);
static void ads1015_stream_remove(struct i2c_client *client);

static struct i2c_driver ads1015_driver = {
	.driver = {
		.name = ADS1015_DEVICE_NAME,
		.owner = THIS_MODULE,
	},
	.probe = ads1015_stream_probe,
	.remove = ads1015_stream_remove,
	.id_table = ads1015_device_ids
};


/* ADS1015 sample scheduler */
static ktime_t ads1015_timer_interval;
static struct hrtimer ads1015_timer;

long read_ads1015_sample(void) {
    // Read the conversion result
    int raw_adc = i2c_smbus_read_word_data(i2c_client, CONV_REG);
    long sample_voltage = raw_adc * FSR_LSB;

    // Return the sample voltage
    return sample_voltage;
}


enum hrtimer_restart ads1015_timer_event(struct hrtimer *t) {
	printk("in timer_event()\n");

	u64 t1, t2, elapsed_time;
	t1 = ktime_get_ns();
	long sample_voltage = 0;
	sample_voltage = read_ads1015_sample();
	t2 = ktime_get_ns();
	elapsed_time = t2 - t1;
	pr_info("sample: %ld uV, elapsed_time = %llu usecs\n", sample_voltage, elapsed_time);
	long skips = hrtimer_forward_now(t, ads1015_timer_interval);
	if (skips > 1) {
		printk("skipped %ld intervals.", skips);
	}
	return HRTIMER_RESTART;
}


static int __init ads1015_stream_init(void) {
	pr_info("attaching to I2C bus\n");
	i2c_adapter = i2c_get_adapter(I2C_BUS);
	pr_info("creating ADS1015 client\n");
	i2c_client = i2c_new_client_device(i2c_adapter, &ads1015_board_info);
	pr_info("registering driver devices\n");
	i2c_add_driver(&ads1015_driver);

	pr_info("testing device by reading config register\n");
	s32 cfgreg = i2c_smbus_read_word_data(i2c_client, CONFIG_REG);
	pr_info("result: 0x%4x\n", cfgreg);

	// Configure ADS1015 for single-ended input on AIN0 with default setting
	// MSB: OS: start, MUX: AINP=AIN0 / AINN=GND, PGA = ±6.144V, MODE = Continuous
	// LSB: DR = 3300 SPS, COMP_MODE = traditional, COMP_POL = active low, COMP_LAT = non-latching, COMP_QUE = disable
	u16 config_data = 0xC0E3;
	int result = i2c_smbus_write_word_data(i2c_client, CONFIG_REG, config_data);
	pr_info("config write returned %d.\n", result);

	/* set up timer */
	pr_info("initializing sample timer\n");
	ads1015_timer_interval = ktime_set(2, 0);
	hrtimer_init(&ads1015_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ads1015_timer.function = &ads1015_timer_event;
	hrtimer_start(&ads1015_timer, ads1015_timer_interval, HRTIMER_MODE_REL);

	pr_info("initialization complete.\n");
	return 0;
}


static int ads1015_stream_probe(struct i2c_client *client) {
	pr_info("device probed\n");
	return 0;
}


static void ads1015_stream_remove(struct i2c_client *client) {
	pr_info("stopping sampling timer.\n");
	hrtimer_cancel(&ads1015_timer);
    	pr_info("stopping ADS1015 auto-sampling.\n");
	// MSB: OS: start, MUX: AINP=AIN0 / AINN=GND, PGA = ±6.144V, MODE = Single Shot
    	// LSB: DR = 3300 SPS, COMP_MODE = traditional, COMP_POL = active low, COMP_LAT = non-latching, COMP_QUE = disable
	u16 config_data = 0xC1E3;
	int result = i2c_smbus_write_word_data(i2c_client, CONFIG_REG, config_data);
	pr_info("config write returned %d.\n", result);
	pr_info("device removed\n");
	return;
}


static void __exit ads1015_stream_exit(void) {
	pr_info("unregistering i2c client device\n");
	i2c_unregister_device(i2c_client);
	pr_info("deleting driver\n");
	i2c_del_driver(&ads1015_driver);
	pr_info("exiting...\n");
}


//static int ads1015_stream_write(unsigned char *buf, unsigned int len) {
//	pr_info("write request\n");
//	return 0;
//}


//static int ads1015_stream_read(unsigned char *buf, unsigned int len) {
//	pr_info("read request\n");
//	return 0;
//}


module_init(ads1015_stream_init);
module_exit(ads1015_stream_exit);


