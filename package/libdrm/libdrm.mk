################################################################################
#
# libdrm
#
################################################################################

LIBDRM_VERSION = 2.4.97
LIBDRM_SOURCE = libdrm-$(LIBDRM_VERSION).tar.bz2
LIBDRM_SITE = https://dri.freedesktop.org/libdrm
LIBDRM_LICENSE = MIT
LIBDRM_INSTALL_STAGING = YES

# patch 0003-configure-Makefile.am-use-pkg-config-to-discover-lib.patch
# touching configure.ac/Makefile.am (and host-xutil_util-macros dependency)
LIBDRM_AUTORECONF = YES

LIBDRM_DEPENDENCIES = \
	libpthread-stubs \
	host-pkgconf \
	host-xutil_util-macros

LIBDRM_CONF_OPT = \
	--disable-cairo-tests \
	--disable-manpages
	
LIBDRM_CONF_OPT += --disable-pie --disable-staticpic

LIBDRM_CONF_ENV = ac_cv_prog_cc_c99='-std=gnu99'

ifeq ($(BR2_PACKAGE_LIBATOMIC_OPS),y)
LIBDRM_DEPENDENCIES += libatomic_ops
ifeq ($(BR2_sparc_v8)$(BR2_sparc_leon3),y)
LIBDRM_CONF_ENV += CFLAGS="$(TARGET_CFLAGS) -DAO_NO_SPARC_V9"
endif
endif

ifeq ($(BR2_PACKAGE_LIBDRM_INTEL),y)
LIBDRM_CONF_OPT += --enable-intel
LIBDRM_DEPENDENCIES += libpciaccess
else
LIBDRM_CONF_OPT += --disable-intel
endif

ifeq ($(BR2_PACKAGE_LIBDRM_RADEON),y)
LIBDRM_CONF_OPT += --enable-radeon
else
LIBDRM_CONF_OPT += --disable-radeon
endif

ifeq ($(BR2_PACKAGE_LIBDRM_AMDGPU),y)
LIBDRM_CONF_OPT += --enable-amdgpu
else
LIBDRM_CONF_OPT += --disable-amdgpu
endif

ifeq ($(BR2_PACKAGE_LIBDRM_NOUVEAU),y)
LIBDRM_CONF_OPT += --enable-nouveau
else
LIBDRM_CONF_OPT += --disable-nouveau
endif

ifeq ($(BR2_PACKAGE_LIBDRM_VMWGFX),y)
LIBDRM_CONF_OPT += --enable-vmwgfx
else
LIBDRM_CONF_OPT += --disable-vmwgfx
endif

ifeq ($(BR2_PACKAGE_LIBDRM_OMAP),y)
LIBDRM_CONF_OPT += --enable-omap-experimental-api
else
LIBDRM_CONF_OPT += --disable-omap-experimental-api
endif

ifeq ($(BR2_PACKAGE_LIBDRM_ETNAVIV),y)
LIBDRM_CONF_OPT += --enable-etnaviv-experimental-api
else
LIBDRM_CONF_OPT += --disable-etnaviv-experimental-api
endif

ifeq ($(BR2_PACKAGE_LIBDRM_EXYNOS),y)
LIBDRM_CONF_OPT += --enable-exynos-experimental-api
else
LIBDRM_CONF_OPT += --disable-exynos-experimental-api
endif

ifeq ($(BR2_PACKAGE_LIBDRM_FREEDRENO),y)
LIBDRM_CONF_OPT += --enable-freedreno
else
LIBDRM_CONF_OPT += --disable-freedreno
endif

ifeq ($(BR2_PACKAGE_LIBDRM_TEGRA),y)
LIBDRM_CONF_OPT += --enable-tegra-experimental-api
else
LIBDRM_CONF_OPT += --disable-tegra-experimental-api
endif

ifeq ($(BR2_PACKAGE_LIBDRM_VC4),y)
LIBDRM_CONF_OPT += --enable-vc4
else
LIBDRM_CONF_OPT += --disable-vc4
endif

ifeq ($(BR2_PACKAGE_HAS_UDEV),y)
LIBDRM_CONF_OPT += --enable-udev
LIBDRM_DEPENDENCIES += udev
else
LIBDRM_CONF_OPT += --disable-udev
endif

ifeq ($(BR2_PACKAGE_VALGRIND),y)
LIBDRM_CONF_OPT += --enable-valgrind
LIBDRM_DEPENDENCIES += valgrind
else
LIBDRM_CONF_OPT += --disable-valgrind
endif

ifeq ($(BR2_PACKAGE_LIBDRM_INSTALL_TESTS),y)
LIBDRM_CONF_OPT += --enable-install-test-programs
ifeq ($(BR2_PACKAGE_CUNIT),y)
LIBDRM_DEPENDENCIES += cunit
endif
endif

$(eval $(autotools-package))
