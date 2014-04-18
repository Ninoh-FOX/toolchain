#!/bin/sh

TARGET_DIR=$1

# Remove binutils executables we don't use
for i in ar as ld ld.bfd nm objcopy ranlib strip ; do
	rm -f ${TARGET_DIR}/usr/bin/$i
done
rm -rf ${TARGET_DIR}/usr/mipsel-gcw0-linux-uclibc

# We use xinetd, so no need for a startup script for the SSH daemons
# and the FTP server.
rm -f ${TARGET_DIR}/etc/init.d/S50sshd
rm -f ${TARGET_DIR}/etc/init.d/S50dropbear
rm -f ${TARGET_DIR}/etc/init.d/S70vsftpd

# The NTP daemon is started in the /etc/network/if-up.d/ntpd
# script and stopped in /etc/network/if-post-down.d/ntpd
rm -f ${TARGET_DIR}/etc/init.d/S49ntp

# Remove the DRI drivers
rm -rf ${TARGET_DIR}/usr/lib/dri
