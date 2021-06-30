################################################################################
#
# sdl2
#
################################################################################

SDL2_VERSION = release-2.0.12
SDL2_SITE = $(call github,pcercuei,SDL,$(SDL2_VERSION))
SDL2_LICENSE = zlib
SDL2_LICENSE_FILES = COPYING.txt
SDL2_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_MESA3D),y)
LIBGLES_DEPENDENCIES = mesa3d
endif

ifeq ($(BR2_PACKAGE_MESA3D_ETNA_VIV),y)
LIBGLES_DEPENDENCIES = mesa3d-etna_viv
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
SDL2_DEPENDENCIES += $(LIBGLES_DEPENDENCIES)
SDL2_CONF_OPT += --enable-video-opengles
else
SDL2_CONF_OPT += --disable-video-opengles
endif

ifeq ($(BR2_PACKAGE_HAS_LIBEGL),y)
SDL2_DEPENDENCIES += $(LIBEGL_DEPENDENCIES)
endif

SDL2_CONF_OPT += --enable-video-fbdev

# uses gnu99 specific features
SDL2_CONF_ENV += CFLAGS="$(TARGET_CFLAGS) -std=gnu99"

# Note: SDL2 looks for X11 headers in host dirs, so if you want to build SDL2
#       with X11 support, better make it safe for cross compilation first.
SDL2_CONF_OPT += --disable-video-x11

ifeq ($(BR2_PACKAGE_DIRECTFB),y)
SDL2_DEPENDENCIES += directfb
SDL2_CONF_OPT += --enable-video-directfb
SDL2_CONF_ENV += ac_cv_path_DIRECTFBCONFIG=$(STAGING_DIR)/usr/bin/directfb-config
else
SDL2_CONF_OPT += --disable-video-directfb
endif

ifeq ($(BR2_PACKAGE_ALSA_LIB),y)
SDL2_DEPENDENCIES += alsa-lib
SDL2_CONF_OPT += --enable-alsa --disable-oss
else
SDL2_CONF_OPT += --disable-alsa
endif

ifeq ($(BR2_PACKAGE_PULSEAUDIO),y)
SDL2_DEPENDENCIES += pulseaudio
SDL2_CONF_OPT += --enable-pulseaudio
else
SDL2_CONF_OPT += --disable-pulseaudio
endif

SDL2_CONF_OPT += --disable-esd

ifeq ($(BR2_PACKAGE_HAS_UDEV),y)
SDL2_CONF_OPT += --enable-libudev
else
SDL2_CONF_OPT += --disable-libudev
endif

ifeq ($(BR2_PACKAGE_DBUS),y)
SDL2_CONF_OPT += --enable-dbus
else
SDL2_CONF_OPT += --disable-dbus
endif

ifeq ($(BR2_PACKAGE_SDL2_KMSDRM),y)
SDL2_DEPENDENCIES += libdrm libgbm
SDL2_CONF_OPT += --enable-video-kmsdrm
else
SDL2_CONF_OPT += --disable-video-kmsdrm
endif

# OpenDingux hack: We've got tslib to make porting easier, but we've got no
#                  touch screen, so having SDL try to use tslib is pointless.
# ifeq ($(BR2_PACKAGE_TSLIB),y)
# SDL2_DEPENDENCIES += tslib
# SDL2_CONF_OPT += --enable-input-tslib
# else
SDL2_CONF_OPT += --disable-input-tslib
# endif

SDL2_CONF_OPT += --disable-rpath

define SDL2_FIXUP_SDL2_CONFIG
	$(SED) 's%^prefix=.*%prefix=$(STAGING_DIR)/usr%' \
		$(STAGING_DIR)/usr/bin/sdl2-config
	$(SED) 's%^exec_prefix=.*%exec_prefix=$${prefix}%' \
		$(STAGING_DIR)/usr/bin/sdl2-config
endef

define SDL2_REMOVE_SDL2_CONFIG
	rm $(TARGET_DIR)/usr/bin/sdl2-config
endef

SDL2_POST_INSTALL_TARGET_HOOKS += SDL2_REMOVE_SDL2_CONFIG
SDL2_POST_INSTALL_STAGING_HOOKS += SDL2_FIXUP_SDL2_CONFIG

$(eval $(autotools-package))
