################################################################################
#
# wayland
#
################################################################################

WAYLAND_KMS_VERSION = master
WAYLAND_KMS_SITE = $(call github,thayama,wayland-kms,$(WAYLAND_KMS_VERSION))
WAYLAND_KMS_SOURCE = wayland-kms-$(WAYLAND_KMS_VERSION).tar.gz
WAYLAND_KMS_LICENSE = MIT
WAYLAND_KMS_LICENSE_FILES = COPYING

WAYLAND_KMS_AUTORECONF = YES

WAYLAND_KMS_INSTALL_STAGING = YES
WAYLAND_KMS_DEPENDENCIES = \
	wayland \
	libdrm \
	etna_viv \
	expat \
	host-pkgconf \
	host-wayland \
	host-gawk \
	host-flex \
	host-bison \
	host-python \
	host-libxml2
	
ifeq ($(BR2_PACKAGE_MESA3D),y)
WAYLAND_KMS_DEPENDENCIES += mesa3d
endif

ifeq ($(BR2_PACKAGE_MESA3D_ETNA_VIV),y)
WAYLAND_KMS_DEPENDENCIES +=	mesa3d-etna_viv
endif

$(eval $(autotools-package))
$(eval $(host-autotools-package))
