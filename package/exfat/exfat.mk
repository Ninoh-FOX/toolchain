################################################################################
#
# exfat
#
################################################################################

EXFAT_VERSION = 1.3.0
EXFAT_SITE = http://ftp.debian.org/debian/pool/main/f/fuse-exfat/
EXFAT_SOURCE = fuse-exfat_$(EXFAT_VERSION).orig.tar.gz
EXFAT_DEPENDENCIES = libfuse
EXFAT_LICENSE = GPLv3+
EXFAT_LICENSE_FILES = COPYING
EXFAT_AUTORECONF = YES

$(eval $(autotools-package))
