#$!/bin/bash
i2cdetect -y 1
for ((i = 0; i < 10; i++)); do cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw; done;

