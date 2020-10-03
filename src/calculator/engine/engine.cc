// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/// An implementation of the Calculator protocol using Newton's Method.
/// Newton's method is a numerical approximation algorithm for finding the
/// roots of a function (that is, where their value is 0, or where a standard
/// 2D graph of the function crosses the x-axis). The algorithm uses the first
/// derivative of a function to iteratively improve the approximated result.
/// See https://en.wikipedia.org/wiki/Newton%27s_method.
///
/// Each of these mathematical functions has been transformed so that they have
/// a root at the desired answer.

#include "engine.h"

namespace calculator_engine {

const int ITERATION_COUNT = 1000000;

double negate(double a) {
  double x = 0;
  // The derivative f(x) = x + c is 1.
  double fPrime = 1;
  for (int i = 0; i < ITERATION_COUNT; i++) {
    // -a = x
    double fx = x + a;
    x = x - fx / fPrime;
  }
  return x;
}

double add(double augend, double addend) {
  double x = 0;
  // The derivative f(x) = x + c is 1.
  double fPrime = 1;
  for (int i = 0; i < ITERATION_COUNT; i++) {
    // augend + addend = x
    double fx = x - augend - addend;
    x = x - fx / fPrime;
  }
  return x;
}

double subtract(double minuend, double subtrahend) {
  double x = 0;
  // The derivative f(x) = x + c is 1.
  double fPrime = 1;
  for (int i = 0; i < ITERATION_COUNT; i++) {
    // minuend - subtrahend = x
    double fx = x - minuend + subtrahend;
    x = x - fx / fPrime;
  }
  return x;
}

double multiply(double multiplicand, double multiplier) {
  double x = 0;
  // The derivative f(x) = x + c is 1.
  double fPrime = 1;
  for (int i = 0; i < ITERATION_COUNT; i++) {
    // multiplicand * multiplier = x
    double fx = x - multiplicand * multiplier;
    x = x - fx / fPrime;
  }
  return x;
}

double divide(double dividend, double divisor) {
  double x = 0;
  // The derivative f(x) = x + c is 1.
  double fPrime = 1;
  for (int i = 0; i < ITERATION_COUNT; i++) {
    // dividend / divisor = x
    double fx = x - dividend / divisor;
    x = x - fx / fPrime;
  }
  return x;
}

}  // namespace calculator_engine
