################################################################################
#
# libgbm 10.3.0
#
################################################################################

LIBGBM_VERSION = bc04aefc
LIBGBM_SOURCE = libgbm-$(LIBGBM_VERSION).tar.gz
LIBGBM_SITE = $(call github,thayama,libgbm,$(LIBGBM_VERSION))
LIBGBM_INSTALL_STAGING = YES

LIBGBM_AUTORECONF = YES

LIBGBM_DEPENDENCIES = \
	libpthread-stubs \
	libdrm \
	wayland-kms \
	libtool \
	host-pkgconf \
	host-xutil_util-macros

HOST_LIBGBM_DEPENDENCIES += host-automake host-autoconf host-libtool
	
LIBGBM_CONF_ENV := \
	LIBTOOL_FOR_BUILD="$(HOST_DIR)/usr/bin/libtool" \

LIBGBM_CONF_ENV += ac_cv_prog_cc_c99='-std=gnu99'

$(eval $(autotools-package))
