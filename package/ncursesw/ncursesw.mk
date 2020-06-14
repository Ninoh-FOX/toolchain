################################################################################
#
# ncursesw
#
################################################################################

NCURSESW_VERSION = 5.9+20140913
NCURSESW_SOURCE = ncurses_$(NCURSESW_VERSION).orig.tar.gz
NCURSESW_SITE = http://deb.debian.org/debian/pool/main/n/ncurses
NCURSESW_INSTALL_STAGING = YES
NCURSESW_DEPENDENCIES = host-ncurses
HOST_NCURSESW_DEPENDENCIES =
NCURSESW_PROGS = clear infocmp tabs tic toe tput tset
NCURSESW_LICENSE = MIT with advertising clause
NCURSESW_LICENSE_FILES = README
NCURSESW_CONFIG_SCRIPTS = ncurses5-config

NCURSESW_CONF_OPT = \
	$(if $(BR2_PREFER_STATIC_LIB),--without-shared,--with-shared) \
	--without-cxx \
	--without-cxx-binding \
	--without-ada \
	--without-tests \
	--disable-big-core \
	--without-profile \
	--disable-rpath \
	--disable-rpath-hack \
	--enable-echo \
	--enable-const \
	--enable-overwrite \
	--enable-pc-files \
	--enable-widec \
	$(if $(BR2_PACKAGE_NCURSESW_TARGET_PROGS),,--without-progs) \
	--without-manpages

# Install after busybox for the full-blown versions
ifeq ($(BR2_PACKAGE_BUSYBOX),y)
	NCURSESW_DEPENDENCIES += busybox
endif

NCURSESW_LIBS-y = libncursesw
NCURSESW_LIBS-$(BR2_PACKAGE_NCURSESW_TARGET_MENU) += libmenuw
NCURSESW_LIBS-$(BR2_PACKAGE_NCURSESW_TARGET_PANEL) += libpanelw
NCURSESW_LIBS-$(BR2_PACKAGE_NCURSESW_TARGET_FORM) += libformw

ifneq ($(BR2_ENABLE_DEBUG),y)
NCURSESW_CONF_OPT += --without-debug
endif

# ncurses breaks with parallel build, but takes quite a while to
# build single threaded. Work around it similar to how Gentoo does
define NCURSESW_BUILD_CMDS
	$(MAKE1) -C $(@D) DESTDIR=$(STAGING_DIR) sources
	rm -rf $(@D)/misc/pc-files
	$(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR)
endef

ifneq ($(BR2_PREFER_STATIC_LIB),y)
define NCURSESW_INSTALL_TARGET_LIBS
	for lib in $(NCURSESW_LIBS-y); do \
		cp -dpf $(NCURSESW_DIR)/lib/$${lib}.so* $(TARGET_DIR)/usr/lib/; \
	done
endef
endif

ifeq ($(BR2_PACKAGE_NCURSESW_TARGET_PROGS),y)
define NCURSESW_INSTALL_TARGET_PROGS
	for x in $(NCURSESW_PROGS); do \
		$(INSTALL) -m 0755 $(NCURSESW_DIR)/progs/$$x \
			$(TARGET_DIR)/usr/bin/$$x; \
	done
	ln -sf tset $(TARGET_DIR)/usr/bin/reset
endef
endif

define NCURSESW_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib
	$(NCURSESW_INSTALL_TARGET_LIBS)
	$(NCURSESW_INSTALL_TARGET_PROGS)
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt100-w $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt102-w $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt200-w $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt220-w $(TARGET_DIR)/usr/share/terminfo/v
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/a
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/a/ansiw $(TARGET_DIR)/usr/share/terminfo/a
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/s
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/s/screen-w $(TARGET_DIR)/usr/share/terminfo/s

endef # NCURSESW_INSTALL_TARGET_CMDS

#
# On systems with an older version of tic, the installation of ncurses hangs
# forever. To resolve the problem, build a static version of tic on host
# ourselves, and use that during installation.
#
define HOST_NCURSESW_BUILD_CMDS
	$(MAKE1) -C $(@D) sources
	$(MAKE) -C $(@D)/progs tic
endef

HOST_NCURSESW_CONF_OPT = \
	--with-shared --without-gpm \
	--without-manpages \
	--without-cxx \
	--without-cxx-binding \
	--without-ada

$(eval $(autotools-package))
$(eval $(host-autotools-package))