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

echo
echo "==== Run host tests ===="
for dir in "${REPO_ROOT}/${ROOT_OUT_DIR}"/*; do
  [[ -e "$dir" ]] || break # Handle case with empty out directory

  while IFS= read -r testname
  do
    "$dir/$testname"
  done < "$dir/all_host_tests.txt"
done

echo
echo "Success!"
