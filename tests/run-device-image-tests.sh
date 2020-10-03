#!/bin/bash
# Copyright 2019 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This is a test script that can be run inside femu-exec-wrapper.sh or on a local device
# It verifies that the system image has basic functionality and that it works as expected
# For x64 tests:
# ../third_party/fuchsia-sdk/bin/femu-exec-wrapper.sh --exec ./run-device-image-tests.sh
#
# For arm64 tests:
# FUCHSIA_ARCH=arm64 ../third_party/fuchsia-sdk/bin/femu-exec-wrapper.sh --image qemu-arm64 --experiment-arm64 --software-gpu --exec ./run-device-image-tests.sh

TEST_SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SDK_BIN_DIR="${TEST_SRC_DIR}/../third_party/fuchsia-sdk/bin"
if [[ "${FUCHSIA_ARCH}" == "" ]]; then
  FUCHSIA_ARCH="x64"
  echo "No FUCHSIA_ARCH selected so using default ${FUCHSIA_ARCH}"
fi
if [[ "${FUCHSIA_ARCH}" == "arm64" ]]; then
  # Select very long timeouts since the arm64 emulator is very slow
  TIMEOUT_RUN=60
  TIMEOUT_START=60
else
  TIMEOUT_RUN=30
  TIMEOUT_START=5
fi
echo "Running tests for FUCHSIA_ARCH ${FUCHSIA_ARCH} with run timeout ${TIMEOUT_RUN} and start timeout ${TIMEOUT_START} seconds"

ERROR_COUNT="0"
ERROR_LIST=""

# Force all pipes to return any non-zero error code instead of just the last
set -o pipefail

# Check that a remote command can run and check the exit code, it should not block for longer than ${TIMEOUT} seconds
function wait_command {
  echo "Running wait_command: $*"
  RESULT=0
  timeout "${TIMEOUT_RUN}" "${SDK_BIN_DIR}/fssh.sh" "${@}" || RESULT="$?"
  if [ "${RESULT}" == "0" ]; then
    echo "OK: $*"
  elif [ "${RESULT}" == "124" ]; then
    echo "TIMEOUT: Did not reach exit of command after ${TIMEOUT_RUN} seconds: $*"
    ERROR_COUNT=$((ERROR_COUNT+1))
    ERROR_LIST="${ERROR_LIST} \"$*\""
  else
    echo "ERROR: Result ${RESULT} for $*"
    ERROR_COUNT=$((ERROR_COUNT+1))
    ERROR_LIST="${ERROR_LIST} \"$*\""
  fi
}

# Useful for remote commands that never normally terminate, just check that they run for a while without exiting
function timeout_command {
  TIMEOUT="$1"
  shift
  echo "Running timeout_command for ${TIMEOUT}: $*"
  RESULT=0
  timeout "${TIMEOUT}" "${SDK_BIN_DIR}/fssh.sh" "${@}" || RESULT="$?"
  if [ "${RESULT}" == "0" ]; then
    echo "ERROR: Exited with 0 but was expecting to wait"
    ERROR_COUNT=$((ERROR_COUNT+1))
    ERROR_LIST="${ERROR_LIST} \"$*\""
  elif [ "${RESULT}" == "124" ]; then
    # This is the expected result if the timeout command terminates the shell
    echo "OK: $*"
  else
    echo "ERROR: Result ${RESULT} for command: $*"
    ERROR_COUNT=$((ERROR_COUNT+1))
    ERROR_LIST="${ERROR_LIST} \"$*\""
  fi
}

# Publish .far files in preparation for running
function publish_far {
  FAR="$1"
  echo "Publishing ${FAR}"
  if [ ! -f "${FAR}" ]; then
    echo "ERROR: ${FAR} does not exist"
    ERROR_COUNT=$((ERROR_COUNT+1))
    ERROR_LIST="${ERROR_LIST} \"$*\""
    return
  fi
  if ! "${SDK_BIN_DIR}/fpublish.sh" "${FAR}" 2>&1; then
    echo "ERROR: Could not publish ${FAR}"
    ERROR_COUNT=$((ERROR_COUNT+1))
    ERROR_LIST="${ERROR_LIST} \"$*\""
    return
  fi
}

# Check that a component is running on a device
function check_cs_exists {
  NAME="$1"
  echo "Checking for cs exists ${NAME}"
  # cs outputs each line as "  ${component}[$pid]: fuchsia-pkg://${url} (sha)"
  FOUND=$("${SDK_BIN_DIR}/fssh.sh" "cs" | grep " ${NAME}\[")
  if [[ "${FOUND}" == "" ]]; then
    echo "ERROR: ${NAME} does not appear in cs"
    ERROR_COUNT=$((ERROR_COUNT+1))
    ERROR_LIST="${ERROR_LIST} \"$*\""
  else
    echo "OK: $*"
  fi
}

# ---- Begin device setup before tests ----

# Check that pm is running, check both the process name and arguments
# shellcheck disable=SC2009
PS_PM=$(ps -axww | grep "pm serve" | grep "amber-files")
if [ "${PS_PM}" == "" ]; then
  echo "ERROR! pm package serve is not running"
  exit 1
fi
AMBER_SRCS=$("${SDK_BIN_DIR}/fssh.sh" amber_ctl list_srcs | grep RepoUrl | grep "fuchsia-pkg")
if [ "${AMBER_SRCS}" == "" ]; then
  echo "ERROR! amber_ctl does not have a repo configured in list_srcs, make sure fserve.sh has been run"
  exit 1
fi

# ---- Start tests ----

# Restart tiles_ctl so id numbers of new tiles is consistent
wait_command tiles_ctl quit
wait_command tiles_ctl start
# Check if tiles_ctl component is running
check_cs_exists tiles.cmx
wait_command tiles_ctl list
# Run some basic Fuchsia commands
wait_command echo hello
wait_command date

# This should check if the web view is working, but it currently does not work.
# We just use a localhost IP address so the network is not actually needed
# wait_command tiles_ctl add "http://127.0.0.1"

# Check if basic .far files work
publish_far "${TEST_SRC_DIR}/../out/${FUCHSIA_ARCH}/hello_world.far"
wait_command run "fuchsia-pkg://fuchsia.com/hello_world#meta/hello_world.cmx"

# Start the graphical bouncing_ball demo
publish_far "${TEST_SRC_DIR}/../out/${FUCHSIA_ARCH}/bouncing_ball.far"
wait_command tiles_ctl add "fuchsia-pkg://fuchsia.com/bouncing_ball#meta/bouncing_ball.cmx"
# Wait for the bouncing_ball demo to actually start and make sure it keeps running for a while
wait_command sleep "${TIMEOUT_START}"
check_cs_exists "bouncing_ball.cmx"
wait_command tiles_ctl remove 1

if [ "${ERROR_COUNT}" == "0" ]; then
  echo "OK: All tests passed!"
else
  echo "ERROR: ${ERROR_COUNT} tests failed, see log output"
  echo "ERROR: List${ERROR_LIST}"
  exit 1
fi
