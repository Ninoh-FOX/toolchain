#!/bin/bash
set -e
umask 0022

VERSION="`cat ./version.txt`"

if [ -r "updatev2/vmlinuz.bin" ]; then
KERNEL="updatev2/vmlinuz.bin"
else
KERNEL=""
fi

if [ -r "updatev2/modules.squashfs" ]; then
MODULES_FS="updatev2/modules.squashfs"
else
MODULE_FS=""
fi

if [ -r "updatev2/rootfs.squashfs" ]; then
ROOTFS="updatev2/rootfs.squashfs"
else
ROOTFS=""
fi

if [ -r "updatev2/mininit-syspart" ]; then
MININIT="updatev2/mininit-syspart"
else
MININIT=""
fi

if [ -r "updatev2/ubiboot.bin" ]; then
BOOTLOADERS="updatev2/ubiboot.bin"
else
BOOTLOADERS=""
fi

# TODO: Reinstate this:
# X-OD-Manual=CHANGELOG

# Copy kernel and rootfs to update dir.
# We want to support symlinks for the kernel and rootfs images and if no
# copy is made, specifying the symlink will include the symlink in the OPK
# and specifying the real path might use a different name than the updatev2
# script expects.
if [ "$KERNEL" -a "$ROOTFS" ] ; then
	if [ `date -r "$KERNEL" +%s` -gt `date -r "$ROOTFS" +%s` ] ; then
		DATE=`date -r "$KERNEL" +%F`
	else
		DATE=`date -r "$ROOTFS" +%F`
	fi

elif [ "$KERNEL" ] ; then
	DATE=`date -r "$KERNEL" +%F`
elif [ "$ROOTFS" ] ; then
	DATE=`date -r "$ROOTFS" +%F`
else
	echo "ERROR: No kernel or rootfs found."
	exit 1
fi

if [ "$KERNEL" ] ; then

	chmod a-x "$KERNEL" "$MODULES_FS"

	echo -n "Calculating SHA1 sum of kernel... "
	sha1sum "$KERNEL" | cut -d' ' -f1 > "updatev2/vmlinuz.bin.sha1"
	echo "done"

	echo -n "Calculating SHA1 sum of modules file-system... "
	sha1sum "$MODULES_FS" | cut -d' ' -f1 > "updatev2/modules.squashfs.sha1"
	echo "done"

	KERNEL="$KERNEL updatev2/vmlinuz.bin.sha1"
        MODULES_FS="$MODULES_FS updatev2/modules.squashfs.sha1"
fi

if [ "$ROOTFS" ] ; then

	echo -n "Calculating SHA1 sum of rootfs... "
	sha1sum "$ROOTFS" | cut -d' ' -f1 > "updatev2/rootfs.squashfs.sha1"
	echo "done"

	ROOTFS="$ROOTFS updatev2/rootfs.squashfs.sha1"
fi

if [ "$BOOTLOADERS" ] ; then

	echo -n "Calculating SHA1 sum of bootloaders... "
        sha1sum "$BOOTLOADERS" | cut -d' ' -f1 > "updatev2/ubiboot.bin.sha1"
        echo "done"

        BOOTLOADERS="$BOOTLOADERS updatev2/ubiboot.bin.sha1"
fi

if [ "$MININIT" ] ; then

	echo -n "Calculating SHA1 sum of mininit-syspart... "
	sha1sum "$MININIT" | cut -d' ' -f1 > "updatev2/mininit-syspart.sha1"
	echo "done"

	MININIT="$MININIT updatev2/mininit-syspart.sha1"
fi

echo "$DATE" > updatev2/date.txt

echo "$VERSION" > updatev2/version.txt

# Report metadata.
echo
echo "=========================="
echo "Bootloaders:          $BOOTLOADERS"
echo "Mininit:              $MININIT"
echo "Kernel:               $KERNEL"
echo "Modules file system:  $MODULES_FS"
echo "Root file system:     $ROOTFS"
echo "build date:           $DATE"
echo "build version:        $VERSION"
echo "=========================="
echo

# Write metadata.
cat > updatev2/default.gcw0.desktop <<EOF
[Desktop Entry]
Name=OS update $VERSION
Comment=POCKETGO2 v2 ROGUE update $DATE
Exec=update.sh
Icon=rogue
Terminal=true
Type=Application
StartupNotify=true
Categories=applications;
EOF

# Create OPK.
OPK_FILE=updatev2/pocketgo2v2-update-$VERSION-$DATE.opk
mksquashfs \
	updatev2/default.gcw0.desktop \
	updatev2/rogue.png \
	updatev2/update.sh \
	updatev2/trimfat.py \
	updatev2/flash_partition.sh \
	updatev2/date.txt \
    updatev2/version.txt \
    updatev2/fsck.fat \
	$BOOTLOADERS \
	$MININIT \
	$KERNEL \
	$MODULES_FS \
    $ROOTFS \
	$OPK_FILE \
	-no-progress -noappend -comp gzip -all-root

echo
echo "=========================="
echo
echo "updater OPK:       $OPK_FILE"
echo
