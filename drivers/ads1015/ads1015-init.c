#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timekeeping.h>
#include <linux/proc_fs.h>

#include "ads1015-io.h"
#include "ads1015-sampler.h"


static struct proc_dir_entry *ads1015_proc = NULL;

static const struct proc_ops ads1015_proc_fops = {
	.proc_read = ads1015_read,
	.proc_write = ads1015_write,
};


static int __init ads1015_init(void)
{
	printk("Starting ADS1015 driver\n");

	u32 clockres = 0;
	clockres = ktime_get_resolution_ns();
	printk("clock resolution: %d\n", clockres);

	//create an entry in the procfs
	ads1015_proc = proc_create("ads1015", 0666, NULL, &ads1015_proc_fops);
	if (ads1015_proc == NULL)
	{
		return -1;
	}

	ads1015_sampler_init();
	return 0;
}

static void __exit ads1015_exit(void)
{
	printk("Exiting ADS1015 driver.\n");
	proc_remove(ads1015_proc);
	return;
}

module_init(ads1015_init);
module_exit(ads1015_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DDL");
MODULE_DESCRIPTION("Driver for the ADS1015 daughtercard by AdaFruit\n");
MODULE_VERSION("1.0");


