################################################################################
#
## hocoslamfy
#
#################################################################################
HOCOSLAMFY_VERSION = f60f367938321f0bd5cf355c5721905bc5c41812
HOCOSLAMFY_SITE = $(call github,jamesofarrell,hocoslamfy,$(HOCOSLAMFY_VERSION))
HOCOSLAMFY_AUTORECONF = YES
HOCOSLAMFY_INSTALL_STAGING = YES

CROSS_COMPILE=$(TARGET_CROSS)

define HOCOSLAMFY_BUILD_CMDS
	    $(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" -C $(@D) 
	    $(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" -C $(@D) opk
endef

define HOCOSLAMFY_INSTALL_OPK_IMAGES
    (   cd $(@D) ; \
        mkdir -p $(BINARIES_DIR)/opks ; \
        cp -vf hocoslamfy-od.opk $(BINARIES_DIR)/opks )
endef
HOCOSLAMFY_POST_INSTALL_TARGET_HOOKS += HOCOSLAMFY_INSTALL_OPK_IMAGES


$(eval $(generic-package))
