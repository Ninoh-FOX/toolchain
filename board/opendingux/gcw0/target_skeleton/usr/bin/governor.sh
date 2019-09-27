#!/bin/bash

STATE=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`
counter=0

while [[ STATE != "performance" ]]; do
	echo 'performance' > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
	#echo "doing some work"
	sleep 2
	STATE=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`
	counter=$((counter+1))
	if [[ $counter -gt 10 ]]; then
		break;
	fi
	echo "counter = $counter"
	echo "state = $STATE"
done

echo "done"
echo "counter = $counter"
echo "state = $STATE"
