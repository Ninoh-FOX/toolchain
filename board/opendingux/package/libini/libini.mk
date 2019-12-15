################################################################################
#
# libini
#
################################################################################

LIBINI_VERSION = master
LIBINI_SITE = $(call github,pcercuei,libini,$(LIBINI_VERSION))
LIBINI_INSTALL_STAGING = YES

define LIBINI_BUILD_PYTHON
	(cd $(@D)/python ; $(HOST_DIR)/usr/bin/python setup.py build)
endef
define LIBINI_INSTALL_PYTHON
	(cd $(@D)/python ; $(HOST_DIR)/usr/bin/python setup.py install --prefix=$(TARGET_DIR)/usr)
endef

ifeq ($(BR2_PACKAGE_PYTHON),y)
	LIBINI_DEPENDENCIES += python host-python
	LIBINI_POST_BUILD_HOOKS += LIBINI_BUILD_PYTHON
	LIBINI_POST_INSTALL_TARGET_HOOKS += LIBINI_INSTALL_PYTHON
endif

$(eval $(cmake-package))
