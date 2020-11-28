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

if [ -r "update_v/mininit-syspart" ]; then
MININIT="update_v/mininit-syspart"
else
MININIT=""
fi

if [ -r "update_v/ubiboot-rg350.bin" ]; then
BOOTLOADERS="update_v/ubiboot-rg350.bin"
else
BOOTLOADERS=""
fi

# TODO: Reinstate this:
# X-OD-Manual=CHANGELOG

# Copy kernel and rootfs to update_v dir.
# We want to support symlinks for the kernel and rootfs images and if no
# copy is made, specifying the symlink will include the symlink in the OPK
# and specifying the real path might use a different name than the update_v
# script expects.
if [ "$KERNEL" ] ; then
	DATE=`date -r "$KERNEL" +%F`
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

if [ "$BOOTLOADERS" ] ; then

	echo -n "Calculating SHA1 sum of bootloaders... "
        sha1sum "$BOOTLOADERS" | cut -d' ' -f1 > "update_v/ubiboot-rg350.bin.sha1"
        echo "done"

        BOOTLOADERS="$BOOTLOADERS update_v/ubiboot-rg350.bin.sha1"
fi

if [ "$MININIT" ] ; then

	echo -n "Calculating SHA1 sum of mininit-syspart... "
	sha1sum "$MININIT" | cut -d' ' -f1 > "update_v/mininit-syspart.sha1"
	echo "done"

	MININIT="$MININIT update_v/mininit-syspart.sha1"
fi

echo "$DATE" > update_v/date.txt

echo "$VERSION" > update_v/version.txt

# Report metadata.
echo
echo "=========================="
echo "Bootloaders:          $BOOTLOADERS"
echo "Mininit:              $MININIT"
echo "Kernel:               $KERNEL"
echo "Modules file system:  $MODULES_FS"
echo "build date:           $DATE"
echo "build version:        $VERSION"
echo "=========================="
echo

# Write metadata.
cat > update_v/default.gcw0.desktop <<EOF
[Desktop Entry]
Name=Kernel update $VERSION
Comment=RG280V ROGUE update $DATE
Exec=update.sh
Icon=rogue
Terminal=true
Type=Application
StartupNotify=true
Categories=applications;
EOF

# Create OPK.
OPK_FILE=update_v/rg280v-Kernel-update-$VERSION-$DATE.opk
mksquashfs \
	update_v/default.gcw0.desktop \
	update_v/rogue.png \
	update_v/update.sh \
	update_v/trimfat.py \
	update_v/flash_partition.sh \
	update_v/date.txt \
	update_v/version.txt \
	update_v/fsck.fat \
	$BOOTLOADERS \
	$MININIT \
	$KERNEL \
	$MODULES_FS \
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

rm update_v/default.gcw0.desktop update_v/date.txt update_v/version.txt update_v/*.sha1
