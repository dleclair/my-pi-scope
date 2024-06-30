#!/bin/bash


pinctrl set 21 op

led_state="dh"
while true;
do
	if [ $led_state = "dh" ]
	then
		led_state="dl"
		echo "LED OFF"
	else
		led_state="dh"
		echo "LED ON"

	fi
	pinctrl set 21 $led_state
	sleep 1
done;
