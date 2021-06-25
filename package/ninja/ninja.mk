################################################################################
#
# ninja
#
################################################################################

NINJA_VERSION = 1.9.0
NINJA_SITE = $(call github,ninja-build,ninja,v$(NINJA_VERSION))
NINJA_LICENSE = Apache-2.0
NINJA_LICENSE_FILES = COPYING
NINJA_CONF_OPT = --bootstrap

define HOST_NINJA_INSTALL_CMDS
	cd $(@D); ./configure.py $(NINJA_CONF_OPT)
	$(INSTALL) -m 0755 -D $(@D)/ninja $(HOST_DIR)/usr/bin/ninja
endef

$(eval $(generic-package))
$(eval $(host-generic-package))
