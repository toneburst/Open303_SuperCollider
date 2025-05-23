#! /bin/bash

# Adapted from:
# https://github.com/madskjeldgaard/portedplugins/blob/main/scripts/rpi_build
#
# This version for offline build/install on macOS. Requires SuperCollider source in same dir as plugin project.

# Directory containing this script
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# Local source directory for SuperCollider source and plugin project (must be in same dir!)
SOURCE_DIR="${SCRIPT_DIR}/../.."
# Local plugin project dir name
PROJ_NAME="Open303_SuperCollider"
# Project directory
PROJ_DIR="${SOURCE_DIR}/${PROJ_NAME}"
# Local SuperCollider source dir
SC_DIR="${SOURCE_DIR}/supercollider"
# SuperCollider extension install directory (standard macOS location)
INSTALL_DIR="${HOME}/Library/Application Support/SuperCollider/Extensions"

# Create build directory
cd "$PROJ_DIR"

# Check if SuperCollider source directory exists
if [ ! -d "$SC_DIR" ]; then
    echo "ERROR: SuperCollider source directory not found at $SC_DIR. Exiting"
    exit 1
else
    echo "SuperCollider source directory found at $SC_DIR."
fi

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
    rm -rf "${INSTALL_DIR}/${PROJ_NAME}"
fi

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

echo "Build complete. Installing to ${INSTALL_DIR}"
cmake --build . --config Release --target install

# Check if the install was successful
if [ $? -ne 0 ]; then
    echo "ERROR: Install failed. Exiting"
    exit 1
fi

echo "Plugin built and installed to ${INSTALL_DIR}"

exit 0
