#! /bin/bash

# Adapted from:
# https://github.com/madskjeldgaard/portedplugins/blob/main/scripts/rpi_build

# Temp dir for build
TMP_DIR="/tmp/o303scplugin"
# Project directory
PROJ_DIR="${TMP_DIR}/open303_supercollider"
# SuperCollider source directory
SC_DIR="${TMP_DIR}/supercollider"
# SuperCollider extension install directory
INSTALL_DIR=$HOME/local/share/SuperCollider/Extensions

# Check out plugin repo
git clone --recurse-submodules https://github.com/madskjeldgaard/portedplugins.git $PROJ_DIR

# Check out SuperCollider source code
git clone --recurse-submodules https://github.com/supercollider/supercollider.git $SC_DIR

# Create build directory

cd $PROJ_DIR || 'exit'
echo "Making build directory... "
mkdir build
cd build || 'exit'

# Start build process
echo "Starting build from $(pwd)"
cmake .. -DCMAKE_BUILD_TYPE='Release' -DSC_PATH="${SC_DIR}" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"
cmake --build . --config Release

echo "Build complete. Installing to ${INSTALL_DIR}"
cmake --build . --config Release --target install

# Clean up
echo "Cleaning up build files..."
rm -rf "$TMP_DIR"

echo "Done!"