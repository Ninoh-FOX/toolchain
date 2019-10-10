#!/bin/sh

OPK_NAME=rg350_update_2019_10_09.opk

echo ${OPK_NAME}

# create default.gcw0.desktop
cat > default.gcw0.desktop <<EOF
[Desktop Entry]
Name=OS Update
Comment=JBDoge Update 2019-10-09
Exec=update.sh
Icon=Update
Terminal=true
Type=Application
StartupNotify=true
Categories=applications;
EOF

# create sha1sums (TODO: move this elsewhere)
sha1sum output/images/vmlinuz.bin > output/images/vmlinuz.bin.sha1
sha1sum output/images/rootfs.squashfs > output/images/rootfs.squashfs.sha1
sha1sum output/images/rootfs.squashfs > output/images/modules.squashfs.sha1

# create opk
FLIST="output/images/vmlinuz.bin"
FLIST="${FLIST} output/images/rootfs.squashfs"
FLIST="${FLIST} output/images/modules.squashfs"
FLIST="output/images/vmlinuz.bin.sha1"
FLIST="${FLIST} output/images/rootfs.squashfs.sha1"
FLIST="${FLIST} output/images/modules.squashfs.sha1"
FLIST="${FLIST} board/opendingux/gcw0/flash_partition.sh"
FLIST="${FLIST} board/opendingux/gcw0/update.sh"
FLIST="${FLIST} board/opendingux/gcw0/trimfat.py"
FLIST="${FLIST} default.gcw0.desktop"
# icon file
FLIST="${FLIST} board/opendingux/gcw0/Update.png"
FLIST="${FLIST} board/opendingux/gcw0/opendingux.png"

rm -f ${OPK_NAME}
mksquashfs ${FLIST} output/images/${OPK_NAME} -all-root -no-xattrs -noappend -no-exports

cat default.gcw0.desktop
rm -f default.gcw0.desktop
