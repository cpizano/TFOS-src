#!/bin/bash
# Copyright 2019 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set -eu # Error checking
err_print() {
  cleanup $FAR_FILE
  echo "Error on line $1"
}
trap 'err_print $LINENO' ERR
DEBUG_LINE() {
    $@
}

TEST_SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Common functions.
source "${TEST_SRC_DIR}/../scripts/common.sh" || exit $?
REPO_ROOT=$(get_gn_root) # finds path to REPO_ROOT
EXTRACTED_FAR_DIR_NAME=extracted_far_data

cleanup() {
  rm -rf $(dirname $1)/$EXTRACTED_FAR_DIR_NAME
}

# Ensure far tool is present
FAR_BIN="$REPO_ROOT/third_party/fuchsia-sdk/tools/far"
if [ ! -x "$FAR_BIN" ]; then
  echo "Error: Could not find file far tool in \""$FAR_BIN"\""
  exit 1;
fi

function usage {
  echo
  echo "Usage: $0 far_file"
  echo "* far_file: Fuchsia archive to be tested."
  echo
}

FAR_FILE="$1"
if [[ ! -f "$FAR_FILE" ]]; then
  usage
  echo "Error: invalid far file path \""$FAR_FILE"\""
  exit 1;
fi

echo
echo "Testing $FAR_FILE"
FAR_EXTRACTED_DIR=$(dirname $FAR_FILE)/$EXTRACTED_FAR_DIR_NAME/$(basename $FAR_FILE)/extracted
mkdir -p $FAR_EXTRACTED_DIR
$FAR_BIN extract --archive=$FAR_FILE --output=$FAR_EXTRACTED_DIR
$FAR_BIN extract --archive=$FAR_EXTRACTED_DIR/meta.far --output=$FAR_EXTRACTED_DIR
FAR_EXTRACTED_META_DIR=$FAR_EXTRACTED_DIR/meta/contents
cat $FAR_EXTRACTED_META_DIR | sed 's/^/  /'

if [[ ! $(grep -F "ld.so.1" $FAR_EXTRACTED_META_DIR) ]]; then
  echo "**** Failed to find ld.so.1 mentioned in $FAR_FILE ****"
  exit 1;
elif [[ ! $(grep -F "libc++.so" $FAR_EXTRACTED_META_DIR) ]]; then
  echo "**** Failed to find libc++.so mentioned in $FAR_FILE ****"
  exit 1;
elif [[ ! $(grep -F "libfdio.so" $FAR_EXTRACTED_META_DIR) ]]; then
  echo "**** Failed to find libfdio.so mentioned in $FAR_FILE ****"
  exit 1;
fi

cleanup $FAR_FILE

echo "Test for "$FAR_FILE" passed."
