################################################################################
#
# o2xiv
#
################################################################################

O2XIV_VERSION = master
O2XIV_SITE = $(call github,pcercuei,o2xiv,$(O2XIV_VERSION))
O2XIV_LICENSE = GPLv2
O2XIV_LICENSE_FILES = COPYING

O2XIV_DEPENDENCIES = sdl sdl_image sdl_ttf
O2XIV_MAKE_ENV = CROSS_COMPILE="$(TARGET_CROSS)"

define O2XIV_BUILD_CMDS
	$(O2XIV_MAKE_ENV) $(MAKE) -C $(@D)
endef

ifeq ($(BR2_PACKAGE_SDL_COMPAT),y)
O2XIV_DEPENDENCIES += sdl_compat
endif

ifeq ($(BR2_PACKAGE_GMENU2X),y)
O2XIV_DEPENDENCIES += gmenu2x
define O2XIV_INSTALL_TARGET_GMENU2X
	$(INSTALL) -m 0644 -D package/o2xiv/gmenu2x \
		$(TARGET_DIR)/usr/share/gmenu2x/sections/applications/25_o2xiv
endef
else
ifeq ($(BR2_PACKAGE_GMENU2XM),y)
O2XIV_DEPENDENCIES += gmenu2x
define O2XIV_INSTALL_TARGET_GMENU2X
	$(INSTALL) -m 0644 -D package/o2xiv/gmenu2x \
		$(TARGET_DIR)/usr/share/gmenu2x/sections/applications/25_o2xiv
endef
endif
endif

define O2XIV_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/o2xiv $(TARGET_DIR)/usr/bin/o2xiv
	$(O2XIV_INSTALL_TARGET_GMENU2X)
endef

$(eval $(generic-package))
