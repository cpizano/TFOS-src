// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/// A mathematics engine that can be called via FIDL to perform mathematical
/// operations over a protocol.

#ifndef EXAMPLES_CALCULATOR_ENGINE_ENGINE_H_
#define EXAMPLES_CALCULATOR_ENGINE_ENGINE_H_

namespace calculator_engine {

/// Calculates the negation of the operand.
double negate(double a);

/// Calculates the sum of the operands.
double add(double augend, double addend);

/// Calculates the difference of the operands. This is equivalent to
/// minuend - subtrahend.
double subtract(double minuend, double subtrahend);

/// Calculates the product of the operands.
double multiply(double multiplicand, double multiplier);

/// Calculates the quotient of the operands. This is equivalent to
/// dividend / divisor.
double divide(double dividend, double divisor);

}  // namespace calculator_engine

#endif  // EXAMPLES_CALCULATOR_ENGINE_ENGINE_H_
