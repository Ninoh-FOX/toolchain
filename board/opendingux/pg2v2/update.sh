#!/bin/sh
set -x

cd `dirname $0`

export KERNEL=./vmlinuz.bin
export MODULES_FS=./modules.squashfs
export ROOTFS=./rootfs.squashfs
DATE_FILE=./date.txt

export SYSTEM_DEVICE=/dev/mmcblk0
export SYSTEM_PART_NUM=1
export SYSTEM_PARTITION=${SYSTEM_DEVICE}p${SYSTEM_PART_NUM}
export SYSTEM_MOUNTPOINT=/mnt/_system_update
KERNEL_TMP_DEST=$SYSTEM_MOUNTPOINT/update_kernel.bin
KERNEL_DEST=$SYSTEM_MOUNTPOINT/vmlinuz.bin
KERNEL_BACKUP=$SYSTEM_MOUNTPOINT/vmlinuz.bak

ROOTFS_TMP_DEST=$SYSTEM_MOUNTPOINT/update_rootfs.bin
ROOTFS_CURRENT=$SYSTEM_MOUNTPOINT/rootfs.squashfs

MODULES_FS_TMP_DEST=$SYSTEM_MOUNTPOINT/update_modules.bin
MODULES_FS_DEST=$SYSTEM_MOUNTPOINT/update_m.bin
MODULES_FS_CURRENT=$SYSTEM_MOUNTPOINT/modules.squashfs

if [ `cat /proc/cmdline |grep rootfs_bak` ] ; then
	# If we're running the backup rootfs, we can overwrite the
	# regular rootfs without problem
	ROOTFS_DEST=$ROOTFS_CURRENT
else
	# Otherwise, the regular rootfs is currently mounted, so we
	# cannot overwrite it; we let min-init (in initramfs) do the switch
	ROOTFS_DEST=$SYSTEM_MOUNTPOINT/update_r.bin
fi

error_quit() {
	rm -f "$KERNEL_TMP_DEST" \
		"$ROOTFS_TMP_DEST" "$ROOTFS_DEST" "${ROOTFS_DEST}.sha1" \
		"$MODULES_FS_TMP_DEST" "$MODULES_FS_DEST" "${MODULES_FS_DEST}.sha1"

	# Forces other mount points to be read-only before unmounting
	mount -o remount,ro "$SYSTEM_MOUNTPOINT"
	umount "$SYSTEM_MOUNTPOINT"
	exit 1
}

DISCLAIMER="\Zb\Z3NOTICE\Zn

While we carefully constructed this updater,
it is possible flaws in the updater or in
the updated OS could lead to \Zb\Z3data loss\Zn. We
recommend that you \Zb\Z3backup\Zn all valuable
personal data on your GCW Zero before you
perform the update.

Do you want to update now?"

UP_TO_DATE=yes
export BAR=`which bar`

if [ -f "$DATE_FILE" ] ; then
	DATE="`cat $DATE_FILE`"
	export DIALOGOPTS="--colors --backtitle \"OpenDingux update $DATE\""
fi

echo "screen_color = (RED,RED,ON)" > /tmp/dialog_err.rc

if [ -f "$KERNEL" -a -f "$KERNEL.sha1" -a -f "$KERNEL_DEST.sha1" ] ; then
	SHA1_OLD=`cat "$KERNEL_DEST.sha1"`
	SHA1_NEW=`cat "$KERNEL.sha1"`
	if [ "$SHA1_OLD" != "$SHA1_NEW" ] ; then
		UP_TO_DATE=no
	fi
fi

if [ -f "$MODULES_FS" -a -f "$MODULES_FS.sha1" -a -f "$MODULES_FS_CURRENT.sha1" ] ; then
	SHA1_OLD=`cat "$MODULES_FS_CURRENT.sha1"`
	SHA1_NEW=`cat "$MODULES_FS.sha1"`
	if [ "$SHA1_OLD" != "$SHA1_NEW" ] ; then
		UP_TO_DATE=no
	fi
fi

if [ -f "$ROOTFS" -a -f "$ROOTFS.sha1" -a "$UP_TO_DATE" = "yes" ] ; then
	SHA1_OLD=`cat "$ROOTFS_CURRENT.sha1"`
	SHA1_NEW=`cat "$ROOTFS.sha1"`
	if [ "$SHA1_OLD" != "$SHA1_NEW" ] ; then
		UP_TO_DATE=no
	fi
fi

if [ "$UP_TO_DATE" = "yes" ] ; then
	dialog --defaultno --yesno 'The system seems to be already up to date.\n\n
Do you really want to continue?' 10 30
	if [ $? -ne 0 ] ; then
		error_quit
	fi
fi

