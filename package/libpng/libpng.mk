################################################################################
#
# libpng 1.4.13 >> 1.4.15
#
################################################################################

LIBPNG_VERSION = 1.4.15
LIBPNG_SERIES = 14
LIBPNG_SOURCE = libpng-$(LIBPNG_VERSION).tar.gz
LIBPNG_SITE = http://downloads.sourceforge.net/project/libpng/libpng${LIBPNG_SERIES}/older-releases/$(LIBPNG_VERSION)
LIBPNG_LICENSE = libpng license
LIBPNG_LICENSE_FILES = LICENSE
LIBPNG_INSTALL_STAGING = YES
LIBPNG_DEPENDENCIES = host-pkgconf zlib
LIBPNG_CONFIG_SCRIPTS = libpng$(LIBPNG_SERIES)-config libpng-config

$(eval $(autotools-package))
$(eval $(host-autotools-package))
