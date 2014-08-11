#############################################################
#
# p7zip
#
#############################################################
P7ZIP_VERSION = 9.20.1
P7ZIP_SOURCE = p7zip_$(P7ZIP_VERSION)_src_all.tar.bz2
P7ZIP_SITE = http://downloads.sourceforge.net/project/p7zip/p7zip/$(P7ZIP_VERSION)
P7ZIP_LICENSE = LGPLv2.1+

define P7ZIP_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" BR_CFLAGS="$(TARGET_CFLAGS)" \
		CXX="$(TARGET_CXX)" BR_CXXFLAGS="$(TARGET_CXXFLAGS)" \
		-C $(@D) 7zr
endef

define P7ZIP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/bin/7zr $(TARGET_DIR)/usr/bin/7zr
endef

$(eval $(generic-package))
