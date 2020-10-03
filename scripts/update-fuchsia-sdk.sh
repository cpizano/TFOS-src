#!/bin/bash
# Copyright 2020 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Specify the version of the tools to download
if [[ "$1" == "" ]]; then
  VER_FUCHSIA_SDK="latest"
else
  VER_FUCHSIA_SDK="$1"
fi

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

# Common functions.
# shellcheck disable=SC1090
source "${SCRIPT_SRC_DIR}/common.sh" || exit $?
THIRD_PARTY_DIR="$(get_third_party_dir)" # finds path to //third_party
FUCHSIA_SDK_DIR="${THIRD_PARTY_DIR}/fuchsia-sdk" # finds path to //third_party/fuchsia-sdk
TMP_SDK_DOWNLOAD_DIR=$(mktemp -d)
DOWNLOADED_SDK_PATH="${TMP_SDK_DOWNLOAD_DIR}/gn-sdk.tar.gz"
TMP_SDK_DIR=$(mktemp -d)

cleanup() {
  # Remove the SDK downloads directory
  if [ -f "${TMP_SDK_DOWNLOAD_DIR}" ]; then
    rm -rf "${TMP_SDK_DOWNLOAD_DIR}"
  fi
  if [ -d "${TMP_SDK_DIR}" ]; then
    rm -rf "${TMP_SDK_DIR}"
  fi
}

if is-mac; then
  PLATFORM="mac"
else
  PLATFORM="linux"
fi
ARCH="${PLATFORM}-amd64"

# You can browse the GCS bucket from here to look for builds https://console.cloud.google.com/storage/browser/fuchsia/development
# You can get the instance ID with the following curl commands:
#  Linux: `curl -sL "https://storage.googleapis.com/fuchsia/development/LATEST_LINUX`
#  Mac: `curl -sL "https://storage.googleapis.com/fuchsia/development/LATEST_MAC`
# You can use the gsutil command-line tool to browse and search as well:
#  Get the instance ID:
#    Linux: `gsutil cat gs://fuchsia/development/LATEST_LINUX`
#    Mac: `gsutil cat gs://fuchsia/development/LATEST_MAC`
#  List the SDKs available for the instance ID
#    `gsutil ls -r gs://fuchsia/development/$INSTANCE_ID/sdk`
#  Download a SDK from GCS to your current directory:
#    Linux: `gsutil cp gs://fuchsia/development/$INSTANCE_ID/sdk/linux-amd64/gn.tar.gz .`
#    Mac: `gsutil cp gs://fuchsia/development/$INSTANCE_ID/sdk/mac-amd64/gn.tar.gz .`

# If specified version is "latest" get the latest version number
if [ "${VER_FUCHSIA_SDK}" == "latest" ]; then
  PLATFORM_UPPER="$(echo "${PLATFORM}" | tr '[:lower:]' '[:upper:]')"
  VER_FUCHSIA_SDK="$(curl -sL "https://storage.googleapis.com/fuchsia/development/LATEST_${PLATFORM_UPPER}")"
fi

echo "Downloading Fuchsia SDK ${VER_FUCHSIA_SDK} ..."
# Example URL: https://storage.googleapis.com/fuchsia/development/8888449404525421136/sdk/linux-amd64/gn.tar.gz
curl -sL "https://storage.googleapis.com/fuchsia/development/${VER_FUCHSIA_SDK}/sdk/${ARCH}/gn.tar.gz" -o "${DOWNLOADED_SDK_PATH}"
echo "complete."
echo

echo "Extracting Fuchsia SDK..."
tar -xf "${DOWNLOADED_SDK_PATH}" -C "${TMP_SDK_DIR}"
echo "complete."
echo


# Delete existing SDK
if [ -d "${FUCHSIA_SDK_DIR}" ]; then
  echo "Removing existing SDK..."
  # Remove entire folder and remake folder of the same name to remove hidden files
  # e.g. third_party/fuchsia-sdk/.build-id/
  rm -rf "${FUCHSIA_SDK_DIR}"
  mkdir "${FUCHSIA_SDK_DIR}"
  echo "complete."
  echo
fi

# Copy new SDK to SDK dir
cp -r "${TMP_SDK_DIR}/." "${FUCHSIA_SDK_DIR}"

cleanup

echo "New SDK downloaded and extracted successfully to ${FUCHSIA_SDK_DIR}."
