#!/bin/bash

echo "Device name: " `cat /sys/bus/iio/devices/iio:device0/name`
cat /sys/bus/iio/devices/iio:device0/uevent
echo "Current timestamp clock: " `cat /sys/bus/iio/devices/iio:device0/current_timestamp_clock`
echo "voltage0 scale: " `cat /sys/bus/iio/devices/iio:device0/in_voltage0_scale`
echo "scale options: " `cat /sys/bus/iio/devices/iio:device0/scale_available`
echo "voltage0 sampling frequency: " `cat /sys/bus/iio/devices/iio:device0/in_voltage0_sampling_frequency`
echo "sampling frequency options: " `cat /sys/bus/iio/devices/iio:device0/sampling_frequency_available`
echo "current trigger: " `cat /sys/bus/iio/devices/iio:device0/trigger/current_trigger`
echo "voltage0 either en: " `cat /sys/bus/iio/devices/iio:device0/events/in_voltage0_thresh_either_en`
echo "voltage0 either period: " `cat /sys/bus/iio/devices/iio:device0/events/in_voltage0_thresh_either_period`
echo "voltage0 falling value: " `cat /sys/bus/iio/devices/iio:device0/events/in_voltage0_thresh_falling_value`
echo "voltage0 rising en: " `cat /sys/bus/iio/devices/iio:device0/events/in_voltage0_thresh_rising_en`
echo "voltage0 rising value: " `cat /sys/bus/iio/devices/iio:device0/events/in_voltage0_thresh_rising_value`

