################################################################################
#
# gmenunx
#
################################################################################
GMENUNX_VERSION = 8e053793
GMENUNX_SITE = $(call github,podulator,gmenunx,$(GMENUNX_VERSION))
GMENUNX_DEPENDENCIES = dejavu libpng sdl sdl_ttf
GMENUNX_INSTALL_TARGET = YES

GMENUNX_MAKE_OPTS = \
    MD="mkdir -p" \
    CROSS_COMPILE="$(TARGET_CROSS)"
    
ifeq ($(BR2_PACKAGE_LIBOPK),y)
GMENUNX_DEPENDENCIES += libopk
endif

ifeq ($(BR2_PACKAGE_LIBXDGMIME),y)
GMENUNX_DEPENDENCIES += libxdgmime
endif

define GMENUNX_BUILD_CMDS
    $(MAKE) $(GMENUNX_MAKE_OPTS) -C $(@D) all dist \
        -f Makefile.${BR2_PACKAGE_GMENUNX_PLATFORM}
endef

define GMENUNX_INSTALL_GMENUNX
    (   cd $(@D) ; \
	cp -r assets/$(BR2_PACKAGE_GMENUNX_PLATFORM) $(TARGET_DIR)/usr/share/gmenunx ; \
	cp dist/$(BR2_PACKAGE_GMENUNX_PLATFORM)/gmenunx/gmenunx $(TARGET_DIR)/usr/bin/ ; )
endef
GMENUNX_POST_INSTALL_TARGET_HOOKS += GMENUNX_INSTALL_GMENUNX


$(eval $(generic-package))