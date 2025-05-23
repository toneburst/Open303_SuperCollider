#! /bin/bash

# Adapted from:
# https://github.com/madskjeldgaard/portedplugins/blob/main/scripts/rpi_build
#
# This version for offline build/install on Norns when Norns itself has no internet access.
# Requires SuperCollider source in same dir as this plugin project.
# This plugin project directory and SuperCollider source should be installed at path /home/we/dust/development/SuperCollider.
# See below for required paths.
# Script to be run locally on Norns via SSH or remotely run on the device from local development machine
# (see scripts/norns_build_remote.sh).

# Temp dir for build
BASE_DIR="/home/we/development/SuperCollider"
# Project directory
PROJ_DIR="${BASE_DIR}/Open303_SuperCollider"
# SuperCollider source directory
SC_DIR="${BASE_DIR}/supercollider"
# Intermediate build directory
INSTALL_CACHE="${PROJ_DIR}/build/install_cache"
# SuperCollider extension install directory
INSTALL_DIR="/home/we/.local/share/SuperCollider/Extensions"

# Check if local SuperCollider source directory exists
if [ ! -d "$SC_DIR" ]; then
    echo "ERROR: SuperCollider source directory not found at $SC_DIR."
    echo "Exiting now"
    exit 1
fi

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

# Start build process
echo "Starting build from $(pwd)"
cmake .. -DCMAKE_BUILD_TYPE='Release' -DSC_PATH="${SC_DIR}" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"
cmake --build . --config Release

# Check if the build was successful
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed. Exiting"
    exit 1
else
    echo "Build completed successfully."
fi

# Delete existing extensions
if [ -d "${INSTALL_DIR}/Open303SuperCollider" ]; then
    echo "Deleting existing Open303SuperCollider extension..."
    rm -rf "${INSTALL_DIR}/Open303SuperCollider"
fi

echo "Build complete. Installing to ${INSTALL_DIR}"
cmake --build . --config Release --target install

# Check if the install was successful
if [ $? -ne 0 ]; then
    echo "ERROR: Install failed. Exiting"
    exit 1
fi

# Delete Supernova plugin version
echo "Deleting Supernova plugin version if present..."
rm -rf "${INSTALL_DIR}/toneburst/Open303/Open303_supernova.scx"

echo "Plugin built and installed to ${INSTALL_DIR}"
echo "Please restart your Norns to load the Open303 extension."

exit 0
