################################################################################
#
# linux-console-tools
#
################################################################################

LINUX_CONSOLE_TOOLS_VERSION = 1.4.7
LINUX_CONSOLE_TOOLS_SOURCE = linuxconsoletools-$(LINUX_CONSOLE_TOOLS_VERSION).tar.bz2
LINUX_CONSOLE_TOOLS_SITE = http://downloads.sourceforge.net/project/linuxconsole
LINUX_CONSOLE_TOOLS_LICENSE = GPLv2+
LINUX_CONSOLE_TOOLS_LICENSE_FILES = COPYING


LINUX_CONSOLE_TOOLS_PROGRAMS =

ifeq ($(BR2_PACKAGE_LINUX_CONSOLE_TOOLS_JOY),y)
LINUX_CONSOLE_TOOLS_PROGRAMS += jscal jscal-restore jscal-store jstest
endif

ifeq ($(BR2_PACKAGE_LINUX_CONSOLE_TOOLS_FF),y)
LINUX_CONSOLE_TOOLS_PROGRAMS += ffcfstress ffset fftest
ifeq ($(BR2_PACKAGE_SDL),y)
LINUX_CONSOLE_TOOLS_PROGRAMS += ffmvforce
LINUX_CONSOLE_TOOLS_DEPENDENCIES += sdl
endif
ifeq ($(BR2_PACKAGE_SDL_COMPAT),y)
LINUX_CONSOLE_TOOLS_DEPENDENCIES += sdl_compat
endif
endif

ifeq ($(BR2_PACKAGE_LINUX_CONSOLE_TOOLS_INPUTATTACH),y)
LINUX_CONSOLE_TOOLS_PROGRAMS += inputattach
endif


define LINUX_CONSOLE_TOOLS_BUILD_CMDS
$(MAKE) PREFIX=/usr PROGRAMS="$(LINUX_CONSOLE_TOOLS_PROGRAMS)" \
	CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define LINUX_CONSOLE_TOOLS_INSTALL_TARGET_CMDS
$(MAKE) PREFIX=/usr PROGRAMS="$(LINUX_CONSOLE_TOOLS_PROGRAMS)" \
	DESTDIR=$(TARGET_DIR) -C $(@D) install
endef

$(eval $(generic-package))
