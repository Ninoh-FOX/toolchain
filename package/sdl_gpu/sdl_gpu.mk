################################################################################
#
# SDL_GPU
#
################################################################################

SDL_GPU_VERSION = master
SDL_GPU_SITE = $(call github,grimfang4,sdl-gpu,$(SDL_GPU_VERSION))
SDL_GPU_LICENSE = zlib
SDL_GPU_LICENSE_FILES = COPYING.txt
SDL_GPU_INSTALL_STAGING = YES

SDL_GPU_CONF_OPT +=-G "Unix Makefiles" -DUSE_SDL1=ON -DDISABLE_OPENGL=ON -DDEFAULT_BUILD_DEMOS=OFF -DBUILD_DEMOS=OFF -DBUILD_TESTS=OFF -DDEFAULT_INSTALL_LIBRARY=ON -DBUILD_VIDEO_TEST=OFF -DBUILD_TOOLS=OFF

$(eval $(cmake-package))
