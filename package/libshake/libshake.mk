#############################################################
#
# libshake 0.3.0 api1
#
#############################################################
LIBSHAKE_VERSION = 0.3.0
LIBSHAKE_SITE = $(call github,zear,libShake,$(LIBSHAKE_VERSION))
LIBSHAKE_LICENSE = MIT
LIBSHAKE_LICENSE_FILES = LICENSE.txt
LIBSHAKE_INSTALL_STAGING = YES

LIBSHAKE_MAKE_ENV = PREFIX=/usr PLATFORM=$(BR2_PACKAGE_LIBSHAKE_PLATFORM)
define LIBSHAKE_BUILD_CMDS
	$(LIBSHAKE_MAKE_ENV) $(MAKE) -C $(@D)
endef

define LIBSHAKE_INSTALL_STAGING_CMDS
	$(LIBSHAKE_MAKE_ENV) DESTDIR="$(STAGING_DIR)" $(MAKE) -C $(@D) install
endef

define LIBSHAKE_INSTALL_TARGET_CMDS
	$(LIBSHAKE_MAKE_ENV) DESTDIR="$(TARGET_DIR)" $(MAKE) -C $(@D) install-lib
endef

$(eval $(generic-package))