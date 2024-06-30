
obj-m += my-ads1015.o

all: ads1015_stream.o ads1015_overlay.dtbo

ads1015_stream.o:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

ads1015_overlay.dtbo:
	dtc -@ -I dts -O dtb -o my-ads1015_overlay.dtbo my-ads1015_overlay.dts

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm *.dtbo

