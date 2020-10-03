#!/bin/bash
# Copyright 2019 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set -eu # Error checking
err_print() {
  cleanup
  echo "Error on line $1"
}
trap 'err_print $LINENO' ERR

SCRIPT_SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
FORCE=0

# Common functions.
# shellcheck disable=SC1090
source "${SCRIPT_SRC_DIR}/common.sh" || exit $?
REPO_ROOT=$(get_gn_root) # finds path to REPO_ROOT
BUILD_TOOLS_DIR=$(get_buildtools_dir) # finds path to BUILD_TOOLS_DIR
DEPOT_TOOLS_DIR=$(get_depot_tools_dir) # finds path to DEPOT_TOOLS_DIR
DEBUG_FLAG="true"

OUT_DIR="${REPO_ROOT}/out"

cleanup() {
  echo "Cleaning up built files in ${OUT_DIR} ..."
  # Remove the out directory but check for safety
  if [ "${OUT_DIR}" == "/" ]; then
    echo "Error: out directory cannot be \"/\""
    exit 1
  else
    rm -rf "${OUT_DIR}"
  fi
}

function usage {
  echo "Usage: $0"
  echo "  [--force]"
  echo "    Delete out directory and override build tools directory existence check"
  echo "  [--release]"
  echo "    Builds with debug=false."
}

# Parse command line
for i in "$@"
do
case $i in
    -f|--force)
    FORCE=1
    ;;
    --release)
    DEBUG_FLAG="false"
    OUT_DIR="${OUT_DIR}-release"
    ;;
    *)
    # unknown option
    usage
    exit 1
    ;;
esac
done

# Ensure build tools repo exists
if [ ! -d "${BUILD_TOOLS_DIR}" ]; then
  echo "Error: Could not find build tools in \"${REPO_ROOT}/${BUILD_TOOLS_DIR}\""
  echo "Have you run \"./scripts/download-build-tools.sh\"?"
  exit 1;
fi

# Cleanup before build it force flag is set.
if [ ! "${FORCE}" == 0 ]; then
  cleanup
fi

echo "Building for Fuchsia on arm64..."
"${DEPOT_TOOLS_DIR}/gn" gen "${OUT_DIR}/arm64" "--args=target_os=\"fuchsia\" target_cpu=\"arm64\" is_debug=$DEBUG_FLAG"
"${DEPOT_TOOLS_DIR}/ninja" -C "${OUT_DIR}/arm64" default tests

echo "Building for Fuchsia on x64..."
"${DEPOT_TOOLS_DIR}/gn" gen "${OUT_DIR}/x64"   "--args=target_os=\"fuchsia\" target_cpu=\"x64\" is_debug=$DEBUG_FLAG"
"${DEPOT_TOOLS_DIR}/ninja" -C "${OUT_DIR}/x64" default tests

echo "Building for linux on x64..."
"${DEPOT_TOOLS_DIR}/gn" gen "${OUT_DIR}/linux" "--args=target_os=\"linux\" target_cpu=\"x64\" is_debug=$DEBUG_FLAG"
"${DEPOT_TOOLS_DIR}/ninja" -C "${OUT_DIR}/linux" default tests

echo
echo "Samples built successfully!"
