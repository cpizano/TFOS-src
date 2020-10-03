// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/// A calculator engine that can be called via FIDL to perform mathematical
/// operations over a FIDL protocol.

#ifndef EXAMPLES_CALCULATOR_ENGINE_ENGINE_DRIVER_H_
#define EXAMPLES_CALCULATOR_ENGINE_ENGINE_DRIVER_H_

#include <fuchsia/examples/calculator/cpp/fidl.h>
#include <lib/fidl/cpp/binding_set.h>
#include <lib/sys/cpp/component_context.h>

#include "engine.h"

namespace calculator_engine {

namespace calculator = fuchsia::examples::calculator;

/// An implementation of the fidl.examples.calculator.Calculator service. This
/// service routes incoming FIDL requests to the calculation functions declared
/// in engine.h. By keeping the service separate, it's possible to test the
/// Engine through its FIDL interface with an automated test that runs on a
/// Fuchsia device.
class Engine : public calculator::Calculator {
 public:
  explicit Engine();
  virtual void DoUnaryOp(calculator::UnaryOp op, double a, DoUnaryOpCallback callback);
  virtual void DoBinaryOp(calculator::BinaryOp op, double a, double b, DoBinaryOpCallback callback);

 protected:
  Engine(std::unique_ptr<sys::ComponentContext> context);

 private:
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
  std::unique_ptr<sys::ComponentContext> context_;
  fidl::BindingSet<calculator::Calculator> bindings_;
};

}  // namespace calculator_engine

#endif  // EXAMPLES_CALCULATOR_ENGINE_ENGINE_DRIVER_H_