dialog --defaultno --yes-label 'Update' --no-label 'Cancel' --yesno "$DISCLAIMER" 15 48
if [ $? -eq 1 ] ; then
	error_quit
fi

clear
echo 'Update in progress - please be patient.'
echo

HWVARIANT="`sed -n 's/.*hwvariant=\([a-z_0-9]\+\).*/\1/p' /proc/cmdline`"

if [ -z "$HWVARIANT" ] ; then
	# Only old "Frankenzeros" can have a bootloader so old that
	# it doesn't pass the 'hwvariant' parameter to the kernel...
	HWVARIANT="v11_ddr2_256mb"
fi

export BOOTLOADER="./ubiboot-$HWVARIANT.bin"

# The update will fail if /media/system is mounted,
# so we first un-mount it.
# For some reason, future ro mounts will also fail if /media/system was
# mounted rw before it was unmounted.
if [ -d /media/system ] ; then
	mount -o remount,ro /media/system
	umount /media/system
fi

mkdir -p "$SYSTEM_MOUNTPOINT"

# Checks if the system partition has been resized.
if [ -f "$ROOTFS" ] ; then
	SYSTEM_END_THEORY=819199
	SYSTEM_END=$(partx $SYSTEM_DEVICE -n $SYSTEM_PART_NUM -g -o end)
	if [ $SYSTEM_END_THEORY -ne $SYSTEM_END ] ; then
		exec ./flash_partition.sh
	fi
fi

# Linux will refuse to mount read-write if other mount points are read-only,
# so we mount read-only first and remount read-write after
if [ -z "`grep ${SYSTEM_PARTITION}.*rw /proc/mounts`" ] ; then
	mount -o ro "$SYSTEM_PARTITION" "$SYSTEM_MOUNTPOINT"
	mount -o remount,rw "$SYSTEM_MOUNTPOINT"
else
	mount -o rw "$SYSTEM_PARTITION" "$SYSTEM_MOUNTPOINT"
fi

if [ -f "$ROOTFS" ] ; then
	echo 'Installing updated root filesystem... '

	if [ "$BAR" ] ; then
		$BAR -w 54 -0 ' ' -n -o "$ROOTFS_TMP_DEST" "$ROOTFS"
	else
		cp "$ROOTFS" "$ROOTFS_TMP_DEST"
	fi

	if [ $? -ne 0 ] ; then
		DIALOGRC="/tmp/dialog_err.rc" \
			dialog --msgbox 'ERROR!\n\nUnable to update RootFS.\nDo you have enough space available?' 10 34
		error_quit
	fi
fi

if [ -f "$KERNEL" ] ; then
	echo 'Installing updated kernel... '

	if [ "$BAR" ] ; then
		$BAR -w 54 -0 ' ' -n -o "$KERNEL_TMP_DEST" "$KERNEL"
	else
		cp "$KERNEL" "$KERNEL_TMP_DEST"
	fi

	if [ $? -ne 0 ] ; then
		DIALOGRC="/tmp/dialog_err.rc" \
			dialog --msgbox 'ERROR!\n\nUnable to update kernel.' 8 34
		error_quit
	fi

	echo 'Installing updated modules filesystem... '

	if [ "$BAR" ] ; then
		$BAR -w 54 -0 ' ' -n -o "$MODULES_FS_TMP_DEST" "$MODULES_FS"
	else
		cp "$MODULES_FS" "$MODULES_FS_TMP_DEST"
	fi

	if [ $? -ne 0 ] ; then
		DIALOGRC="/tmp/dialog_err.rc" \
			dialog --msgbox 'ERROR!\n\nUnable to update modules filesystem.' 8 34
		error_quit
	fi
fi

echo 'Flushing write cache... '
sync
echo ''

# Make sure that the verification steps use data from disk, not cached data.
echo 3 > /proc/sys/vm/drop_caches

if [ -f "$ROOTFS" ] ; then
	if [ -f "$ROOTFS.sha1" ] ; then
		echo 'Verifying checksum of updated root filesystem...'
		if [ "$BAR" ] ; then
			SHA1=`$BAR -w 54 -0 ' ' -n "$ROOTFS_TMP_DEST" | sha1sum | cut -d' ' -f1`
		else
			SHA1=`sha1sum "$ROOTFS_TMP_DEST" | cut -d' ' -f1`
		fi

		if [ "$SHA1" != "`cat $ROOTFS.sha1`" ] ; then
			DIALOGRC="/tmp/dialog_err.rc" \
				dialog --msgbox 'ERROR!\n\nUpdated RootFS is corrupted!' 9 34
			error_quit
		fi
	fi
fi

