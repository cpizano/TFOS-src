# Fuchsia Samples
A collection of samples demonstrating how to build, run, and test Fuchsia components outside the Fuchsia source code tree.

| These samples might be changed in backward-incompatible ways and is not recommended for production use. It is not subject to any SLA or deprecation policy. |
|-------|

## Setup
1. Install required dependencies for building:

    ```
    sudo apt-get install curl unzip clang python python2
    ```

    Note that some distributions may not have a python2 package, an alternative could be `python2.7`.

1. Clone this repo and submodules:
    ```
    git clone https://fuchsia.googlesource.com/samples --recursive --depth 1
    ```

   If you have already cloned this repo without the `--recursive` flag you can run `git submodule init && git submodule update --recursive` to download the submodules.
   The git history is quite large, and you can download only the newest commit with `--depth 1`. Remove this flag if you want the entire history.
1. Change directory to the root of the repo and setup and run the tests:

    ```
    cd samples
    ./scripts/setup-and-test.sh
    ```
    This script downloads all required dependencies (this may take 5-30min), builds the samples, and runs the tests. If the script completes without errors all tests have passed.

## C++ Samples (GN)

### Supported host platforms and build systems
The C++ samples in this repo only support Linux hosts and the [GN build system](https://gn.googlesource.com/gn/).

### Getting started
To get started see the [SDK documentation](https://fuchsia.dev/fuchsia-src/development/sdk).

### Samples
#### hello_world
The `src/hello_world` directory contains the source for the sample that prints "Hello, world". The `BUILD.gn` contains rules for how to build a single binary (`hello_bin`) with its shared and static libraries dependencies. The `hello_world.cmx` contains metadata necessary to run the binary as a component on Fuchsia, and it can be launched from a shell with:
```
run fuchsia-pkg://fuchsia.com/hello_world#meta/hello_world.cmx`
```
#### rot13 echo server and client
rot13 ("rotate by 13 places") takes a message a replaces each letter in the message with the letter 13th places after it in the alphabet. The rot13 sample has three parts: a client, a server, and a library defining the communication protocol for  the server and client:
* **FIDL**:The `src/rot13/fild` directory contains the definition of a Fuchsia interprocess communication (IPC) protocol for a rot13 server and client. `rot13.fidl` is a Fuchsia Interface Definition Language (FIDL) file that defines a message that can be sent between the server and client's that implment the protocol. The `BUILD.gn` file describes how to build the FIDL file as a library that can be used as a dependency for the client and server binaries.
* **Client**: The `src/rot13/client` directory contains the source for the rot13 echo client that sends the string "uryyb jbeyq" ("hello world" after being rot13 encoded) to a rot13 server via Fuchsia's interprocess communication (IPC) system called FIDL (Fuchsia Interface Definition Language) using the protocol defined in `src/rot13/fidl/rot13.fidl`. The client then prints the response from the server. The `BUILD.gn` file describes how to build the client binary with the necessary dependencies including a the FIDL library defined in `src/rot13/fidl`. The `meta/rot13_client.cmx` contains metadata necessary to run the binary as a component on Fuchsia.
* **Server**: The `src/rot13/server` directory contains the source for the rot13 echo server that takes rot13 encoded messages from clients and decodes the messages and sends the decoded message back to the client. The server receives and responds to message via Fuchsia's interprocess communication (IPC) system called FIDL (Fuchsia Interface Definition Language) using the protocol defined in `src/rot13/fidl/rot13.fidl`. The `BUILD.gn` file describes how to build the server binary with the necessary dependencies including a the FIDL library defined in `src/rot13/fidl`. The `meta/rot13_client.cmx` contains metadata necessary to run the binary as a component on Fuchsia.

### Repository structure
#### build
The `build` directory contains the build configuration for the sample including the toolchain, targets and tests.
#### buildtools
The `buildtools` directory contains scripts to help with the build process like downloading needed tools (e.g. gn, ninja).
#### src
The `src` directory contains the source code for the C++ samples.
#### third_party
GN samples has two third_party dependencies: the Fuchsia GN SDK and googletest (googletest is only required for testing):
* **Fuchsia SDK**: The Fuchsia SDK is a set of tools and libraries required to build Fuchsia components outside Fuchsia source code tree. It contains libraries and tools for: Installing/running Fuchsia and Fuchsia components/packages on a device, Fuchsia's interprocess communication (IPC) system (FIDL), and other tasks needed to build, run, and test Fuchsia componenets.
* **googletest**: googletest is a C++ testing framework. googletest is used to write tests for the rot13 and hello world samples. googletest is only required for testing.

## Emulating x64 on x64 hosts
Fuchsia x64 system images can be started with the included emulator scripts and run on x64 hosts. Native Vulkan support on the host is required for graphics support.
1. Install dependencies for Vulkan support:
    ```
    sudo apt-get install libvulkan1 mesa-vulkan-drivers
    ```
1. Download tool dependencies:
    ```
    ./scripts/download-build-tools.sh
    ```
1. Generate Ninja files for bouncing_ball demo:
    ```
    ./buildtools/linux64/gn gen out/x64 --args='target_os="fuchsia" target_cpu="x64"'
    ```
1. Build the bouncing_ball demo:
    ```
    ./buildtools/linux64/ninja -C out/x64 bouncing_ball
    ```
1. Start the emulator with networking support:
    ```
    ./third_party/fuchsia-sdk/bin/femu.sh -N
    ```
1. Start the package server:
    ```
    ./third_party/fuchsia-sdk/bin/fserve.sh --image qemu-x64
    ```
1. Publish the updated package for bouncing_ball:
    ```
    ./third_party/fuchsia-sdk/bin/fpublish.sh ./out/x64/bouncing_ball.far
    ```
1. SSH to the emulator and start up a demo:
    ```
    ./third_party/fuchsia-sdk/bin/fssh.sh tiles_ctl add fuchsia-pkg://fuchsia.com/bouncing_ball#meta/bouncing_ball.cmx
    ```

## Emulating arm64 on x64 hosts
Fuchsia arm64 system images can be emulated on x64 hosts with the included emulator scripts, although the performance is much slower since each instruction needs to be emulated. Graphics output is also not supported at this time, access is only via serial console or SSH.
1. Download tool dependencies:
    ```
    ./scripts/download-build-tools.sh
    ```
1. Generate Ninja files for hello_world demo:
    ```
    ./buildtools/linux64/gn gen out/arm64 --args='target_os="fuchsia" target_cpu="arm64"'
    ```
1. Build the hello_world demo:
    ```
    ./buildtools/linux64/ninja -C out/arm64 hello_world
    ```
1. Start the emulator with networking support and arm64 emulation:
    ```
    ./third_party/fuchsia-sdk/bin/femu.sh -N --experiment-arm64 --image qemu-arm64 --headless --software-gpu
    ```
1. Wait for the emulator to start, this may take a few minutes for the SSH server to allow connections:
    ```
    ./third_party/fuchsia-sdk/bin/fssh.sh uname -a
    ```
1. Start the package server:
    ```
    ./third_party/fuchsia-sdk/bin/fserve.sh --image qemu-arm64
    ```
1. Publish the updated package for hello_world:
    ```
    ./third_party/fuchsia-sdk/bin/fpublish.sh ./out/arm64/hello_world.far
    ```
1. SSH to the emulator and start a command-line demo:
    ```
    ./third_party/fuchsia-sdk/bin/fssh.sh run fuchsia-pkg://fuchsia.com/hello_world#meta/hello_world.cmx
    ```
