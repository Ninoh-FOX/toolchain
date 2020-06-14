#!/bin/sh

if [ -z "$1" ] || [ "x$1" = "xstart" ]; then 

	if [ -f /media/data/.reconfig ] && [ -f /media/data/.finish ]; then
		exit 1
	fi
	
	if [ ! -r /media/data/.reconfig ] && [ -f /media/data/.finish ]; then
		rm -rf /media/data/local/etc/*
		echo -n 1 >/sys/devices/virtual/vtconsole/vtcon1/bind
		clear
		echo "wipe old config from file system"
		sleep 2
		echo "done"
		sleep 2
		echo "The system reboot in 10s..."
		sleep 10
		echo -n 0 >/sys/devices/virtual/vtconsole/vtcon1/bind
		touch /media/data/.reconfig
		reboot
	fi
	
	if [ -f /media/data/.reconfig ] && [ ! -r /media/data/.finish ]; then
	    echo -n 1 >/sys/devices/virtual/vtconsole/vtcon1/bind
		clear
		echo "Finishing installation."
		echo "This will prevent data corruption in case the console"
		echo "did not shutdown properly on the first system boot."
		sleep 4
		echo "done"
		sleep 2
		echo "The system reboot in 10s..."
		sleep 10
		echo -n 0 >/sys/devices/virtual/vtconsole/vtcon1/bind
		touch /media/data/.finish
		reboot
	fi
	
	if [ ! -r /media/data/.reconfig ] && [ ! -r /media/data/.finish ]; then
		rm -rf /media/data/local/etc/*
		echo -n 1 >/sys/devices/virtual/vtconsole/vtcon1/bind
		clear
		echo "wipe old config from file system"
		sleep 2
		echo "done"
		sleep 2
		echo "The system reboot in 10s..."
		sleep 10
		echo -n 0 >/sys/devices/virtual/vtconsole/vtcon1/bind
		touch /media/data/.reconfig
		reboot
	fi
fi