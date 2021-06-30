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
	host-pkgconf \
	host-wayland \
	host-gawk \
	host-flex \
	host-bison \
	host-python \
	host-libxml2
$(eval $(autotools-package))
$(eval $(host-autotools-package))
