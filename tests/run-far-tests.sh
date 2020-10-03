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

TEST_SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Common functions.
# shellcheck disable=SC1090
source "${TEST_SRC_DIR}/../scripts/common.sh" || exit $?
REPO_ROOT=$(get_gn_root) # finds path to REPO_ROOT

ROOT_OUT_DIR="out"

function usage {
  echo "Usage: $0"
  echo "  [--release]"
  echo "    Uses the out-release/ directory to run tests."
}

# Parse command line
for i in "$@"
do
case $i in
    --release)
    ROOT_OUT_DIR="${ROOT_OUT_DIR}-release"
    ;;
    *)
    # unknown option
    usage
    exit 1
    ;;
esac
done

FAR_DIRS=( "${REPO_ROOT}/${ROOT_OUT_DIR}/arm64" "${REPO_ROOT}/${ROOT_OUT_DIR}/x64" )

echo "==== Testing FAR files ===="
far_files=()
for d in "${FAR_DIRS[@]}"; do
  while IFS='' read -r line; do far_files+=("$line"); done < <(find "${d}" -name "*.far" ! -name "meta.far")
done
if [ ! ${#far_files[@]} -eq 0 ]; then
    printf '%s\n' "${far_files[@]}"
else
    echo "Error: No far files found in \"${FAR_DIRS[*]}\""
    exit 1;
fi

echo
echo "==== Scanning FAR files to check for missing shared libraries ===="

for far in "${far_files[@]}"; do
    "${TEST_SRC_DIR}/far.sh" "${far}"
done

echo
echo "No missing shared libraries found."

echo
echo "Success!"
