################################################################################
#
# libopk
#
################################################################################

LIBOPK_VERSION = v1.0.6
LIBOPK_SITE = $(call github,gcwnow,libopk,$(LIBOPK_VERSION))
LIBOPK_DEPENDENCIES = zlib libini
LIBOPK_INSTALL_STAGING = YES

LIBOPK_MAKE_ENV = CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" \
				  CROSS_COMPILE="$(TARGET_CROSS)" PREFIX=/usr

define LIBOPK_BUILD_PYTHON
	(cd $(@D)/python ; $(HOST_DIR)/usr/bin/python setup.py build)
endef
define LIBOPK_INSTALL_PYTHON
	(cd $(@D)/python ; $(HOST_DIR)/usr/bin/python setup.py install --prefix=$(TARGET_DIR)/usr)
endef

ifeq ($(BR2_PACKAGE_PYTHON),y)
	LIBOPK_DEPENDENCIES += python host-python
	LIBOPK_POST_BUILD_HOOKS += LIBOPK_BUILD_PYTHON
	LIBOPK_POST_INSTALL_TARGET_HOOKS += LIBOPK_INSTALL_PYTHON
endif

define LIBOPK_BUILD_CMDS
	$(LIBOPK_MAKE_ENV) $(MAKE) -C $(@D)
endef

define LIBOPK_INSTALL_STAGING_CMDS
	$(LIBOPK_MAKE_ENV) DESTDIR="$(STAGING_DIR)" $(MAKE) -C $(@D) install
endef

define LIBOPK_INSTALL_TARGET_CMDS
	$(LIBOPK_MAKE_ENV) DESTDIR="$(TARGET_DIR)" $(MAKE) -C $(@D) install-bin install-lib
endef

$(eval $(generic-package))
