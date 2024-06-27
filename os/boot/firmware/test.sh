#$!/bin/bash

echo 0x$(xxd /sys/class/i2c-adapter/i2c-1/of_node/clock-frequency | cut -f 2,3 -d ' ' --output-delimiter='') | xargs printf "%d\n"

i2cdetect -y 1
for ((i = 0; i < 10; i++)); do cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw; done;

