#!/bin/bash
set -e
umask 0022

VERSION="`cat ./version.txt`"

if [ -r "updateplay/vmlinuz.bin" ]; then
KERNEL="updateplay/vmlinuz.bin"
else
KERNEL=""
fi

if [ -r "updateplay/modules.squashfs" ]; then
MODULES_FS="updateplay/modules.squashfs"
else
MODULE_FS=""
fi

if [ -r "updateplay/rootfs.squashfs" ]; then
ROOTFS="updateplay/rootfs.squashfs"
else
ROOTFS=""
fi

if [ -r "updateplay/mininit-syspart" ]; then
MININIT="updateplay/mininit-syspart"
else
MININIT=""
fi

if [ -r "updateplay/ubiboot-v20_mddr_512mb.bin" ]; then
BOOTLOADERS="updateplay/ubiboot-v20_mddr_512mb.bin"
else
BOOTLOADERS=""
fi

# TODO: Reinstate this:
# X-OD-Manual=CHANGELOG

# Copy kernel and rootfs to updateplay dir.
# We want to support symlinks for the kernel and rootfs images and if no
# copy is made, specifying the symlink will include the symlink in the OPK
# and specifying the real path might use a different name than the updateplay
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
	sha1sum "$KERNEL" | cut -d' ' -f1 > "updateplay/vmlinuz.bin.sha1"
	echo "done"

	echo -n "Calculating SHA1 sum of modules file-system... "
	sha1sum "$MODULES_FS" | cut -d' ' -f1 > "updateplay/modules.squashfs.sha1"
	echo "done"

	KERNEL="$KERNEL updateplay/vmlinuz.bin.sha1"
        MODULES_FS="$MODULES_FS updateplay/modules.squashfs.sha1"
fi

if [ "$ROOTFS" ] ; then

	echo -n "Calculating SHA1 sum of rootfs... "
	sha1sum "$ROOTFS" | cut -d' ' -f1 > "updateplay/rootfs.squashfs.sha1"
	echo "done"

	ROOTFS="$ROOTFS updateplay/rootfs.squashfs.sha1"
fi

if [ "$BOOTLOADERS" ] ; then

	echo -n "Calculating SHA1 sum of bootloaders... "
        sha1sum "$BOOTLOADERS" | cut -d' ' -f1 > "updateplay/ubiboot-v20_mddr_512mb.bin.sha1"
        echo "done"

        BOOTLOADERS="$BOOTLOADERS updateplay/ubiboot-v20_mddr_512mb.bin.sha1"
fi

if [ "$MININIT" ] ; then

	echo -n "Calculating SHA1 sum of mininit-syspart... "
	sha1sum "$MININIT" | cut -d' ' -f1 > "updateplay/mininit-syspart.sha1"
	echo "done"

	MININIT="$MININIT updateplay/mininit-syspart.sha1"
fi

echo "$DATE" > updateplay/date.txt

echo "$VERSION" > updateplay/version.txt

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
cat > updateplay/default.gcw0.desktop <<EOF
[Desktop Entry]
Name=OS update $VERSION
Comment=PlayGo ROGUE update $DATE
Exec=update.sh
Icon=rogue
Terminal=true
Type=Application
StartupNotify=true
Categories=applications;
EOF

# Create OPK.
OPK_FILE=updateplay/PlayGo-update-$VERSION-$DATE.opk
mksquashfs \
	updateplay/default.gcw0.desktop \
	updateplay/rogue.png \
	updateplay/update.sh \
	updateplay/trimfat.py \
	updateplay/flash_partition.sh \
	updateplay/date.txt \
    updateplay/version.txt \
    updateplay/fsck.fat \
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
