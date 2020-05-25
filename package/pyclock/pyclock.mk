################################################################################
#
# pyCLOCK
#
################################################################################
PYCLOCK_VERSION = master
PYCLOCK_SITE = $(call github,gcwnow,pyCLOCK,$(PYCLOCK_VERSION))
PYCLOCK_DEPENDENCIES = python-pygame

ifeq ($(BR2_PACKAGE_GMENU2X),y)
PYCLOCK_DEPENDENCIES += gmenu2x
define PYCLOCK_INSTALL_TARGET_GMENU2X
	$(INSTALL) -m 0644 -D package/pyclock/gmenu2x $(TARGET_DIR)/usr/share/gmenu2x/sections/applications/20_pyclock
endef
else
ifeq ($(BR2_PACKAGE_GMENU2XM),y)
PYCLOCK_DEPENDENCIES += gmenu2x
define PYCLOCK_INSTALL_TARGET_GMENU2X
	$(INSTALL) -m 0644 -D package/pyclock/gmenu2x $(TARGET_DIR)/usr/share/gmenu2x/sections/applications/20_pyclock
endef
endif
endif

define PYCLOCK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/pyclock.dge $(TARGET_DIR)/usr/bin/pyclock
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/usr/share/pyclock/
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/usr/share/pyclock/ $(@D)/*.png
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/usr/share/pyclock/ $(@D)/*.wav
	$(PYCLOCK_INSTALL_TARGET_GMENU2X)
endef

$(eval $(generic-package))
