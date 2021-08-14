#!/bin/bash

##Create MBR###
echo ""
echo "creating MBR"
echo ""
echo ""

source ./partition_layout.sh

echo "Creating master boot record..."
mkdir -p images
./genmbr.py > images/mbr.bin <<EOF
${SYSTEM_START},$((${DATA_START} - ${SYSTEM_START})),0b
${DATA_START},$((800 * 1024 * 2)),83
EOF

##Create DATA###
echo ""
echo ""
echo "---creating DATA-----------------------"
echo ""
echo ""

# Currently everything runs as root, but that is going to change.
USER_UID=0
USER_GID=0

source ./su_command.sh

TOTAL_SIZE=0

echo "Gathering applications..."
APPS=
for app in apps/*
do
	if test -f $app
	then
		APPS="${APPS} $app"
		SIZE=$(stat -Lc %s ${app})
		TOTAL_SIZE=$((${TOTAL_SIZE} + ${SIZE}))
		echo "app: ${app} - $((${SIZE} / 1024)) kB"
	elif [ $app != "apps/*" ]
	then
		echo "skipping non-regular file: $app"
	fi
done
echo "Total data size: ${TOTAL_SIZE} bytes"
echo

# Pick a partition size that is large enough to contain all files but not much
# larger so the image stays small.
IMAGE_SIZE=$((8 + ${TOTAL_SIZE} / (920*1024)))

echo "Creating data partition of ${IMAGE_SIZE} MB..."
mkdir -p images
dd if=/dev/zero of=images/data.bin bs=1M count=${IMAGE_SIZE}
/sbin/mkfs.ext4 -b4096 -I128 -m3 \
		-E lazy_itable_init=0,lazy_journal_init=0,resize=268435456 \
		-O large_file,^huge_file,^uninit_bg,^ext_attr,^has_journal \
		-F images/data.bin
echo

echo "Populating data partition..."
echo "(this step needs superuser privileges)"
mkdir mnt
${SU_CMD} "
	mount images/data.bin mnt -o loop &&
	install -m 755 -o ${USER_UID} -g ${USER_GID} -d mnt/apps/ &&
	if [ \"${APPS}\" != \"\" ]
	then
		install -m 644 -o ${USER_UID} -g ${USER_GID} -t mnt/apps/ ${APPS}
	fi &&
	install -m 755 -o 0 -g 0 -d mnt/local/etc/init.d &&
	install -m 755 -o 0 -g 0 resize_data_part.target-sh mnt/local/etc/init.d/S00resize &&
	umount mnt
	"
rmdir mnt

##Create SYSTEM###
echo ""
echo ""
echo "---creating SYSTEM-----------------------"
echo ""
echo ""

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

##Create SDIMAGE###
echo ""
echo ""
echo "---creating SDIMAGE-----------------------"
echo ""
echo ""

cp ../../output/images/ubiboot/ubiboot-rg350.bin ubiboot.bin

source ./partition_layout.sh

( cp images/mbr.bin images/sd_image.bin &&
  dd if=ubiboot.bin of=images/sd_image.bin ibs=512 seek=1 &&
  dd if=images/system.bin of=images/sd_image.bin ibs=512 seek=${SYSTEM_START} &&
  dd if=images/data.bin of=images/sd_image.bin ibs=512 seek=${DATA_START}
) || rm -f images/sd_image.bin


rm -f images/mbr.bin images/system.bin images/data.bin ubiboot.bin
echo ""
echo ""
echo "All done!!"
echo "SDIMAGE is in images/sd_image.bin"
