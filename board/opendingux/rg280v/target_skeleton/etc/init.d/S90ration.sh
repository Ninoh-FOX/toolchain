#!/bin/sh
#
# Simple script to load/store screen ration mode
#

STATEFILE=/usr/local/etc/ration.state
SYSFSFILE=/sys/devices/platform/jz-lcd.0/keep_aspect_ratio

case "$1" in
	start)
		echo "Loading ration mode setting..."
		if [ -f $STATEFILE ]; then
			cp $STATEFILE $SYSFSFILE
		fi
		;;
	stop)
		echo "Storing ration mode setting..."
		cp $SYSFSFILE $STATEFILE
		;;
	*)
		echo "Usage: $0 {start|stop}"
		exit 1
esac

exit $?
