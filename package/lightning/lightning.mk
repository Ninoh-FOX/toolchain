################################################################################
#
# lightning
#
################################################################################

LIGHTNING_VERSION = 2.1.3
LIGHTNING_SITE = http://ftp.gnu.org/gnu/lightning/
LIGHTNING_LICENSE = GPLv3
LIGHTNING_LICENSE_FILES = COPYING
LIGHTNING_INSTALL_STAGING = YES
LIGHTNING_AUTORECONF = YES

ifeq ($(BR2_PACKAGE_BINUTILS),y)
LIGHTNING_DEPENDENCIES += binutils zlib
LIGHTNING_CONF_OPT += --enable-disassembler
endif

$(eval $(autotools-package))
