#############################################################
#
# LIBSHAKE2 0.3.1 api2
#
#############################################################
LIBSHAKE2_VERSION = 0.3.1
LIBSHAKE2_SITE = $(call github,zear,libshake,$(LIBSHAKE2_VERSION))
LIBSHAKE2_LICENSE = MIT
LIBSHAKE2_LICENSE_FILES = LICENSE.txt
LIBSHAKE2_INSTALL_STAGING = YES

LIBSHAKE2_MAKE_ENV = PREFIX=/usr PLATFORM=$(BR2_PACKAGE_LIBSHAKE2_PLATFORM)

define LIBSHAKE2_BUILD_CMDS
	$(LIBSHAKE2_MAKE_ENV) $(MAKE) -C $(@D)
endef

define LIBSHAKE2_INSTALL_STAGING_CMDS
	$(LIBSHAKE2_MAKE_ENV) DESTDIR="$(STAGING_DIR)" $(MAKE) -C $(@D) install
endef

define LIBSHAKE2_INSTALL_TARGET_CMDS
	$(LIBSHAKE2_MAKE_ENV) DESTDIR="$(TARGET_DIR)" $(MAKE) -C $(@D) install-lib
endef

$(eval $(generic-package))