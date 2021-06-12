################################################################################
#
# DinguxCommander
#
################################################################################

DINGUX_COMMANDER_VERSION = 1760de1
DINGUX_COMMANDER_SITE = $(call github,Ninoh-FOX,od-commander,$(DINGUX_COMMANDER_VERSION))
DINGUX_COMMANDER_DEPENDENCIES = sdl sdl2 sdl_gfx sdl2_gfx sdl_image sdl_ttf dejavu

DINGUX_COMMANDER_RESDIR = usr/share/DinguxCommander

ifeq ($(BR2_PACKAGE_SDL_COMPAT),y)
DINGUX_COMMANDER_DEPENDENCIES += sdl_compat
endif

ifeq ($(BR2_PACKAGE_DINGUX_COMMANDER_TARGET_PLATFORM),"rg350")
DINGUX_COMMANDER_CONF_OPT += \
	-DTARGET_PLATFORM=rg350 \
	-DAUTOSCALE=1 \
	-DSCREEN_WIDTH=320 \
	-DSCREEN_HEIGHT=240 \
	-DPPU_X=1 \
	-DPPU_Y=1 \
	-DWITH_SYSTEM_SDL_TTF=ON \
	-DWITH_SYSTEM_SDL_GFX=ON
else
ifeq ($(BR2_PACKAGE_DINGUX_COMMANDER_TARGET_PLATFORM),"rg350m")
DINGUX_COMMANDER_CONF_OPT += \
	-DTARGET_PLATFORM=rg350 \
	-DAUTOSCALE=1 \
	-DSCREEN_WIDTH=640 \
	-DSCREEN_HEIGHT=480 \
	-DPPU_X=2 \
	-DPPU_Y=2 \
	-DWITH_SYSTEM_SDL_TTF=ON \
	-DWITH_SYSTEM_SDL_GFX=ON
endif
endif



ifeq ($(BR2_PACKAGE_GMENU2X),y)
DINGUX_COMMANDER_DEPENDENCIES += gmenu2x
define DINGUX_COMMANDER_INSTALL_TARGET_GMENU2X
	$(INSTALL) -m 0644 -D package/dingux-commander/gmenu2x $(TARGET_DIR)/usr/share/gmenu2x/sections/applications/25_DinguxCommander
endef
else
ifeq ($(BR2_PACKAGE_GMENU2XM),y)
DINGUX_COMMANDER_DEPENDENCIES += gmenu2x
define DINGUX_COMMANDER_INSTALL_TARGET_GMENU2X
	$(INSTALL) -m 0644 -D package/dingux-commander/gmenu2x $(TARGET_DIR)/usr/share/gmenu2x/sections/applications/25_DinguxCommander
endef
endif
endif

define DINGUX_COMMANDER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/commander $(TARGET_DIR)/usr/bin/DinguxCommander
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/$(DINGUX_COMMANDER_RESDIR)/
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/$(DINGUX_COMMANDER_RESDIR)/ $(@D)/res/*
	$(DINGUX_COMMANDER_INSTALL_TARGET_GMENU2X)
endef

$(eval $(cmake-package))
