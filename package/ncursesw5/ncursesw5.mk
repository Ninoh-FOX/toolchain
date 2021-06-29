################################################################################
#
# ncursesw5
#
################################################################################

NCURSESW5_VERSION = 5.9+20140913
NCURSESW5_SOURCE = ncurses_$(NCURSESW5_VERSION).orig.tar.gz
NCURSESW5_SITE = http://deb.debian.org/debian/pool/main/n/ncurses
NCURSESW5_INSTALL_STAGING = YES
NCURSESW5_DEPENDENCIES = host-ncurses
HOST_NCURSESW5_DEPENDENCIES =
NCURSESW5_PROGS = clear infocmp tabs tic toe tput tset
NCURSESW5_LICENSE = MIT with advertising clause
NCURSESW5_LICENSE_FILES = README
NCURSESW5_CONFIG_SCRIPTS = ncursesw5-config

NCURSESW5_CONF_OPT = \
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
	$(if $(BR2_PACKAGE_NCURSESW5_TARGET_PROGS),,--without-progs) \
	--without-manpages

# Install after busybox for the full-blown versions
ifeq ($(BR2_PACKAGE_BUSYBOX),y)
	NCURSESW5_DEPENDENCIES += busybox
endif

NCURSESW5_LIBS-y = libncursesw
NCURSESW5_LIBS-$(BR2_PACKAGE_NCURSESW5_TARGET_MENU) += libmenuw
NCURSESW5_LIBS-$(BR2_PACKAGE_NCURSESW5_TARGET_PANEL) += libpanelw
NCURSESW5_LIBS-$(BR2_PACKAGE_NCURSESW5_TARGET_FORM) += libformw

ifneq ($(BR2_ENABLE_DEBUG),y)
NCURSESW5_CONF_OPT += --without-debug
endif

# ncurses breaks with parallel build, but takes quite a while to
# build single threaded. Work around it similar to how Gentoo does
define NCURSESW5_BUILD_CMDS
	$(MAKE1) -C $(@D) DESTDIR=$(STAGING_DIR) sources
	rm -rf $(@D)/misc/pc-files
	$(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR)
endef

ifneq ($(BR2_PREFER_STATIC_LIB),y)
define NCURSESW5_INSTALL_TARGET_LIBS
	for lib in $(NCURSESW5_LIBS-y); do \
		cp -dpf $(NCURSESW5_DIR)/lib/$${lib}.so* $(TARGET_DIR)/usr/lib/; \
	done
endef
endif

ifeq ($(BR2_PACKAGE_NCURSESW5_TARGET_PROGS),y)
define NCURSESW5_INSTALL_TARGET_PROGS
	for x in $(NCURSESW5_PROGS); do \
		$(INSTALL) -m 0755 $(NCURSESW5_DIR)/progs/$$x \
			$(TARGET_DIR)/usr/bin/$$x; \
	done
	ln -sf tset $(TARGET_DIR)/usr/bin/reset
endef
endif

define NCURSESW5_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib
	$(NCURSESW5_INSTALL_TARGET_LIBS)
	$(NCURSESW5_INSTALL_TARGET_PROGS)
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt100-w $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt102-w $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt200-w $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt220-w $(TARGET_DIR)/usr/share/terminfo/v
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/a
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/a/ansiw $(TARGET_DIR)/usr/share/terminfo/a
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/s
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/s/screen-w $(TARGET_DIR)/usr/share/terminfo/s

endef # NCURSESW5_INSTALL_TARGET_CMDS

#
# On systems with an older version of tic, the installation of ncurses hangs
# forever. To resolve the problem, build a static version of tic on host
# ourselves, and use that during installation.
#
define HOST_NCURSESW5_BUILD_CMDS
	$(MAKE1) -C $(@D) sources
	$(MAKE) -C $(@D)/progs tic
endef

HOST_NCURSESW5_CONF_OPT = \
	--with-shared --without-gpm \
	--without-manpages \
	--without-cxx \
	--without-cxx-binding \
	--without-ada

$(eval $(autotools-package))
$(eval $(host-autotools-package))