if [ -f "$KERNEL" ] ; then
	if [ -f "$KERNEL.sha1" ] ; then
		echo 'Verifying checksum of updated kernel...'
		if [ "$BAR" ] ; then
			SHA1=`$BAR -w 54 -0 ' ' -n "$KERNEL_TMP_DEST" | sha1sum | cut -d' ' -f1`
		else
			SHA1=`sha1sum "$KERNEL_TMP_DEST" | cut -d' ' -f1`
		fi

		if [ "$SHA1" != "`cat $KERNEL.sha1`" ] ; then
			DIALOGRC="/tmp/dialog_err.rc" \
				dialog --msgbox 'ERROR!\n\nUpdated kernel is corrupted!' 9 34
			error_quit
		fi
	fi

	if [ -f "$MODULES_FS.sha1" ] ; then
		echo 'Verifying checksum of updated modules filesystem...'
		if [ "$BAR" ] ; then
			SHA1=`$BAR -w 54 -0 ' ' -n "$MODULES_FS_TMP_DEST" | sha1sum | cut -d' ' -f1`
		else
			SHA1=`sha1sum "$MODULES_FS_TMP_DEST" | cut -d' ' -f1`
		fi

		if [ "$SHA1" != "`cat $MODULES_FS.sha1`" ] ; then
			DIALOGRC="/tmp/dialog_err.rc" \
				dialog --msgbox 'ERROR!\n\nUpdated modules filesystem is corrupted!' 9 34
			error_quit
		fi
	fi
fi

if [ -f "$BOOTLOADER" ] ; then
	if [ -f "$BOOTLOADER.sha1" ] ; then
		echo 'Verifying checksum of updated bootloader...'
		if [ "$BAR" ] ; then
			SHA1=`$BAR -w 54 -0 ' ' -n "$BOOTLOADER" | sha1sum | cut -d' ' -f1`
		else
			SHA1=`sha1sum "$BOOTLOADER" | cut -d' ' -f1`
		fi

		if [ "$SHA1" != "`cat $BOOTLOADER.sha1`" ] ; then
			DIALOGRC="/tmp/dialog_err.rc" \
				dialog --msgbox 'ERROR!\n\nUpdated bootloader is corrupted!' 9 34
			error_quit
		fi
	fi
fi

echo ''
echo 'Committing changes. Please wait...'

if [ -f "$ROOTFS" ] ; then
	# Synchronize the dates
	touch -d "`date -r "$ROOTFS" +'%F %T'`" "$ROOTFS_TMP_DEST"

	if [ -f "$ROOTFS.sha1" ] ; then
		cp "$ROOTFS.sha1" "$ROOTFS_DEST.sha1"
		sync
	fi

	mv "$ROOTFS_TMP_DEST" "$ROOTFS_DEST"
	sync
fi

if [ -f "$KERNEL" ] ; then
	# Synchronize the dates
	touch -d "`date -r "$KERNEL" +'%F %T'`" "$KERNEL_TMP_DEST"

	# Special case when the destination modules filesystem does not exist
	if [ ! -e "$MODULES_FS_CURRENT" ] ; then
		MODULES_FS_DEST="$MODULES_FS_CURRENT"
	fi

	# Don't create a backup if we are already running from the backup kernel,
	# so that no matter what, we'll still have a working kernel installed.
	if [ -z `cat /proc/cmdline |grep kernel_bak` ] ; then
		cp "$KERNEL_DEST" "$KERNEL_BACKUP"
		cp "$KERNEL_DEST.sha1" "$KERNEL_BACKUP.sha1"
		if [ -e "$MODULES_FS_DEST.sha1" ] ; then
			cp "$MODULES_FS_DEST.sha1" "$MODULES_FS_DEST.sha1.bak"
		fi
	else
		MODULES_FS_DEST="$MODULES_FS_CURRENT"
	fi

	if [ -f "$KERNEL.sha1" ] ; then
		cp "$KERNEL.sha1" "$KERNEL_DEST.sha1"
		sync
	fi

	if [ -f "$MODULES_FS.sha1" ] ; then
		cp "$MODULES_FS.sha1" "$MODULES_FS_DEST.sha1"
		sync
	fi

	mv "$MODULES_FS_TMP_DEST" "$MODULES_FS_DEST"
	mv "$KERNEL_TMP_DEST" "$KERNEL_DEST"
	sync
fi

if [ -f "$BOOTLOADER" ] ; then
	dd if="$BOOTLOADER" of="$SYSTEM_DEVICE" bs=512 seek=1 \
		count=16 conv=notrunc 2>/dev/null
	sync
fi

dialog --msgbox 'Update complete!\nThe system will now restart.\n\n
If for some reason the system fails to boot, try to press the
following keys while powering on the device:\n
    -X to boot the last working kernel,\n
    -Y to boot the last working rootfs.\n\n
Note that pressing both keys during the power-on sequence will load the very
same Operating System (kernel + rootfs) you had before upgrading.' 16 0
reboot
