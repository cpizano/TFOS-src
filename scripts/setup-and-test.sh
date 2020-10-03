#!/bin/bash
# Copyright 2019 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set -eu # Error checking
err_print() {
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
REPO_ROOT="$(get_gn_root)" # finds path to REPO_ROOT

"${REPO_ROOT}/scripts/download-build-tools.sh"
"${REPO_ROOT}/scripts/build.sh"
"${REPO_ROOT}/tests/build-all-sdk-targets.sh"
"${REPO_ROOT}/tests/run-far-tests.sh"
"${REPO_ROOT}/tests/run-host-tests.sh"

# Build again in release mode.
"${REPO_ROOT}/scripts/build.sh" --release
"${REPO_ROOT}/tests/build-all-sdk-targets.sh" --release
"${REPO_ROOT}/tests/run-far-tests.sh" --release
"${REPO_ROOT}/tests/run-host-tests.sh" --release
