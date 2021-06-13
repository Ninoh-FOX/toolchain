################################################################################
#
# libsndfile
#
################################################################################

LIBSNDFILE_VERSION = 1.0.25
LIBSNDFILE_SITE = http://www.mega-nerd.com/libsndfile/files
LIBSNDFILE_INSTALL_STAGING = YES
LIBSNDFILE_LICENSE = LGPLv2.1+
LIBSNDFILE_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_FLAC),y)
LIBSNDFILE_DEPENDENCIES += flac
endif

ifeq ($(BR2_PACKAGE_LIBOGG),y)
LIBSNDFILE_DEPENDENCIES += libogg
endif

ifeq ($(BR2_PACKAGE_LIBVORBIS),y)
LIBSNDFILE_DEPENDENCIES += libvorbis
endif

$(eval $(autotools-package))
