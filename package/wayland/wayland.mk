################################################################################
#
# wayland
#
################################################################################

WAYLAND_VERSION = 1.5.0
WAYLAND_SITE = http://wayland.freedesktop.org/releases
WAYLAND_SOURCE = wayland-$(WAYLAND_VERSION).tar.xz
WAYLAND_LICENSE = MIT
WAYLAND_LICENSE_FILES = COPYING

WAYLAND_INSTALL_STAGING = YES
WAYLAND_DEPENDENCIES = libffi host-pkgconf host-wayland expat

# wayland-scanner is only needed for building, not on the target
WAYLAND_CONF_OPT = --disable-scanner

# We must provide a specialy-crafted wayland-scanner .pc file
# which we vampirise and adapt from the host-wayland copy
define WAYLAND_SCANNER_PC
	$(INSTALL) -m 0644 -D $(HOST_DIR)/usr/lib/pkgconfig/wayland-scanner.pc \
	              $(STAGING_DIR)/usr/lib/pkgconfig/wayland-scanner.pc
	$(SED) 's:^prefix=.*:prefix=/usr:' \
	    -e 's:^wayland_scanner=.*:wayland_scanner=$(HOST_DIR)/usr/bin/wayland-scanner:' \
	    $(STAGING_DIR)/usr/lib/pkgconfig/wayland-scanner.pc
endef

define WAYLAND_SERVER_PC
	$(INSTALL) -m 0644 -D $(HOST_DIR)/usr/lib/pkgconfig/wayland-server.pc \
	              $(STAGING_DIR)/usr/lib/pkgconfig/wayland-server.pc
	$(SED) 's:^prefix=.*:prefix=/usr:' \
	    -e 's:^wayland_server=.*:wayland_server=$(HOST_DIR)/usr/bin/wayland-server:' \
	    $(STAGING_DIR)/usr/lib/pkgconfig/wayland-server.pc
endef

define WAYLAND_CLIENT_PC
	$(INSTALL) -m 0644 -D $(HOST_DIR)/usr/lib/pkgconfig/wayland-client.pc \
	              $(STAGING_DIR)/usr/lib/pkgconfig/wayland-client.pc
	$(SED) 's:^prefix=.*:prefix=/usr:' \
	    -e 's:^wayland_client=.*:wayland_client=$(HOST_DIR)/usr/bin/wayland-client:' \
	    $(STAGING_DIR)/usr/lib/pkgconfig/wayland-client.pc
endef

WAYLAND_POST_INSTALL_STAGING_HOOKS += \
	WAYLAND_SCANNER_PC \
	WAYLAND_SERVER_PC \
	WAYLAND_CLIENT_PC

# Remove the DTD from the target, it's not needed at runtime
define WAYLAND_TARGET_CLEANUP
	rm -rf $(TARGET_DIR)/usr/share/wayland
endef
WAYLAND_POST_INSTALL_TARGET_HOOKS += WAYLAND_TARGET_CLEANUP

$(eval $(autotools-package))
$(eval $(host-autotools-package))
