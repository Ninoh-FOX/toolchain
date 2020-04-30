#!/bin/sh
#
# Simple script to load/store ALSA volume.
#

VOLUME_STATEFILE=/usr/local/etc/volume.state
VOLUME_DEVICE=default
VOLUME_CONTROL=PCM

case "$1" in
	start)
		if [ -f $VOLUME_STATEFILE ]; then
			echo "Loading sound volume..."
			/usr/bin/alsa-setvolume $VOLUME_DEVICE $VOLUME_CONTROL `cat $VOLUME_STATEFILE`
		fi
		;;
	stop)
		echo "Storing sound volume..."
		/usr/bin/alsa-getvolume $VOLUME_DEVICE $VOLUME_CONTROL > $VOLUME_STATEFILE
		;;
	*)
		echo "Usage: $0 {start|stop}"
		exit 1
esac

exit $?
