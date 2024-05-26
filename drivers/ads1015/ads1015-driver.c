#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timekeeping.h>

#include <linux/proc_fs.h>
#include <linux/slab.h>


static struct proc_dir_entry *ads1015_proc = NULL;

#define SAMPLE_BUF_SIZE 1024
static char sample_buf[SAMPLE_BUF_SIZE];


ssize_t ads1015_read(struct file *file, char __user *user, size_t size, loff_t *off)
{
	unsigned long rc = 0;

	rc = copy_to_user(user, "Hello.\n", 7);
	if (rc > 0) {
		printk("copy_to_user failed to write %ld bytes\n", rc);
	}
	return 7;
}

ssize_t ads1015_write(struct file *file, const char __user *user, size_t size, loff_t *off)
{
	unsigned long rc = 0;
	memset(sample_buf, 0x00, sizeof(sample_buf));
	if (size > SAMPLE_BUF_SIZE) {
		size = SAMPLE_BUF_SIZE;
	}
	rc = copy_from_user(sample_buf, user, size);
	if (rc > 0) {
		printk("copy_from_user failed to read %ld bytes\n", rc);
	}
	printk("you said '%s'\n", sample_buf);
	return size;
}


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


