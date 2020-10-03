// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
/// A tool that connects to a calculator engine over FIDL to perform
/// arithmetic operations.
#include <fuchsia/examples/calculator/cpp/fidl.h>
#include <lib/async-loop/cpp/loop.h>
#include <lib/async-loop/default.h>

#include <iostream>
#include <string>

#include "client.h"

namespace calculator = fuchsia::examples::calculator;

namespace calculator_cli {
const int kArgumentError = -1;

/// The configuration as expressed by command-line arguments passed to this
/// tool.
struct Configuration {
  int a;
  int b;
  calculator::BinaryOp op;
};

/// Prints the usage information for this tool.
void PrintUsage(char *arg0) {
  std::cerr << "Usage:" << std::endl;
  std::cerr << arg0 << " a b op" << std::endl;
}

/// Parses the arguments into a structured form. If parsing fails, this
/// function prints usage information to the screen and exits the entire
/// program.
Configuration ParseArguments(int argc, char **argv) {
  if (argc < 3) {
    PrintUsage(argv[0]);
    exit(kArgumentError);
  }
  Configuration config;
  // Each arg is known to be null-terminated, so strlen here is acceptable.
  char *a_end = argv[1] + strlen(argv[1]);
  char *b_end = argv[2] + strlen(argv[2]);
  config.a = std::strtod(argv[1], &a_end);
  config.b = std::strtod(argv[2], &b_end);
  if (argv[1] == a_end || argv[2] == b_end) {
    std::cerr << "Couldn't parse input numbers.";
    PrintUsage(argv[0]);
    exit(kArgumentError);
  }
  std::string o = argv[3];
  if (o == "+") {
    config.op = calculator::BinaryOp::ADDITION;
  } else if (o == "-") {
    config.op = calculator::BinaryOp::SUBTRACTION;
  } else if (o == "*") {
    config.op = calculator::BinaryOp::MULTIPLICATION;
  } else if (o == "/") {
    config.op = calculator::BinaryOp::DIVISION;
  } else {
    std::cerr << "Operation not supported. Acceptable operations: +, -, *, /" << std::endl;
    PrintUsage(argv[0]);
    exit(kArgumentError);
  }
  return config;
}
}  // namespace calculator_cli

/// Entry point for the calculator CLI.
int main(int argc, char **argv) {
  calculator_cli::Configuration args = calculator_cli::ParseArguments(argc, argv);
  async::Loop loop(&kAsyncLoopConfigAttachToCurrentThread);
  calculator_cli::CalculatorClient app;
  app.Start(calculator_cli::kServerUrl);
  app.calculator()->DoBinaryOp(args.op, args.a, args.b, [&loop](calculator::Result value) {
    if (value.is_error()) {
      std::cerr << "Error: " << value.error().message << std::endl;
    } else {
      std::cout << "Result: " << value.number() << std::endl;
    }
    loop.Quit();
  });
  return loop.Run();
}
