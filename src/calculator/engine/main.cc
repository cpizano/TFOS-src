// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/// A driver for the math engine. This file is the entry point of execution.

#include <fuchsia/examples/calculator/cpp/fidl.h>
#include <lib/async-loop/cpp/loop.h>
#include <lib/async-loop/default.h>

#include "engine_driver.h"

namespace calculator = fuchsia::examples::calculator;

int main() {
  async::Loop loop(&kAsyncLoopConfigAttachToCurrentThread);
  calculator_engine::Engine app;
  fidl::BindingSet<calculator::Calculator> bindings;
  loop.Run();

  return 0;
}
