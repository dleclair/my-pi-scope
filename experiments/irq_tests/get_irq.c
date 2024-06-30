#include <linux/interrupts.h>

int main(int argc, char**argv) {
	printf("getting IRQ # for GPIO #: %d", argv[1]);
	return 0;
}

