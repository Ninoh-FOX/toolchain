################################################################################
#
# libpng16
#
################################################################################

LIBPNG16_VERSION = 1.6.16
LIBPNG16_SERIES = 16
LIBPNG16_SOURCE = libpng-$(LIBPNG16_VERSION).tar.gz
LIBPNG16_SITE = http://downloads.sourceforge.net/project/libpng/libpng${LIBPNG16_SERIES}/older-releases/$(LIBPNG16_VERSION)
LIBPNG16_LICENSE = libpng license
LIBPNG16_LICENSE_FILES = LICENSE
LIBPNG16_INSTALL_STAGING = YES
LIBPNG16_DEPENDENCIES = host-pkgconf zlib
LIBPNG16_CONFIG_SCRIPTS = libpng$(LIBPNG16_SERIES)-config libpng-config

$(eval $(autotools-package))
$(eval $(host-autotools-package))
