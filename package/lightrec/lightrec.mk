################################################################################
#
# lightrec
#
################################################################################

LIGHTREC_VERSION = v0.4
LIGHTREC_SITE = $(call github,pcercuei,lightrec,$(LIGHTREC_VERSION))
LIGHTREC_INSTALL_STAGING = YES

$(eval $(cmake-package))