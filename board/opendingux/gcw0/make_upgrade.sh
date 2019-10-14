#!/bin/sh

KERNEL=output/images/vmlinuz.bin
ROOTFS=output/images/rootfs.squashfs
MODULES=output/images/modules.squashfs

# create sha1sums
sha1sum "$KERNEL" | cut -d' ' -f1 > "$KERNEL.sha1"
sha1sum "$ROOTFS" | cut -d' ' -f1 > "$ROOTFS.sha1"
sha1sum "$MODULES" | cut -d' ' -f1 > "$MODULES.sha1"

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

OPK_NAME=rg350-update-$DATE.opk

# Report metadata.
echo
echo "=========================="
echo
echo "NAME                  $OPK_NAME"
echo "Bootloaders:          $BOOTLOADERS"
echo "Mininit:              $MININIT"
echo "Kernel:               $KERNEL"
echo "Modules file system:  $MODULES"
echo "Root file system:     $ROOTFS"
echo "  build date:         $DATE"
echo
echo "=========================="
echo

# create default.gcw0.desktop
cat > default.gcw0.desktop <<EOF
[Desktop Entry]
Name=OS Update
Comment=OpenDingux Update $DATE
Exec=update.sh
Icon=opendingux
Terminal=true
Type=Application
StartupNotify=true
Categories=applications;
EOF

echo "$DATE" > output/images/date.txt

# create opk
FLIST="$KERNEL $KERNEL.sha1"
FLIST="${FLIST} $ROOTFS $ROOTFS.sha1"
FLIST="${FLIST} $MODULES $MODULES.sha1"
FLIST="${FLIST} output/images/date.txt"
FLIST="${FLIST} board/opendingux/gcw0/flash_partition.sh"
FLIST="${FLIST} board/opendingux/gcw0/update.sh"
FLIST="${FLIST} board/opendingux/gcw0/trimfat.py"
FLIST="${FLIST} default.gcw0.desktop"
# icon file
FLIST="${FLIST} board/opendingux/gcw0/Update.png"
FLIST="${FLIST} board/opendingux/gcw0/opendingux.png"

rm -f output/images/${OPK_NAME}
mksquashfs ${FLIST} output/images/${OPK_NAME} -no-progress -noappend -comp gzip -all-root

cat default.gcw0.desktop
rm -f default.gcw0.desktop
