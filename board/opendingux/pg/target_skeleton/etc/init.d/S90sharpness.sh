#!/bin/sh
#
# Simple script to load/store screen sharpness
#

STATEFILE=/usr/local/etc/sharpnessup.state
SYSFSFILE=/sys/devices/platform/jz-lcd.0/sharpness_upscaling
STATEFILE2=/usr/local/etc/sharpnessdown.state
SYSFSFILE2=/sys/devices/platform/jz-lcd.0/sharpness_downscaling

case "$1" in
	start)
		echo "Loading sharpness setting..."
		if [ -f $STATEFILE ]; then
			cp $STATEFILE $SYSFSFILE
		fi
		if [ -f $STATEFILE2 ]; then
			cp $STATEFILE2 $SYSFSFILE2
		fi
		;;
	stop)
		echo "Storing sharpness setting..."
		cp $SYSFSFILE $STATEFILE
		cp $SYSFSFILE2 $STATEFILE2
		;;
	*)
		echo "Usage: $0 {start|stop}"
		exit 1
esac

exit $?
