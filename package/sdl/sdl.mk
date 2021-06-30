################################################################################
#
# sdl
#
################################################################################

SDL_VERSION = 3da6a28e
SDL_SITE = $(call github,OpenDingux,SDL,$(SDL_VERSION))
SDL_LICENSE = LGPL-2.1+
SDL_LICENSE_FILES = COPYING
SDL_CPE_ID_VENDOR = libsdl
SDL_CPE_ID_PRODUCT = simple_directmedia_layer
SDL_INSTALL_STAGING = YES

# we're patching configure.in, but package cannot autoreconf with our version of
# autotools, so we have to do it manually instead of setting SDL_AUTORECONF = YES
define SDL_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef

SDL_PRE_CONFIGURE_HOOKS += SDL_RUN_AUTOGEN
HOST_SDL_PRE_CONFIGURE_HOOKS += SDL_RUN_AUTOGEN

SDL_DEPENDENCIES += host-automake host-autoconf host-libtool libao
HOST_SDL_DEPENDENCIES += host-automake host-autoconf host-libtool

SDL_CONF_OPT += --enable-video-qtopia=no

ifeq ($(BR2_PACKAGE_SDL_FBCON),y)
SDL_CONF_OPT += --enable-video-fbcon=yes
else
SDL_CONF_OPT += --enable-video-fbcon=no
endif

ifeq ($(BR2_PACKAGE_SDL_DIRECTFB),y)
SDL_DEPENDENCIES += directfb
SDL_CONF_OPT += --enable-video-directfb=yes
SDL_CONF_ENV = ac_cv_path_DIRECTFBCONFIG=$(STAGING_DIR)/usr/bin/directfb-config
else
SDL_CONF_OPT += --enable-video-directfb=no
endif

ifeq ($(BR2_PACKAGE_LIBDRM),y)
SDL_DEPENDENCIES += libdrm libgbm
SDL_CONF_OPT += --enable-video-kmsdrm=yes
else
SDL_CONF_OPT += --enable-video-kmsdrm=no
endif

ifeq ($(BR2_PACKAGE_SDL_X11),y)
SDL_CONF_OPT += --enable-video-x11=yes
SDL_DEPENDENCIES += \
	xlib_libX11 xlib_libXext \
	$(if $(BR2_PACKAGE_XLIB_LIBXRENDER), xlib_libXrender) \
	$(if $(BR2_PACKAGE_XLIB_LIBXRANDR), xlib_libXrandr)
else
SDL_CONF_OPT += --enable-video-x11=no
endif

ifneq ($(BR2_USE_MMU),y)
SDL_CONF_OPT += --enable-dga=no
endif

# overwrite autodection (prevents confusion with host libpth version)
ifeq ($(BR2_PACKAGE_LIBPTHSEM_COMPAT),y)
SDL_CONF_OPT += --enable-pth=yes
SDL_CONF_ENV += ac_cv_path_PTH_CONFIG=$(STAGING_DIR)/usr/bin/pth-config
SDL_DEPENDENCIES += libpthsem
else
SDL_CONF_OPT += --enable-pth=no
endif

ifeq ($(BR2_PACKAGE_HAS_UDEV),y)
SDL_CONF_OPT += --enable-libudev=yes
else
SDL_CONF_OPT += --enable-libudev=no
endif

ifeq ($(BR2_PACKAGE_TSLIB),y)
SDL_DEPENDENCIES += tslib
endif

ifeq ($(BR2_PACKAGE_ALSA_LIB),y)
SDL_DEPENDENCIES += alsa-lib
endif

ifeq ($(BR2_PACKAGE_MESA3D),y)
SDL_DEPENDENCIES += mesa3d
endif

ifeq ($(BR2_PACKAGE_MESA3D_ETNA_VIV),y)
SDL_DEPENDENCIES += mesa3d-etna_viv
endif

SDL_CONF_OPT += \
	--enable-rpath=no \
	--enable-pulseaudio=no \
	--enable-arts=no \
	--enable-esd=no \
	--enable-nasm=no \
	--enable-video-ps3=no

HOST_SDL_CONF_OPT += \
	--enable-pulseaudio=no \
	--enable-video-x11=no \
	--enable-arts=no \
	--enable-esd=no \
	--enable-nasm=no \
	--enable-video-ps3=no

SDL_CONFIG_SCRIPTS = sdl-config

$(eval $(autotools-package))
$(eval $(host-autotools-package))
