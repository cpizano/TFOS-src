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
DEBUG_LINE() {
    "$@"
}

SCRIPT_SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
FORCE=0

# shellcheck disable=SC1090
source "${SCRIPT_SRC_DIR}/common.sh" || exit $?
REPO_ROOT="$(get_gn_root)" # finds path to REPO_ROOT
BUILD_TOOLS_DIR="$(get_buildtools_dir)" # finds path to BUILD_TOOLS_DIR
DEPOT_TOOLS_DIR="$(get_depot_tools_dir)" # finds path to DEPOT_TOOLS_DIR
DOWNLOADS_DIR="${BUILD_TOOLS_DIR}/downloads"

cleanup() {
  echo "Cleaning up downloaded build tools..."
  # Remove the download directories
  rm -rf "${BUILD_TOOLS_DIR}" "${DEPOT_TOOLS_DIR}"
}

function usage {
  echo "Usage: $0"
  echo "  [--force]"
  echo "    Delete build tools directory before downloading"
}

# Parse command line
for i in "$@"
do
case "${i}" in
    -f|--force)
    FORCE=1
    ;;
    *)
    # unknown option
    usage
    exit 1
    ;;
esac
done

# If force option is set, cleanup all downloaded tools
if (( FORCE )); then
  cleanup
fi

# Create build tools directory if it doesn't exist
if [ ! -d "${BUILD_TOOLS_DIR}" ]; then
  mkdir "${BUILD_TOOLS_DIR}"
fi

# Create depot tools directory if it doesn't exist
if [[ ! -d "${DEPOT_TOOLS_DIR}" ]]; then
  mkdir "${DEPOT_TOOLS_DIR}"
fi

# Create build tools download directory if it doesn't exist
if [ ! -d "${DOWNLOADS_DIR}" ]; then
  mkdir "${DOWNLOADS_DIR}"
fi

if is-mac; then
  ARCH=mac-amd64
else
  ARCH=linux-amd64
fi

# Download a CIPD archive and extract it to a directory based on the name and ${ARCH}
# download_cipd [name] [cipd-ref] [cipd-version] [cipd-architecture]
function download_cipd {
  CIPD_NAME="$1"
  # Valid cipd references can be found with the command-line tool: cipd ls -r | grep $search
  CIPD_REF="$2"
  # Valid cipd versions can be of many types, such as "latest", a git_revision, or a version string
  CIPD_VERSION="$3"
  # Download for a specific architecture, if empty string then download a generic version
  # For CIPD urls, replace /dl/ with /p/ if you want to inspect the directory in a web browser
  if [[ "$4" == "" ]]; then
    CIPD_URL="https://chrome-infra-packages.appspot.com/dl/${CIPD_REF}/+/${CIPD_VERSION}"
  else
    CIPD_URL="https://chrome-infra-packages.appspot.com/dl/${CIPD_REF}/${4}/+/${CIPD_VERSION}"
  fi
  CIPD_FILE="${DOWNLOADS_DIR}/${CIPD_NAME}-${ARCH}-${CIPD_VERSION}.zip"
  CIPD_TMP="${DOWNLOADS_DIR}/tmp-${CIPD_NAME}-${ARCH}-${CIPD_VERSION}"
  CIPD_DIR="${DOWNLOADS_DIR}/${CIPD_NAME}-${ARCH}-${CIPD_VERSION}"

  # Check that unzip is available, otherwise it will fail below and imply that the data was invalid
  UNZIP_BIN="$(command -v unzip)"
  if [[ "${UNZIP_BIN}" == "" ]]; then
    fx-error "Cannot find unzip."
    exit 2
  fi

  if [ ! -f "${CIPD_FILE}" ]; then
    mkdir -p "${DOWNLOADS_DIR}"
    echo "Downloading ${CIPD_NAME} archive ${CIPD_URL} ..."
    curl -L "${CIPD_URL}" -o "${CIPD_FILE}" -#
    echo -e "Verifying ${CIPD_NAME} download ${CIPD_FILE} ...\c"
    # CIPD will return a file containing "no such ref" if the URL is invalid, so need to verify the ZIP file
    if ! unzip -qq -t "${CIPD_FILE}" &> /dev/null; then
      rm -f "${CIPD_FILE}"
      echo "Error: Downloaded archive from ${CIPD_URL} failed with invalid data"
      exit 1
    fi
    rm -rf "${CIPD_TMP}" "${CIPD_DIR}"
    echo "complete."
  fi
  if [ ! -d "${CIPD_DIR}" ]; then
    echo -e "Extracting ${CIPD_NAME} archive to ${CIPD_DIR} ...\c"
    rm -rf "${CIPD_TMP}"
    unzip -q "${CIPD_FILE}" -d "${CIPD_TMP}"
    ln -sf "${CIPD_NAME}-${ARCH}-${CIPD_VERSION}" "${DOWNLOADS_DIR}/${CIPD_NAME}-${ARCH}"
    mv "${CIPD_TMP}" "${CIPD_DIR}"
    echo "complete."
  fi
}

# Download prebuilt binaries with specific versions known to work with the SDK.
# These values can be found in $FUCHSIA_ROOT/integration/prebuilts but should
# not need to be updated regularly since these tools do not change very often.
download_cipd "clang"   "fuchsia/third_party/clang"  "git_revision:b25fc4123c77097c05ea221e023fa5c6a16e0f41" "${ARCH}"
download_cipd "gn"      "gn/gn"                      "git_revision:239533d2d91a04b3317ca9101cf7189f4e651e4d" "${ARCH}"
download_cipd "ninja"   "infra/ninja"                "version:1.9.0"                                         "${ARCH}"
# Download python version of gsutil, not referenced by $FUCHSIA_ROOT/integration/prebuilts, with generic architecture
download_cipd "gsutil"  "infra/gsutil"               "version:4.46"                                          ""
# buildidtool is used to extract build id symbols from binaries.
download_cipd "buildidtool"  "fuchsia/tools/buildidtool"  "git_revision:5c546d2e55ae0a2afe565333c6118a48780f8017" "${ARCH}"


# Always refresh the symlinks because this script may have been updated
echo -e "Rebuilding symlinks in ${DEPOT_TOOLS_DIR} ...\c"
ln -sf "../downloads/clang-${ARCH}" "${DEPOT_TOOLS_DIR}/clang-${ARCH}"
ln -sf "../downloads/clang-${ARCH}/bin/clang-format" "${DEPOT_TOOLS_DIR}/clang-format"
ln -sf "../downloads/gn-${ARCH}/gn" "${DEPOT_TOOLS_DIR}/gn"
ln -sf "../downloads/ninja-${ARCH}/ninja" "${DEPOT_TOOLS_DIR}/ninja"
ln -sf "../downloads/buildidtool-${ARCH}/buildidtool" "${DEPOT_TOOLS_DIR}/buildidtool"
ln -sf "../downloads/gsutil-${ARCH}/gsutil" "${DEPOT_TOOLS_DIR}/gsutil"
if [ ! -x "$(command -v gsutil)" ]; then
  ln -sf "../../../buildtools/downloads/gsutil-${ARCH}/gsutil" "${REPO_ROOT}/third_party/fuchsia-sdk/bin/gsutil"
fi
echo "complete."

echo "All build tools downloaded and extracted successfully to ${BUILD_TOOLS_DIR}"
