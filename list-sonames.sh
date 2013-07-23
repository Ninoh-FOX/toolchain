#!/bin/sh
# Lists the SONAME entries of all libraries in /usr/lib of the sysroot.
# This can be used to check whether upgraded packages will break binary
# compatibility.

ARCH=`sed -ne 's%^BR2_ARCH="\(.*\)"$%\1%p' .config`
VENDOR=`sed -ne 's%^BR2_TOOLCHAIN_BUILDROOT_VENDOR="\(.*\)"$%\1%p' .config`
HOSTDIR=`sed -ne 's%^BR2_HOST_DIR="\(.*\)"$%\1%p' .config`
HOSTDIR=`echo "$HOSTDIR" | sed -e 's%\\$[(]BASE_DIR[)]%output%'`

SYSTEM="${ARCH}-${VENDOR}-linux-uclibc"
SYSROOT="${HOSTDIR}/usr/${SYSTEM}/sysroot"

if [ ! -d "${SYSROOT}" ]
then
	echo "ERROR: No sysroot at ${SYSROOT}"
	exit 1
fi

for lib in ${SYSROOT}/usr/lib/*.so
do
	SONAME=`objdump -p "${lib}" 2> /dev/null | sed -ne 's%[ ^t]*SONAME[ ^t]*%%p'`
	if [ -n "${SONAME}" ]
	then
		echo "${SONAME}"
	fi
done
