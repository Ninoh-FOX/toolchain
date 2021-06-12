################################################################################
#
# gmenu2x
#
################################################################################
GMENU2X_VERSION = master
GMENU2X_SITE = $(call github,Ninoh-FOX,gmenu2x,$(GMENU2X_VERSION))
GMENU2X_DEPENDENCIES = dejavu sdl sdl_ttf libpng
GMENU2X_AUTORECONF = YES
GMENU2X_CONF_OPT = \
	--with-sdl-prefix=$(STAGING_DIR)/usr \
	--enable-platform=$(BR2_PACKAGE_GMENU2X_PLATFORM) \
	--enable-stick=$(BR2_PACKAGE_GMENU2X_STICK) \
	--enable-battery=$(BR2_PACKAGE_GMENU2X_BATTERY)

ifeq ($(BR2_PACKAGE_SDL_COMPAT),y)
GMENU2X_DEPENDENCIES += sdl_compat
endif

ifeq ($(BR2_PACKAGE_LIBOPK),y)
GMENU2X_DEPENDENCIES += libopk
endif

ifeq ($(BR2_PACKAGE_LIBXDGMIME),y)
GMENU2X_DEPENDENCIES += libxdgmime
endif

$(eval $(autotools-package))
