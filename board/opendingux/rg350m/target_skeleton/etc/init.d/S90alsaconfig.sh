#!/bin/sh

case "$1" in
	start)
		echo "alsa-config Starting.."
		/usr/sbin/alsactl restore -f /usr/local/etc/asound.state
		;;
	stop)
		echo "alsa-config Stopping.."
		/usr/sbin/alsactl store -f /usr/local/etc/asound.state
		;;
	*)
		exit 1
		;;

esac

exit 0