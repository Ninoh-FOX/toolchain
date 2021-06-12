################################################################################
#
# bennugd
#
################################################################################

BENNUGD_VERSION = master
BENNUGD_SITE = $(call github,gromv,bennugd_cmake,$(BENNUGD_VERSION))
BENNUGD_DEPENDENCIES = sdl sdl_mixer openssl libpng
BENNUGD_INSTALL_TARGET = YES

ifeq ($(BR2_PACKAGE_SDL_COMPAT),y)
BENNUGD_DEPENDENCIES += sdl_compat
endif

$(eval $(cmake-package))
