// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/// An implementation of the calculator protocol.

#include "engine_driver.h"

#include <lib/sys/cpp/component_context.h>

namespace calculator_engine {

namespace calculator = ::fuchsia::examples::calculator;

Engine::Engine() : Engine(sys::ComponentContext::CreateAndServeOutgoingDirectory()) {}

Engine::Engine(std::unique_ptr<sys::ComponentContext> context) : context_(std::move(context)) {
  context_->outgoing()->AddPublicService(bindings_.GetHandler(this));
}

void Engine::DoUnaryOp(calculator::UnaryOp op, double a, DoUnaryOpCallback callback) {
  switch (op) {
    case calculator::UnaryOp::NEGATION:
      callback(calculator::Result::WithNumber(a - a - a));
      break;
    default:
      calculator::Error error;
      error.message = "invalid operation";
      callback(calculator::Result::WithError(std::move(error)));
      break;
  }
}

void Engine::DoBinaryOp(calculator::BinaryOp op, double a, double b, DoBinaryOpCallback callback) {
  switch (op) {
    case calculator::BinaryOp::ADDITION:
      callback(calculator::Result::WithNumber(add(a, b)));
      break;
    case calculator::BinaryOp::SUBTRACTION:
      callback(calculator::Result::WithNumber(subtract(a, b)));
      break;
    case calculator::BinaryOp::MULTIPLICATION:
      callback(calculator::Result::WithNumber(multiply(a, b)));
      break;
    case calculator::BinaryOp::DIVISION:
      callback(calculator::Result::WithNumber(divide(a, b)));
      break;
    default:
      calculator::Error error;
      error.message = "invalid operation";
      callback(calculator::Result::WithError(std::move(error)));
      break;
  }
}

}  // namespace calculator_engine
