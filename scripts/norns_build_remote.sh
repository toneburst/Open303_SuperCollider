#!/bin/bash

# Sync project folder with Norns using rsync and build on device remotely.
# Should work on macOS and Linux systems. YMMV on Windows.
# Exchange SSH keys with Norns, or you'll have to enter password several times during script execution.

# Directory containing this script
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Local source directory for SuperCollider source and plugin project (must be in same dir!)
SOURCE_DIR="${SCRIPT_DIR}/../.."
# Local plugin project dir name
PROJ_NAME="Open303_SuperCollider"
# Local SuperCollider source dir
SC_DIR="${SOURCE_DIR}/supercollider"

# Norns user
NORNS_USER="we"
# Norns
NORNS="${NORNS_USER}@norns.local"

# Target directory on the Norns device
TARGET_DIR="/home/we/development/SuperCollider"
# Project name/dir name
PROJ_NAME="Open303_SuperCollider"

# Create target directories for SC source and plugin project on Norns if not present (creating intermediate directories as required)
ssh "$NORNS" "mkdir -p ${TARGET_DIR}/supercollider ${TARGET_DIR}/${PROJ_NAME}"

# Check if target dir created successfully
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to create target directories on Norns. Exiting"
    exit 1
else
    echo "Target directories created successfully on Norns."
fi

# Check if SuperCollider source directory exists
if [ ! -d "$SC_DIR" ]; then
    echo "ERROR: SuperCollider source directory not found at $SC_DIR. Exiting"
    exit 1
else
    echo "SuperCollider source directory found at $SC_DIR."
fi

# Sync SuperCollider source directory to the target directory if required
echo "Syncing SuperCollider source to Norns..."
rsync \
    --partial \
    --archive \
    --exclude 'build*' \
    --exclude '*.DS_Store' \
    --itemize-changes \
    "${SC_DIR}/" "${NORNS}:${TARGET_DIR}/supercollider"

# Check if the rsync command was successful
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to sync SuperCollider source. Exiting"
    exit 1
else
    echo "SuperCollider source synced successfully to Norns."
fi

# Sync the project folder to the target directory
echo "Syncing plugin source to Norns..."
rsync \
    --partial \
    --archive \
    --exclude 'build*' \
    --exclude '*.DS_Store' \
    --delete \
    --itemize-changes \
    "${SOURCE_DIR}/${PROJ_NAME}/" "${NORNS}:${TARGET_DIR}/${PROJ_NAME}"

# Check if the rsync command was successful
if [ $? -ne 0 ]; then
    echo "ERROR: rsync failed to sync plugin project directory. Exiting"
    exit 1
else
    echo "Plugin project synced successfully to Norns."
fi

# # # Run build on Norns
ssh "$NORNS" -T < "${SCRIPT_DIR}/norns_build_install_offline.sh"

# Check if plugin build and install was successful
if [ $? -ne 0 ]; then
    echo "ERROR: Plugin build/install failed."
    exit 1
else
    echo "Plugin build and install completed successfully on Norns. Restart your Norns to load the new SuperCollider extension."
fi

exit 0