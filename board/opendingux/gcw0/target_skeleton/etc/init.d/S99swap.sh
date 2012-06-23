#!/bin/sh

if [ -z "$1" ] || [ "x$1" = "xstart" ]; then

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
    size=$(( $SWAP_SD_SIZE_MB * 1024 * 1024 ))
    if [ $size -ne 0 ]; then
        /bin/dd if=/dev/zero of=$SWAP_SD_FILE bs=1 count=0 seek=$size
        /sbin/mkswap $SWAP_SD_FILE
        /sbin/swapon -p$SWAP_SD_PRIORITY $SWAP_SD_FILE
    fi
fi

