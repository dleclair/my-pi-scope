KDIR = /lib/modules/$(shell uname -r)/build

obj-m := ads1015.o

ads1015-objs := ads1015-init.o ads1015-io.o

all:
	make -C $(KDIR) M=$(shell pwd) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean


