################################################################################
#
# opkg
#
################################################################################

OPKG_VERSION = v0.2.2
OPKG_SITE = http://git.yoctoproject.org/git/opkg
OPKG_SITE_METHOD = git
# Uses PKG_CHECK_MODULES() in configure.ac
OPKG_DEPENDENCIES = host-pkgconf
OPKG_LICENSE = GPLv2+
OPKG_LICENSE_FILES = COPYING
OPKG_INSTALL_STAGING = YES
OPKG_CONF_OPT = --disable-curl
OPKG_AUTORECONF = YES

OPKG_DEPENDENCIES += libcurl

# Ensure directory for lockfile exists
define OPKG_CREATE_LOCKDIR
	mkdir -p $(TARGET_DIR)/usr/lib/opkg
endef

ifeq ($(BR2_PACKAGE_OPKG_GPG_SIGN),y)
OPKG_CONF_OPT += --enable-gpg
OPKG_CONF_ENV = ac_cv_path_GPGME_CONFIG=$(STAGING_DIR)/usr/bin/gpgme-config
OPKG_DEPENDENCIES += libgpgme
else
OPKG_CONF_OPT += --disable-gpg
endif

OPKG_POST_INSTALL_TARGET_HOOKS += OPKG_CREATE_LOCKDIR

$(eval $(autotools-package))
