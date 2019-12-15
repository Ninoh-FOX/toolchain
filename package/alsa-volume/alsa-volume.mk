################################################################################
#
# alsa-volume
#
################################################################################

ALSA_VOLUME_VERSION = v1.0
ALSA_VOLUME_SITE = $(call github,opendingux,alsa-volume,$(ALSA_VOLUME_VERSION))
ALSA_VOLUME_LICENSE = Public domain
ALSA_VOLUME_LICENSE_FILES = LICENSE
ALSA_VOLUME_DEPENDENCIES = alsa-lib

define ALSA_VOLUME_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" -C $(@D) all
endef

define ALSA_VOLUME_INSTALL_TARGET_CMDS
	$(MAKE) DESTDIR="$(TARGET_DIR)" -C $(@D) install
endef

$(eval $(generic-package))
