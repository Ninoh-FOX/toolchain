#!/bin/bash
set -e
umask 0022

VERSION="`cat ../version.txt`"

if [ -r "../output/images/vmlinuz.bin" ]; then
KERNEL="../output/images/vmlinuz.bin"
else
KERNEL=""
fi

if [ -r "../output/images/modules.squashfs" ]; then
MODULES_FS="../output/images/modules.squashfs"
else
MODULE_FS=""
fi

if [ -r "../output/images/rootfs.squashfs" ]; then
ROOTFS="../output/images/rootfs.squashfs"
else
ROOTFS=""
fi

if [ -r "update_rg300x/mininit-syspart" ]; then
MININIT="update_rg300x/mininit-syspart"
else
MININIT=""
fi

if [ -r "../output/images/ubiboot/ubiboot-rg350.bin" ]; then
BOOTLOADERS="../output/images/ubiboot/ubiboot-rg350.bin"
else
BOOTLOADERS=""
fi

# TODO: Reinstate this:
# X-OD-Manual=CHANGELOG

# Copy kernel and rootfs to update_m dir.
# We want to support symlinks for the kernel and rootfs images and if no
# copy is made, specifying the symlink will include the symlink in the OPK
# and specifying the real path might use a different name than the update_m
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
	sha1sum "$KERNEL" | cut -d' ' -f1 > "../output/images/vmlinuz.bin.sha1"
	echo "done"

	echo -n "Calculating SHA1 sum of modules file-system... "
	sha1sum "$MODULES_FS" | cut -d' ' -f1 > "../output/images/modules.squashfs.sha1"
	echo "done"

	KERNEL="$KERNEL ../output/images/vmlinuz.bin.sha1"
        MODULES_FS="$MODULES_FS ../output/images/modules.squashfs.sha1"
fi

if [ "$ROOTFS" ] ; then

	echo -n "Calculating SHA1 sum of rootfs... "
	sha1sum "$ROOTFS" | cut -d' ' -f1 > "../output/images/rootfs.squashfs.sha1"
	echo "done"

	ROOTFS="$ROOTFS ../output/images/rootfs.squashfs.sha1"
fi

if [ "$BOOTLOADERS" ] ; then

	echo -n "Calculating SHA1 sum of bootloaders... "
        sha1sum "$BOOTLOADERS" | cut -d' ' -f1 > "../output/images/ubiboot/ubiboot-rg350.bin.sha1"
        echo "done"

        BOOTLOADERS="$BOOTLOADERS ../output/images/ubiboot/ubiboot-rg350.bin.sha1"
fi

if [ "$MININIT" ] ; then

	echo -n "Calculating SHA1 sum of mininit-syspart... "
	sha1sum "$MININIT" | cut -d' ' -f1 > "update_rg300x/mininit-syspart.sha1"
	echo "done"

	MININIT="$MININIT update_rg300x/mininit-syspart.sha1"
fi

echo "$DATE" > update_rg300x/date.txt

echo "$VERSION" > update_rg300x/version.txt

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
cat > update_rg300x/default.gcw0.desktop <<EOF
[Desktop Entry]
Name=OS update $VERSION
Comment=RG300X ROGUE update $DATE
Exec=update.sh
Icon=rogue
params=480 640
Terminal=true
Type=Application
StartupNotify=true
Categories=applications;
EOF

# Create OPK.
OPK_FILE=update_rg300x/rg300x-update-$VERSION-$DATE.opk
mksquashfs \
	update_rg300x/default.gcw0.desktop \
	update_rg300x/rogue.png \
	update_rg300x/update.sh \
	update_rg300x/trimfat.py \
	update_rg300x/flash_partition.sh \
	update_rg300x/date.txt \
    update_rg300x/version.txt \
    update_rg300x/fsck.fat \
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

mv $OPK_FILE ../output/
echo
echo "=========================="
echo
echo "moved OPK to output folder"
echo

rm update_rg300x/default.gcw0.desktop update_rg300x/date.txt update_rg300x/version.txt update_rg300x/*.sha1
