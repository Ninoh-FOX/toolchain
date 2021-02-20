#!/bin/sh

if [ -z "$1" ] || [ "x$1" = "xstart" ]; then

	mount --bind /usr/share/ftphome/media /usr/share/ftphome/media
	mount --make-shared /media
	mount --make-slave /usr/share/ftphome/media
	mount --bind /media /usr/share/ftphome/media
	mount -o remount,bind,ro /media /usr/share/ftphome/media

fi
