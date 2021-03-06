################################################################################
#
# htop
#
################################################################################

HTOP_VERSION = 3.0.4
HTOP_SOURCE = $(HTOP_VERSION).tar.gz
HTOP_SITE = https://github.com/htop-dev/htop/archive
HTOP_DEPENDENCIES = ncurses
# Prevent htop build system from searching the host paths
HTOP_CONF_ENV = HTOP_NCURSES_CONFIG_SCRIPT=$(STAGING_DIR)/usr/bin/$(NCURSES_CONFIG_SCRIPTS)
HTOP_LICENSE = GPL-2.0
HTOP_LICENSE_FILES = COPYING

HTOP_AUTORECONF = YES

define HTOP_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef
HTOP_CONFIGURE_HOOKS += HTOP_RUN_AUTOGEN

ifeq ($(BR2_PACKAGE_NCURSES_WCHAR),y)
HTOP_CONF_OPT += --enable-unicode
else
HTOP_CONF_OPT += --disable-unicode
endif

# ARC uses an old uClibc that needs dladdr() for backtrace support,
# which doesn't work for static only scenario, so as a workaround, we
# pretend that execinfo.h is not available.
ifeq ($(BR2_arc)$(BR2_PREFER_STATIC_LIB),yy)
HTOP_CONF_ENV += ac_cv_header_execinfo_h=no
endif

$(eval $(autotools-package))
