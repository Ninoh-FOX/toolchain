################################################################################
#
# ninja
#
################################################################################

NINJA_VERSION = 1.10.2
NINJA_SITE = $(call github,ninja-build,ninja,v$(NINJA_VERSION))
NINJA_LICENSE = Apache-2.0
NINJA_LICENSE_FILES = COPYING
NINJA_CONF_OPT = --bootstrap

NINJA_DEPENDENCIES += host-python3

define HOST_NINJA_CONFIGURE_CMDS
	cd $(@D); ./configure.py $(NINJA_CONF_OPT)
endef

define HOST_NINJA_INSTALL_CMDS
	$(INSTALL) -m 0755 -D $(@D)/ninja $(HOST_DIR)/usr/bin/ninja
	$(INSTALL) -m 0755 -D $(@D)/build.ninja $(HOST_DIR)/usr/bin/build.ninja
endef

NINJA_PRE_CONFIGURE_HOOKS = HOST_NINJA_CONFIGURE_CMDS
NINJA_POST_INSTALL_HOOKS = HOST_NINJA_INSTALL_CMDS

$(eval $(host-generic-package))
