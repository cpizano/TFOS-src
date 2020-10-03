// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fuchsia/examples/rot13/cpp/fidl.h>
#include <lib/async-loop/cpp/loop.h>
#include <lib/async-loop/default.h>
#include <stdlib.h>
#include <string.h>

#include <string>

#include "rot13_client_app.h"

int main(int argc, const char **argv) {
  std::string msg = "hello world";
  std::string server_url = "fuchsia-pkg://fuchsia.com/rot13_server#meta/rot13_server.cmx";

  for (int i = 1; i < argc - 1; ++i) {
    if (!strcmp("--server", argv[i])) {
      server_url = argv[++i];
    } else if (!strcmp("-m", argv[i])) {
      msg = argv[++i];
    }
  }
  async::Loop loop(&kAsyncLoopConfigAttachToCurrentThread);

  rot13::Rot13ClientApp app;
  app.Start(server_url);

  app.rot13().set_error_handler([&loop](zx_status_t status) {
    fprintf(stderr, "Echo server closed connection: %d\n", status);
    loop.Quit();
  });

  // wait for both calls
  uint32_t checksum = 0;
  std::string rotated;
  app.rot13()->Checksum(msg, [msg, &checksum](uint32_t value) {
    printf("***** Message: %s has checksum of %u\n", msg.c_str(), value);
    checksum = value;
  });

  app.rot13()->Encrypt(msg, [&app, &rotated](fidl::StringPtr value1) {
    printf("Rotated message is %s\n", value1->data());
    app.rot13()->Encrypt(value1,
                         [&rotated](fidl::StringPtr value2) { rotated = value2.value_or(""); });
  });

  int ret = ZX_OK;
  while (ret == ZX_OK) {
    ret = loop.Run(zx::deadline_after(zx::sec(1)), false);
    if (checksum && rotated.length() > 0) {
      printf("unrotated message = %s\n", rotated.c_str());
      loop.Quit();
    }
  }
  return ret == ZX_ERR_CANCELED ? 0 : ret;
}
