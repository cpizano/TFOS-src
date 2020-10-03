// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/// Test cases for the math engine that run on the development host.

#include <gtest/gtest.h>

#include "src/calculator/engine/engine.h"

namespace calculator_engine {

/// The fixture for testing class the calculator engine.
class EngineHostUnitTest : public ::testing::Test {
 protected:
  EngineHostUnitTest() = default;
};

TEST_F(EngineHostUnitTest, Negate) {
  double result = negate(3.5);
  EXPECT_DOUBLE_EQ(-3.5, result);
}

TEST_F(EngineHostUnitTest, Add) {
  double result = add(3.5, 2.5);
  EXPECT_DOUBLE_EQ(6., result);
}

TEST_F(EngineHostUnitTest, Subtract) {
  double result = subtract(3.5, 2.5);
  EXPECT_DOUBLE_EQ(1., result);
}

TEST_F(EngineHostUnitTest, Multiply) {
  double result = multiply(3.5, 2.5);
  EXPECT_DOUBLE_EQ(8.75, result);
}

TEST_F(EngineHostUnitTest, Divide) {
  double result = divide(3.5, 2.5);
  EXPECT_DOUBLE_EQ(1.4, result);
}

}  // namespace calculator_engine
