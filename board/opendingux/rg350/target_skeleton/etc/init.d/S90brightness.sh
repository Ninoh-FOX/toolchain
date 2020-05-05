#!/bin/sh
#
# Simple script to load/store screen brightness
#

STATEFILE=/usr/local/etc/brightness.state
SYSFSFILE=/sys/class/backlight/pwm-backlight/brightness

case "$1" in
	start)
		echo "Loading brightness setting..."
		if [ -f $STATEFILE ]; then
			cp $STATEFILE $SYSFSFILE
		fi
		;;
	stop)
		echo "Storing brightness setting..."
		cp $SYSFSFILE $STATEFILE
		;;
	*)
		echo "Usage: $0 {start|stop}"
		exit 1
esac

exit $?
