#!/bin/sh

failexit()
{
  echo "failed"
  /bin/mount -t ext4 -o noatime,nodiratime,rw /dev/mmcblk0p2 /media/data 1>/dev/null 2>&1
  echo -n 0 >/sys/devices/virtual/vtconsole/vtcon1/bind
  sleep 5
  exit 1
}

if [[ ! -d /media/data || -f /media/data/.partition_resized ]]; then
    echo
    echo -n "Unmounting..."
    if `/bin/umount -l /media/data 1>/dev/null 2>&1`; then
    echo "done"
    else
    failexit
    fi
    /bin/umount -l /dev/mmcblk0p2 1>/dev/null 2>&1
    /usr/sbin/e2fsck -f -y -C 0 /dev/mmcblk0p2
    /bin/mount -t ext4 -o noatime,nodiratime,rw /dev/mmcblk0p2 /media/data
    touch /media/data/.clean
    exit 1   
fi

echo -n 1 >/sys/devices/virtual/vtconsole/vtcon1/bind
clear

echo "Expanding internal partition on first boot."

echo
echo -n "Unmounting..."
if `/bin/umount -l /media/data 1>/dev/null 2>&1`; then
  echo "done"
else
  failexit
fi

echo
echo "Checking and extending partition..."

(echo d
echo 2
echo n
echo
echo
echo
echo
echo w
) | fdisk /dev/mmcblk0
/usr/sbin/partx -u /dev/mmcblk0
/bin/umount -l /dev/mmcblk0p2 1>/dev/null 2>&1
/usr/sbin/e2fsck -f -y -C 0 /dev/mmcblk0p2
/usr/sbin/e2fsck -f /dev/mmcblk0p2
/usr/sbin/resize2fs -p /dev/mmcblk0p2 || failexit
/bin/mount -t ext4 -o noatime,nodiratime,rw /dev/mmcblk0p2 /media/data
touch /media/data/.partition_resized

echo
echo "All done."

sleep 2

echo -n 0 >/sys/devices/virtual/vtconsole/vtcon1/bind