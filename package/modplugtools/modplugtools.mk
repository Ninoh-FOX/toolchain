#############################################################
#
# modplugtools
#
#############################################################
MODPLUGTOOLS_VERSION = 0.5.3
MODPLUGTOOLS_SITE = http://downloads.sourceforge.net/project/modplug-xmms/modplug-tools

MODPLUGTOOLS_DEPENDENCIES = libmodplug libao

# Only play through libao: some platforms don't have OSS support and for those
# that do, playing through libao is still going to be sufficient.
define MODPLUGTOOLS_REMOVE_MODPLUGPLAY
	rm $(TARGET_DIR)/usr/bin/modplugplay
endef
MODPLUGTOOLS_POST_INSTALL_TARGET_HOOKS += MODPLUGTOOLS_REMOVE_MODPLUGPLAY

$(eval $(autotools-package))
