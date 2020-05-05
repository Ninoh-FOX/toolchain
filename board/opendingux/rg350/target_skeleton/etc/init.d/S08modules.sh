#!/bin/sh

case "$1" in
    start)
        echo 'Mounting modules filesystem...'

		MODULES_FILESYSTEM=/boot/modules.squashfs
		if [ "`grep 'kernel_bak' /proc/cmdline`" ] ; then
			MODULES_FILESYSTEM=/boot/modules.squashfs.bak
		fi

		if [ "`grep '/lib/modules' /proc/mounts`" ] ; then
			echo 'Modules filesystem is already mounted' >&2
			exit 1
		fi

		mount -o loop "$MODULES_FILESYSTEM" /lib/modules
        ;;

    stop)
        echo 'Unmounting modules filesystem...'
		umount /lib/modules
        ;;

	*)
		echo "Usage: $0 {start|stop}"
		exit 1
esac
