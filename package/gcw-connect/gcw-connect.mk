################################################################################
#
# gcw-connect
#
################################################################################

GCW_CONNECT_VERSION = master
GCW_CONNECT_SITE = $(call github,Ninoh-FOX,gcwconnect,$(GCW_CONNECT_VERSION))
GCW_CONNECT_DEPENDENCIES = python python-pygame

ifeq ($(BR2_PACKAGE_GMENU2X),y)
GCW_CONNECT_DEPENDENCIES += gmenu2x
define GCW_CONNECT_INSTALL_TARGET_GMENU2X
	$(INSTALL) -m 0644 -D package/gcw-connect/gmenu2x \
		$(TARGET_DIR)/usr/share/gmenu2x/sections/settings/60_wireless
	$(INSTALL) -m 0644 -D $(@D)/icon.png \
		$(TARGET_DIR)/usr/share/gmenu2x/skins/Default/icons/wireless.png
endef
GCW_CONNECT_POST_INSTALL_TARGET_HOOKS += GCW_CONNECT_INSTALL_TARGET_GMENU2X
else
ifeq ($(BR2_PACKAGE_GMENU2XM),y)
GCW_CONNECT_DEPENDENCIES += gmenu2x
define GCW_CONNECT_INSTALL_TARGET_GMENU2X
	$(INSTALL) -m 0644 -D package/gcw-connect/gmenu2x \
		$(TARGET_DIR)/usr/share/gmenu2x/sections/settings/60_wireless
	$(INSTALL) -m 0644 -D $(@D)/icon.png \
		$(TARGET_DIR)/usr/share/gmenu2x/skins/Default/icons/wireless.png
endef
GCW_CONNECT_POST_INSTALL_TARGET_HOOKS += GCW_CONNECT_INSTALL_TARGET_GMENU2X
endif
endif


define GCW_CONNECT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/gcwconnect.py \
		$(TARGET_DIR)/usr/bin/gcwconnect
	cp -r $(@D)/data $(TARGET_DIR)/usr/share/gcwconnect
endef

$(eval $(generic-package))
