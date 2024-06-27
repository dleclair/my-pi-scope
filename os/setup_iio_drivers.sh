#!/bin/bash

sudo modprobe ti-ads1015
sudo modprobe industrialio-configfs
sudo modprobe industrialio-sw-trigger
sudo modprobe iio-trig-sysfs
sudo modprobe iio-trig-hrtimer
# modproble iio-trig-interrupt

sudo sh -c "echo 0 > /sys/bus/iio/devices/iio_sysfs_trigger/add_trigger"
cat /sys/bus/iio/devices/iio_sysfs_trigger/trigger0/name

sudo mkdir -p /config
sudo mount -t configfs none /config
sudo systemctl daemon-reload
sudo mkdir /config/iio/triggers/hrtimer/my_hrtimer_trigger
cat /sys/bus/iio/devices/trigger10/name
sudo sh -c "echo 3000 > /sys/bus/iio/devices/trigger1/sampling_frequency"
sudo sh -c "cat /sys/bus/iio/devices/trigger1/name > /sys/bus/iio/devices/iio\:device0/trigger/current_trigger"

