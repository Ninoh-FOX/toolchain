################################################################################
#
# gmenu2x
#
################################################################################
GMENU2X_VERSION = master
GMENU2X_SITE = $(call github,Ninoh-FOX,gmenu2x,$(GMENU2X_VERSION))
GMENU2X_DEPENDENCIES = dejavu libpng sdl sdl_ttf
GMENU2X_AUTORECONF = YES
GMENU2X_CONF_OPT = \
	--with-sdl-prefix=$(STAGING_DIR)/usr \
	--enable-platform=$(BR2_PACKAGE_GMENU2X_PLATFORM) \
	--enable-stick=$(BR2_PACKAGE_GMENU2X_STICK) \
	--enable-battery=$(BR2_PACKAGE_GMENU2X_BATTERY)
	
ifneq ($(BR2_PACKAGE_GMENU2X_WIDTH),"")
GMENU2X_CONF_OPTS += -DSCREEN_WIDTH=$(BR2_PACKAGE_GMENU2X_WIDTH) -DSCREEN_HEIGHT=$(BR2_PACKAGE_GMENU2X_HEIGHT)
endif

ifneq ($(BR2_PACKAGE_GMENU2X_DEFAULT_FALLBACK_FONTS),"")
GMENU2X_CONF_OPTS += -DDEFAULT_FALLBACK_FONTS=$(BR2_PACKAGE_GMENU2X_DEFAULT_FALLBACK_FONTS)
endif

ifeq ($(BR2_PACKAGE_LIBOPK),y)
GMENU2X_DEPENDENCIES += libopk
endif

ifeq ($(BR2_PACKAGE_LIBXDGMIME),y)
GMENU2X_DEPENDENCIES += libxdgmime
endif

$(eval $(autotools-package))
