################################################################################
#
# python3-setuptools
#
################################################################################

PYTHON3_SETUPTOOLS_VERSION = 44.0.0
PYTHON3_SETUPTOOLS_SOURCE = v$(PYTHON3_SETUPTOOLS_VERSION).tar.gz
PYTHON3_SETUPTOOLS_SITE = $(call github,pypa,setuptools,$(PYTHON3_SETUPTOOLS_VERSION))
PYTHON3_SETUPTOOLS_LICENSE = Python Software Foundation or Zope Public License
PYTHON3_SETUPTOOLS_LICENSE_FILES = PKG-INFO
PYTHON3_SETUPTOOLS_SETUP_TYPE = setuptools
HOST_PYTHON3_SETUPTOOLS_DL_SUBDIR = python-setuptools
HOST_PYTHON3_SETUPTOOLS_NEEDS_HOST_PYTHON = python3

$(eval $(python-package))
$(eval $(host-python-package))
