################################################################################
#
# mikmod
#
################################################################################
MIKMOD_VERSION = 3.3.11.1
MIKMOD_SITE = $(call github,sezero,mikmod,libmikmod-$(MIKMOD_VERSION))
MIKMOD_INSTALL_STAGING = YES
MIKMOD_AUTORECONF = YES

define MIKMOD_FOLDER
	rm -rf $(@D)/libmikmod
	rm -rf $(@D)/old-mikmod
	rm -rf $(@D)/unimod
	cp -r $(@D)/mikmod/* $(@D)/
endef

MIKMOD_POST_PATCH_HOOKS += MIKMOD_FOLDER

$(eval $(autotools-package))