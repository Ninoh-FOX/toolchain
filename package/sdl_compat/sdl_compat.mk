################################################################################
#
# sdl
#
################################################################################

SDL_COMPAT_VERSION = main
SDL_COMPAT_SITE = $(call github,libsdl-org,sdl12-compat,$(SDL_COMPAT_VERSION))
SDL_COMPAT_LICENSE = LGPL-2.1+
SDL_COMPAT_LICENSE_FILES = COPYING
SDL_COMPAT_INSTALL_STAGING = YES

SDL_COMPAT_DEPENDENCIES = sdl sdl2 libgles

ifeq ($(BR2_PACKAGE_MESA3D),y)
SDL_COMPAT_DEPENDENCIES += mesa3d
endif

ifeq ($(BR2_PACKAGE_MESA3D_ETNA_VIV),y)
SDL_COMPAT_DEPENDENCIES += mesa3d-etna_viv
endif

SDL_COMPAT_CONF_ENV = ac_cv_prog_cc_c99='-std=gnu99'

SDL_COMPAT_CONFIG_SCRIPTS = sdl-config

$(eval $(cmake-package))
