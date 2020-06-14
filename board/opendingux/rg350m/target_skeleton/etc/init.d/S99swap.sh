#!/bin/sh

# Defaults.
. /etc/swap.conf

# User overrides.
if [ -f /usr/local/etc/swap.conf ]; then
	. /usr/local/etc/swap.conf
fi

# Swap to compressed RAM.
size=$(( $SWAP_ZRAM_SIZE_MB * 1024 * 1024 ))
if [ $size -ne 0 ]; then
	if [ -b $SWAP_ZRAM_FILE ]; then
		device=`/usr/bin/basename $SWAP_ZRAM_FILE`
		echo $size > /sys/devices/virtual/block/$device/disksize
		/sbin/mkswap $SWAP_ZRAM_FILE
		/sbin/swapon -p$SWAP_ZRAM_PRIORITY $SWAP_ZRAM_FILE
	fi
fi

# Swap to SD card.
size=$(( $SWAP_SD_SIZE_MB * 1024 ))
if [ $size -ne 0 ]; then
	if [[ -r $SWAP_SD_FILE && -f /media/data/.swapon ]]; then
		/sbin/swapon -p$SWAP_SD_PRIORITY $SWAP_SD_FILE
	else
		echo -n 1 >/sys/devices/virtual/vtconsole/vtcon1/bind
		clear
		echo
		echo "Make SWAP file for first time, please wait..."
		echo
		dd if=/dev/zero bs=1024 | pv -s 498M | dd of=$SWAP_SD_FILE bs=1024 count=$size conv=notrunc,noerror 1>/dev/null 2>&1
		echo "done"
		sleep 4
		echo -n 0 >/sys/devices/virtual/vtconsole/vtcon1/bind
		sync
		chown root:root $SWAP_SD_FILE
		chmod 0600 $SWAP_SD_FILE
		/sbin/mkswap $SWAP_SD_FILE
		/sbin/swapon -p$SWAP_SD_PRIORITY $SWAP_SD_FILE
		touch /media/data/.swapon
		sleep 2
	fi
fi
