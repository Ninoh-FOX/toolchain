#!/bin/sh

KERNEL=output/images/vmlinuz.bin
ROOTFS=output/images/rootfs.squashfs
MODULES=output/images/modules.squashfs

# create sha1sums
sha1sum "$KERNEL" | cut -d' ' -f1 > "$KERNEL.sha1"
sha1sum "$ROOTFS" | cut -d' ' -f1 > "$ROOTFS.sha1"
sha1sum "$MODULES" | cut -d' ' -f1 > "$MODULES.sha1"

cp -f $KERNEL output/images/vmlinuz.bak
cp -f $KERNEL.sha1 output/images/vmlinuz.bak.sha1
cp -f $MODULES $MODULES.bak
cp -f $MODULES.sha1 $MODULES.bak.sha1

# remove previous packages
rm -rf output/images/boot.vfat
rm -rf output/images/apps.ext4
rm -rf output/images/hibernation.ext4
rm -rf output/images/sdcard.img

# local_pack_root is where the factory version emulators and ports should reside
mkdir -p output/images/local_pack_root/apps

echo Downloading rs97.bitgala.xyz/RG-350/localpack/emulators/...
( cd  output/images/local_pack_root/apps ; \
wget -N -r -nd --no-parent --reject="index.html*" --reject="scumm*" \
https://rs97.bitgala.xyz/RG-350/localpack/emulators/ ) &> /tmp/emulators.log

echo Downloading rs97.bitgala.xyz/RG-350/localpack/apps/...
( cd  output/images/local_pack_root/apps ; \
wget -N -r -nd --no-parent --reject="index.html*" \
https://rs97.bitgala.xyz/RG-350/localpack/apps/ ) &> /tmp/apps.log

# hiberation_root is just an empty directory to trick genimage into creating
# an empty ext4 partition.  Always clear it out, as it should be empty
mkdir -p /tmp/hiberation_root
rm -rf /tmp/hiberation_root/*

# Clear output/images-tmp
rm -rf output/images-tmp

# Build sdcard.img from board/opendingux/gcw0/genimage.cfg
output/host/usr/bin/genimage \
	    --config board/opendingux/gcw0/genimage.cfg \
	    --rootpath /tmp/hiberation_root \
	    --inputpath output/images \
	    --outputpath output/images \
	    --tmppath output/images-tmp

# Clear output/images-tmp
rm -rf output/images-tmp


