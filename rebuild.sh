#!/bin/sh
set -e

# Get the latest version.
echo "Updating local git repository..."
git pull

# Use the default config and patch it to point to our install location.
make rg350_defconfig
sed -ie 's%^BR2_HOST_DIR=.*$%BR2_HOST_DIR="/opt/gcw0-toolchain"%' .config

# Clear the install location.
echo "Clearing install location..."
rm -rf /opt/gcw0-toolchain/*

# Clear the build location.
echo "Clearing build location..."
rm -rf output/

# Remove non-versioned packages: they might have been updated since the last build.
rm -f dl/bennugd-*.tar.gz
rm -f dl/dingux-commander-*.tar.gz
rm -f dl/etna_viv-*.tar.gz
rm -f dl/gmenu2x-*.tar.gz
rm -f dl/libopk-*.tar.gz
rm -f dl/libini-*.tar.gz
rm -f dl/libxdgmime-*.tar.gz
rm -f dl/mesa3d-etna_viv-*.tar.gz
rm -f dl/pyclock-*.tar.gz
rm -f dl/pwswd-*.tar.gz
rm -f dl/shared-mime-info-*.tar.gz
rm -f dl/o2xiv-*.tar.gz
rm -f dl/gcw-connect-*.tar.gz
rm -f dl/sdl2-*.tar.gz

# Perform the build.
echo "Starting build..."
make

# Create packages.
echo "Creating packages..."
tar -C/opt --exclude='.fakeroot.*' -jcf opendingux-gcw0-toolchain.`date +'%Y-%m-%d'`.tar.bz2 gcw0-toolchain
