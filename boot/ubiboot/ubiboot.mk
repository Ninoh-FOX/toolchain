################################################################################
#
# UbiBoot
#
################################################################################

UBIBOOT_VERSION    = master
UBIBOOT_SITE = $(call github,tonyjih,RG350_UBIBoot,$(UBIBOOT_VERSION))
UBIBOOT_BOARD_NAME = $(call qstrip,$(BR2_TARGET_UBIBOOT_BOARDNAME))

UBIBOOT_LICENSE = GPLv2+
UBIBOOT_LICENSE_FILES = README

UBIBOOT_INSTALL_IMAGES = YES

#
# Apply patches
#

ifeq ($(BR2_TARGET_UBIBOOT_CPU),"overclock")
define CPU_CLOCK_PATCH
	support/scripts/apply-patches.sh $(@D) boot/ubiboot/ overclock.patch.conditional
endef
else ifeq ($(BR2_TARGET_UBIBOOT_CPU),"stockclock")
define CPU_CLOCK_PATCH
	support/scripts/apply-patches.sh $(@D) boot/ubiboot/ stockclock.patch.conditional
endef
endif

UBIBOOT_POST_PATCH_HOOKS = CPU_CLOCK_PATCH

# end Apply patches

define UBIBOOT_BUILD_CMDS
	$(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" -C $(@D) CONFIG=$(UBIBOOT_BOARD_NAME)
endef

define UBIBOOT_INSTALL_IMAGES_CMDS
	mkdir -p $(BINARIES_DIR)/ubiboot
	$(INSTALL) -D -m 0755 $(@D)/output/$(UBIBOOT_BOARD_NAME)/* $(BINARIES_DIR)/ubiboot
endef

$(eval $(generic-package))

ifeq ($(BR2_TARGET_UBIBOOT),y)
# we NEED a board name unless we're at make source
ifeq ($(filter source,$(MAKECMDGOALS)),)
ifeq ($(UBIBOOT_BOARD_NAME),)
$(error NO UBIBoot board name set. Check your BR2_BOOT_UBIBOOT_BOARDNAME setting)
endif
endif

endif
