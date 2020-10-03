// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <lib/async-loop/cpp/loop.h>
#include <lib/async-loop/default.h>

#include <string>

#include "rot13_server_app.h"

int main(int argc, const char** argv) {
  async::Loop loop(&kAsyncLoopConfigAttachToCurrentThread);

  rot13::Rot13ServerApp app;

  int ret = loop.Run();

  printf("rot13 server app exiting: %u\n", ret);
  return ret;
}
