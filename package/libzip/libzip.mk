#############################################################
#
# libzip
#
#############################################################
LIBZIP_VERSION:=0.10.1
LIBZIP_SOURCE:=libzip-$(LIBZIP_VERSION).tar.bz2
LIBZIP_SITE:=http://www.nih.at/libzip
LIBZIP_INSTALL_STAGING=YES

LIBZIP_DEPENDENCIES = zlib
HOST_LIBZIP_DEPENDENCIES = host-zlib

$(eval $(autotools-package))
$(eval $(host-autotools-package))
