################################################################################
#
# libmikmod
#
################################################################################
LIBMIKMOD_VERSION = 3.3.11.1
LIBMIKMOD_SITE = $(call github,sezero,mikmod,libmikmod-$(LIBMIKMOD_VERSION))
LIBMIKMOD_INSTALL_STAGING = YES
LIBMIKMOD_AUTORECONF = YES

define LIBMIKMOD_LIB_FOLDER
	rm -rf $(@D)/mikmod
	rm -rf $(@D)/old-mikmod
	rm -rf $(@D)/unimod
	cp -r $(@D)/libmikmod/* $(@D)/
endef

LIBMIKMOD_POST_PATCH_HOOKS += LIBMIKMOD_LIB_FOLDER

$(eval $(autotools-package))