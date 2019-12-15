#############################################################
#
# unrar
#
#############################################################

UNRAR_VERSION = 5.0.7
UNRAR_SOURCE = unrarsrc-$(UNRAR_VERSION).tar.gz
UNRAR_SITE = ftp://ftp.rarlabs.com/rar
UNRAR_INSTALL_STAGING = YES
UNRAR_LICENSE_FILES = license.txt

UNRAR_DEFINES :=
UNRAR_LDFLAGS :=

ifeq ($(BR2_PACKAGE_UNRAR_SMP),y)
UNRAR_DEFINES += -DRAR_SMP
UNRAR_LDFLAGS += -lpthread
endif

define UNRAR_BUILD_CMDS
	$(MAKE) -C $(@D) \
		CXX="$(TARGET_CXX)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		STRIP="$(TARGET_STRIP)" DEFINES="$(UNRAR_DEFINES)"
endef

define UNRAR_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) install DESTDIR=$(TARGET_DIR)/usr
endef

define UNRAR_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(generic-package))
