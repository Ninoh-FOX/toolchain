################################################################################
#
# f2fs-tools
#
################################################################################

F2FS_TOOLS_VERSION = 1.11.0
F2FS_TOOLS_SOURCE = f2fs-tools_$(F2FS_TOOLS_VERSION).orig.tar.gz
F2FS_TOOLS_SITE = http://ftp.debian.org/debian/pool/main/f/f2fs-tools/
F2FS_TOOLS_DEPENDENCIES = host-pkgconf util-linux
# GIT version, shipped without configure
F2FS_TOOLS_AUTORECONF = YES
F2FS_TOOLS_INSTALL_STAGING = YES
F2FS_TOOLS_LICENSE = GPLv2
F2FS_TOOLS_LICENSE_FILES = COPYING

$(eval $(autotools-package))
