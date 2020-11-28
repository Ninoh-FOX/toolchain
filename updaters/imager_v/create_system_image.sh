#!/bin/bash

source ./partition_layout.sh

if ! which mcopy > /dev/null
then
	echo "Please install the 'mtools' package."
	exit 1
fi

echo "Checking presence of kernel files..."
if test -f ../../output/images/vmlinuz.bin
then
	SIZE=$(stat -Lc %s ../../output/images/vmlinuz.bin)
	echo "vmlinuz.bin: $((${SIZE} / 1024)) kB"
else
	echo "missing main kernel: vmlinuz.bin"
	exit 1
fi

echo "Checking presence of mininit-syspart..."
if test -f mininit-syspart
then
	SIZE=$(stat -Lc %s mininit-syspart)
	echo "mininit-syspart: $((${SIZE} / 1024)) kB"
else
	echo "missing program: mininit-syspart"
	exit 1
fi

echo "Checking presence of root filesystem..."
if test -f ../../output/images/rootfs.squashfs
then
	SIZE=$(stat -Lc %s ../../output/images/rootfs.squashfs)
	echo "rootfs.squashfs: $((${SIZE} / 1024)) kB"
else
	echo "missing root filesystem: rootfs.squashfs"
	exit 1
fi

echo "Checking presence of modules filesystem..."
if test -f ../../output/images/modules.squashfs
then
	SIZE=$(stat -Lc %s ../../output/images/modules.squashfs)
	echo "modules.squashfs: $((${SIZE} / 1024)) kB"
else
	echo "missing modules filesystem: modules.squashfs"
	exit 1
fi

echo "Creating system partition..."
IMAGE_SIZE=$((${DATA_START} - ${SYSTEM_START}))
mkdir -p images
dd if=/dev/zero of=images/system.bin bs=512 count=${IMAGE_SIZE} status=noxfer
/sbin/mkdosfs -s 8 -F 32 images/system.bin
echo

echo "Populating system partition..."
mmd -i images/system.bin ::dev ::root
mcopy -i images/system.bin ../../output/images/vmlinuz.bin ::vmlinuz.bin
sha1sum ../../output/images/vmlinuz.bin | cut -d' ' -f1 | mcopy -i images/system.bin - ::vmlinuz.bin.sha1
mcopy -i images/system.bin mininit-syspart ::mininit-syspart
sha1sum mininit-syspart | cut -d' ' -f1 | mcopy -i images/system.bin - ::mininit-syspart.sha1
mcopy -i images/system.bin ../../output/images/modules.squashfs ::modules.squashfs
sha1sum ../../output/images/modules.squashfs | cut -d' ' -f1 | mcopy -i images/system.bin - ::modules.squashfs.sha1
mcopy -i images/system.bin ../../output/images/rootfs.squashfs ::rootfs.squashfs
sha1sum ../../output/images/rootfs.squashfs | cut -d' ' -f1 | mcopy -i images/system.bin - ::rootfs.squashfs.sha1

echo "Minimizing image size..."
./trimfat.py images/system.bin
