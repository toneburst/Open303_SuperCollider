#! /bin/bash

# Adapted from:
# https://github.com/madskjeldgaard/portedplugins/blob/main/scripts/rpi_build
#
# This version for offline build/install on macOS. Requires SuperCollider source in same dir as plugin project.

# Temp dir for build
BASE_DIR="/home/we/dev"
# Project directory
PROJ_DIR="${BASE_DIR}/Open303_SuperCollider"
# SuperCollider source directory
SC_DIR="${BASE_DIR}/supercollider"
# SuperCollider extension install directory
INSTALL_DIR=$HOME/.local/share/SuperCollider/Extensions

# Create build directory
cd $PROJ_DIR || 'exit'
# Delete existing build directory if present
if [ -d "build" ]; then
    echo "Deleting existing build directory..."
    rm -rf build
fi
echo "Making build directory... "
mkdir build
cd build || 'exit'

# Delete existing extensions
if [ -d "${INSTALL_DIR}/Open303SuperCollider" ]; then
    echo "Deleting existing Open303SuperCollider extension..."
    rm -rf "${INSTALL_DIR}/Open303SuperCollider"
fi

# Start build process
echo "Starting build from $(pwd)"
cmake .. -DCMAKE_BUILD_TYPE='Release' -DSC_PATH="${SC_DIR}" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"
cmake --build . --config Release

echo "Build complete. Installing to ${INSTALL_DIR}"
cmake --build . --config Release --target install

# Delete Supernova plugin version
echo "Deleting Supernova plugin version..."
rm -rf "${INSTALL_DIR}/Open303SuperCollider/Open303/Open303_supernova.scx"

echo "Plugin built and installed to ${INSTALL_DIR}"
echo "Please restart your Norns to load the Open303 extension."
