################################################################################
#
# libgbm 2.0.0
#
################################################################################

LIBGBM_VERSION = master
LIBGBM_SOURCE = libdrm-$(LIBGBM_VERSION).tar.gz
LIBGBM_SITE = https://git.ti.com/cgit/glsdk/libgbm/snapshot
LIBGBM_LICENSE = Texas instruments
LIBGBM_INSTALL_STAGING = YES

LIBGBM_AUTORECONF = YES

LIBGBM_DEPENDENCIES = \
	libpthread-stubs \
	libdrm \
	libtool \
	host-pkgconf \
	host-xutil_util-macros

HOST_LIBGBM_DEPENDENCIES += host-automake host-autoconf host-libtool
	
LIBGBM_CONF_ENV := \
	LIBTOOL_FOR_BUILD="$(HOST_DIR)/usr/bin/libtool" \

LIBGBM_CONF_ENV = ac_cv_prog_cc_c99='-std=gnu99'

$(eval $(autotools-package))
