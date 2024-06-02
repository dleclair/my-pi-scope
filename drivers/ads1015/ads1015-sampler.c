
#include <linux/hrtimer.h>


static ktime_t interval;
static struct hrtimer ads1015_timer;


enum hrtimer_restart timer_event(struct hrtimer *t) {
	printk("in timer_event()\n");

	long skips = hrtimer_forward_now(t, interval);
	if (skips > 0) {
		printk("skipped %ld intervals.", skips);
	}
	return HRTIMER_RESTART;
}


void ads1015_sampler_init(void) {
	printk("in ads1015_sampler_init()\n");
	hrtimer_init(&ads1015_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ads1015_timer.function = &timer_event;

	interval = ktime_set(2, 0);
	printk("interval = %lld\n", interval);
	hrtimer_start(&ads1015_timer, interval, HRTIMER_MODE_REL);
	pr_info("ads1015_samplier_init() complete.\n");
}


