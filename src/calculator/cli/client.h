// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXAMPLES_CALCULATOR_CLI_CLIENT_H_
#define EXAMPLES_CALCULATOR_CLI_CLIENT_H_

#include <fuchsia/examples/calculator/cpp/fidl.h>
#include <fuchsia/sys/cpp/fidl.h>
#include <lib/sys/cpp/component_context.h>

namespace calculator_cli {

const std::string kServerUrl =
    "fuchsia-pkg://fuchsia.com/calculator_engine#meta/calculator_engine.cmx";

/// Performs binary operations. The calculator connects to a calculator engine
/// over FIDL, performs the operation as a one-shot, and exits.
class CalculatorClient {
 public:
  CalculatorClient();
  CalculatorClient(std::unique_ptr<sys::ComponentContext> context);

  fuchsia::examples::calculator::CalculatorPtr &calculator() { return calculator_; }

  void Start(std::string kServerUrl);

 private:
  CalculatorClient(const CalculatorClient &) = delete;
  CalculatorClient &operator=(const CalculatorClient &) = delete;

  std::unique_ptr<sys::ComponentContext> context_;
  fuchsia::sys::ComponentControllerPtr controller_;
  fuchsia::examples::calculator::CalculatorPtr calculator_;
};
}  // namespace calculator_cli

#endif  // EXAMPLES_CALCULATOR_CLI_CLIENT_H_
