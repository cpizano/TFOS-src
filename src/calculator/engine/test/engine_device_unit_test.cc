// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Test cases for the math engine that run on a Fuchsia device.

#include <fuchsia/examples/calculator/cpp/fidl.h>
#include <lib/gtest/test_loop_fixture.h>
#include <lib/sys/cpp/testing/component_context_provider.h>

#include <gtest/gtest.h>

#include "examples/calculator/engine/engine_driver.h"

namespace calculator_engine {

namespace calculator = fuchsia::examples::calculator;

class EngineForTest : public Engine {
 public:
  // Expose injecting constructor so we can pass an instrumented Context
  explicit EngineForTest(std::unique_ptr<sys::ComponentContext> context)
      : Engine(std::move(context)){};
};

// The fixture for testing the Engine class.
class EngineDeviceUnitTest : public gtest::TestLoopFixture {
 public:
  void SetUp() override {
    TestLoopFixture::SetUp();
    mathEngine_.reset(new EngineForTest(provider_.TakeContext()));
  }

  void TearDown() override {
    mathEngine_.reset();
    TestLoopFixture::TearDown();
  }

 protected:
  calculator::CalculatorPtr mathEngine() {
    calculator::CalculatorPtr engine;
    provider_.ConnectToPublicService(engine.NewRequest());
    return engine;
  }

 private:
  std::unique_ptr<EngineForTest> mathEngine_;
  sys::testing::ComponentContextProvider provider_;
};

struct TestRecorder {
  calculator::Result result;
  bool callbackCalled;
};

void recordingCallback(calculator::Result result, TestRecorder *recorder) {
  recorder->callbackCalled = true;
  recorder->result = std::move(result);
}

TEST_F(EngineDeviceUnitTest, Negation) {
  calculator::CalculatorPtr engine = mathEngine();

  TestRecorder recorder;
  fit::function<void(calculator::Result)> callback =
      std::bind(&recordingCallback, std::placeholders::_1, &recorder);

  engine->DoUnaryOp(calculator::UnaryOp::NEGATION, 3.5, std::move(callback));
  RunLoopUntilIdle();

  EXPECT_TRUE(recorder.callbackCalled);
  EXPECT_TRUE(recorder.result.is_number());
  EXPECT_DOUBLE_EQ(-3.5, recorder.result.number());
}

TEST_F(EngineDeviceUnitTest, Addition) {
  calculator::CalculatorPtr engine = mathEngine();

  TestRecorder recorder;
  fit::function<void(calculator::Result)> callback =
      std::bind(&recordingCallback, std::placeholders::_1, &recorder);

  engine->DoBinaryOp(calculator::BinaryOp::ADDITION, 3.5, 2.5, std::move(callback));
  RunLoopUntilIdle();

  EXPECT_TRUE(recorder.callbackCalled);
  EXPECT_TRUE(recorder.result.is_number());
  EXPECT_DOUBLE_EQ(6., recorder.result.number());
}

TEST_F(EngineDeviceUnitTest, Subtraction) {
  calculator::CalculatorPtr engine = mathEngine();

  TestRecorder recorder;
  fit::function<void(calculator::Result)> callback =
      std::bind(&recordingCallback, std::placeholders::_1, &recorder);

  engine->DoBinaryOp(calculator::BinaryOp::SUBTRACTION, 3.5, 2.5, std::move(callback));
  RunLoopUntilIdle();

  EXPECT_TRUE(recorder.callbackCalled);
  EXPECT_TRUE(recorder.result.is_number());
  EXPECT_DOUBLE_EQ(1., recorder.result.number());
}

TEST_F(EngineDeviceUnitTest, Multiplication) {
  calculator::CalculatorPtr engine = mathEngine();

  TestRecorder recorder;
  fit::function<void(calculator::Result)> callback =
      std::bind(&recordingCallback, std::placeholders::_1, &recorder);

  engine->DoBinaryOp(calculator::BinaryOp::MULTIPLICATION, 3.5, 2.5, std::move(callback));
  RunLoopUntilIdle();

  EXPECT_TRUE(recorder.callbackCalled);
  EXPECT_TRUE(recorder.result.is_number());
  EXPECT_DOUBLE_EQ(8.75, recorder.result.number());
}

TEST_F(EngineDeviceUnitTest, Division) {
  calculator::CalculatorPtr engine = mathEngine();

  TestRecorder recorder;
  fit::function<void(calculator::Result)> callback =
      std::bind(&recordingCallback, std::placeholders::_1, &recorder);

  engine->DoBinaryOp(calculator::BinaryOp::DIVISION, 3.5, 2.5, std::move(callback));
  RunLoopUntilIdle();

  EXPECT_TRUE(recorder.callbackCalled);
  EXPECT_TRUE(recorder.result.is_number());
  EXPECT_DOUBLE_EQ(1.4, recorder.result.number());
}

}  // namespace calculator_engine
