################################################################################
#
# libfuse
#
################################################################################

LIBFUSE_VERSION = 2.9.7
LIBFUSE_SOURCE = fuse_$(LIBFUSE_VERSION).orig.tar.gz
LIBFUSE_SITE = http://ftp.debian.org/debian/pool/main/f/fuse/
LIBFUSE_LICENSE = GPLv2 LGPLv2.1
LIBFUSE_LICENSE_FILES = COPYING COPYING.LIB
LIBFUSE_INSTALL_STAGING = YES
LIBFUSE_DEPENDENCIES = $(if $(BR2_PACKAGE_LIBICONV),libiconv)
LIBFUSE_CONF_OPT = \
		--disable-example \
		--enable-lib \
		--enable-util

define LIBFUSE_INSTALL_TARGET_CMDS
	cp -dpf $(STAGING_DIR)/usr/bin/fusermount $(TARGET_DIR)/usr/bin/
	cp -dpf $(STAGING_DIR)/usr/lib/libfuse.so* $(TARGET_DIR)/usr/lib/
endef

$(eval $(autotools-package))
