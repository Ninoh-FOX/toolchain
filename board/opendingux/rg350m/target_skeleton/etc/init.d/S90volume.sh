#!/bin/sh
#
# Simple script to load/store ALSA volume.
#

VOLUME_STATEFILE1=/usr/local/etc/volume1.state
VOLUME_DEVICE1=default
VOLUME_CONTROL1=PCM
VOLUME_STATEFILE2=/usr/local/etc/volume2.state
VOLUME_DEVICE2=default
VOLUME_CONTROL2=Headphone
VOLUME_STATEFILE3=/usr/local/etc/volume3.state
VOLUME_DEVICE3=default
VOLUME_CONTROL3=Master

case "$1" in
	start)
		if [ -f $VOLUME_STATEFILE1 ]; then
			echo "Loading sound volume..."
			/usr/bin/alsa-setvolume $VOLUME_DEVICE1 $VOLUME_CONTROL1 `cat $VOLUME_STATEFILE1`
			else
			/usr/bin/alsa-setvolume $VOLUME_DEVICE1 $VOLUME_CONTROL1 31
		fi
		if [ -f $VOLUME_STATEFILE2 ]; then
			echo "Loading sound volume..."
			/usr/bin/alsa-setvolume $VOLUME_DEVICE2 $VOLUME_CONTROL2 `cat $VOLUME_STATEFILE2`
			else
			/usr/bin/alsa-setvolume $VOLUME_DEVICE2 $VOLUME_CONTROL2 25
		fi
		if [ -f $VOLUME_STATEFILE3 ]; then
			echo "Loading sound volume..."
			/usr/bin/alsa-setvolume $VOLUME_DEVICE3 $VOLUME_CONTROL3 `cat $VOLUME_STATEFILE3`
			else
			/usr/bin/alsa-setvolume $VOLUME_DEVICE2 $VOLUME_CONTROL2 255
		fi
		;;
	stop)
		echo "Storing sound volume..."
		/usr/bin/alsa-getvolume $VOLUME_DEVICE1 $VOLUME_CONTROL1 > $VOLUME_STATEFILE1
		/usr/bin/alsa-getvolume $VOLUME_DEVICE2 $VOLUME_CONTROL2 > $VOLUME_STATEFILE2
		/usr/bin/alsa-getvolume $VOLUME_DEVICE3 $VOLUME_CONTROL3 > $VOLUME_STATEFILE3
		;;
	*)
		echo "Usage: $0 {start|stop}"
		exit 1
esac

exit $?