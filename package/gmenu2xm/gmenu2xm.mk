################################################################################
#
# GMENU2XMm
#
################################################################################
GMENU2XM_VERSION = master
GMENU2XM_SITE = $(call github,Ninoh-FOX,gmenu2xm,$(GMENU2XM_VERSION))
GMENU2XM_DEPENDENCIES = dejavu libpng sdl sdl_ttf
GMENU2XM_AUTORECONF = YES
GMENU2XM_CONF_OPT = \
	--with-sdl-prefix=$(STAGING_DIR)/usr \
	--enable-platform=$(BR2_PACKAGE_GMENU2XM_PLATFORM)

ifeq ($(BR2_PACKAGE_LIBOPK),y)
GMENU2XM_DEPENDENCIES += libopk
endif

ifeq ($(BR2_PACKAGE_LIBXDGMIME),y)
GMENU2XM_DEPENDENCIES += libxdgmime
endif

$(eval $(autotools-package))
