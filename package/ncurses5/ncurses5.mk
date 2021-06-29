################################################################################
#
# ncurses5
#
################################################################################

NCURSES5_VERSION = 5.9
NCURSES5_SOURCE = ncurses-$(NCURSES5_VERSION).tar.gz
NCURSES5_SITE = $(BR2_GNU_MIRROR)/ncurses
NCURSES5_INSTALL_STAGING = YES
NCURSES5_DEPENDENCIES = host-ncurses
HOST_NCURSES5_DEPENDENCIES =
NCURSES5_PROGS = clear infocmp tabs tic toe tput tset
NCURSES5_LICENSE = MIT with advertising clause
NCURSES5_LICENSE_FILES = README
NCURSES5_CONFIG_SCRIPTS = ncurses5-config

NCURSES5_CONF_OPT = \
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
	$(if $(BR2_PACKAGE_NCURSES5_TARGET_PROGS),,--without-progs) \
	--without-manpages

# Install after busybox for the full-blown versions
ifeq ($(BR2_PACKAGE_BUSYBOX),y)
	NCURSES5_DEPENDENCIES += busybox
endif

NCURSES5_LIBS-y = libncurses
NCURSES5_LIBS-$(BR2_PACKAGE_NCURSES5_TARGET_MENU) += libmenu
NCURSES5_LIBS-$(BR2_PACKAGE_NCURSES5_TARGET_PANEL) += libpanel
NCURSES5_LIBS-$(BR2_PACKAGE_NCURSES5_TARGET_FORM) += libform

ifneq ($(BR2_ENABLE_DEBUG),y)
NCURSES5_CONF_OPT += --without-debug
endif

# ncurses breaks with parallel build, but takes quite a while to
# build single threaded. Work around it similar to how Gentoo does
define NCURSES5_BUILD_CMDS
	$(MAKE1) -C $(@D) DESTDIR=$(STAGING_DIR) sources
	rm -rf $(@D)/misc/pc-files
	$(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR)
endef

ifneq ($(BR2_PREFER_STATIC_LIB),y)
define NCURSES5_INSTALL_TARGET_LIBS
	for lib in $(NCURSES5_LIBS-y); do \
		cp -dpf $(NCURSES5_DIR)/lib/$${lib}.so* $(TARGET_DIR)/usr/lib/; \
	done
endef
endif

ifeq ($(BR2_PACKAGE_NCURSES5_TARGET_PROGS),y)
define NCURSES5_INSTALL_TARGET_PROGS
	for x in $(NCURSES5_PROGS); do \
		$(INSTALL) -m 0755 $(NCURSES5_DIR)/progs/$$x \
			$(TARGET_DIR)/usr/bin/$$x; \
	done
	ln -sf tset $(TARGET_DIR)/usr/bin/reset
endef
endif

define NCURSES5_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib
	$(NCURSES5_INSTALL_TARGET_LIBS)
	$(NCURSES5_INSTALL_TARGET_PROGS)
	ln -snf /usr/share/terminfo $(TARGET_DIR)/usr/lib/terminfo
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/x
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/x/xterm $(TARGET_DIR)/usr/share/terminfo/x
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/x/xterm-color $(TARGET_DIR)/usr/share/terminfo/x
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/x/xterm+256color $(TARGET_DIR)/usr/share/terminfo/x
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/x/xterm-256color $(TARGET_DIR)/usr/share/terminfo/x
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/x/xterm-xfree86 $(TARGET_DIR)/usr/share/terminfo/x
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt100 $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt100-putty $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt102 $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt200 $(TARGET_DIR)/usr/share/terminfo/v
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/v/vt220 $(TARGET_DIR)/usr/share/terminfo/v
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/a
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/a/ansi $(TARGET_DIR)/usr/share/terminfo/a
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/l
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/l/linux $(TARGET_DIR)/usr/share/terminfo/l
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/s
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/s/screen $(TARGET_DIR)/usr/share/terminfo/s
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/s/screen-256color $(TARGET_DIR)/usr/share/terminfo/s
	mkdir -p $(TARGET_DIR)/usr/share/terminfo/p
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/p/putty $(TARGET_DIR)/usr/share/terminfo/p
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/p/putty-256color $(TARGET_DIR)/usr/share/terminfo/p
	cp -dpf $(STAGING_DIR)/usr/share/terminfo/p/putty-vt100 $(TARGET_DIR)/usr/share/terminfo/p
endef # NCURSES5_INSTALL_TARGET_CMDS

#
# On systems with an older version of tic, the installation of ncurses hangs
# forever. To resolve the problem, build a static version of tic on host
# ourselves, and use that during installation.
#
define HOST_NCURSES5_BUILD_CMDS
	$(MAKE1) -C $(@D) sources
	$(MAKE) -C $(@D)/progs tic
endef

HOST_NCURSES5_CONF_OPT = \
	--with-shared --without-gpm \
	--without-manpages \
	--without-cxx \
	--without-cxx-binding \
	--without-ada

$(eval $(autotools-package))
$(eval $(host-autotools-package))