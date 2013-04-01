################################################################################
#
# bennugd
#
################################################################################

BENNUGD_VERSION = master
BENNUGD_SITE = $(call github,gromv,bennugd_cmake,$(BENNUGD_VERSION))
BENNUGD_DEPENDENCIES = libpng sdl sdl_mixer openssl
BENNUGD_INSTALL_TARGET = YES

$(eval $(cmake-package))
