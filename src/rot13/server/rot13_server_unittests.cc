// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef __FUCHSIA__
#include <lib/gtest/test_loop_fixture.h>
#include <lib/sys/cpp/testing/component_context_provider.h>
#endif

#include <gtest/gtest.h>

#include "rot13_server_app.h"

namespace rot13 {
namespace testing {

using namespace fuchsia::examples::rot13;

class Rot13ServerAppForTest : public Rot13ServerApp {
 public:
  // Expose injecting constructor so we can pass an instrumented Context
  Rot13ServerAppForTest(std::unique_ptr<sys::ComponentContext> context)
      : Rot13ServerApp(std::move(context)) {}
};

class Rot13ServerAppTest : public gtest::TestLoopFixture {
 public:
  void SetUp() override {
    TestLoopFixture::SetUp();
    rot13ServerApp_.reset(new Rot13ServerAppForTest(provider_.TakeContext()));
  }

  void TearDown() override {
    rot13ServerApp_.reset();
    TestLoopFixture::TearDown();
  }

 protected:
  Rot13Ptr rot13() {
    Rot13Ptr rot13;
    provider_.ConnectToPublicService(rot13.NewRequest());
    return rot13;
  }

 private:
  std::unique_ptr<Rot13ServerAppForTest> rot13ServerApp_;
  sys::testing::ComponentContextProvider provider_;
};

// Check A and Z are rotated correctly.
TEST_F(Rot13ServerAppTest, Encrypt_EdgeCases) {
  Rot13Ptr rot13_ = rot13();
  ::fidl::StringPtr message = "bogus";
  ::fidl::StringPtr resp = "bogus";
  rot13_->Encrypt("aa is AA not ZZ zz!",
                  [&](::fidl::StringPtr retval) { message = retval; });
  rot13_->Encrypt("nn vf NN abg MM mm!",
                  [&](::fidl::StringPtr retval) { resp = retval; });
  RunLoopUntilIdle();
  EXPECT_STREQ("nn vf NN abg MM mm!", message->data());
  EXPECT_STREQ("aa is AA not ZZ zz!", resp->data());
}

// Answer "Hello World" with "Uryyb Jbeyq"
TEST_F(Rot13ServerAppTest, Encrypt_HelloWorld) {
  Rot13Ptr rot13_ = rot13();
  ::fidl::StringPtr message = "bogus";
  rot13_->Encrypt("Hello World!",
                  [&](::fidl::StringPtr retval) { message = retval; });
  RunLoopUntilIdle();
  EXPECT_STREQ("Uryyb Jbeyq!", message->data());
}

// Answer "" with ""
TEST_F(Rot13ServerAppTest, Encrypt_Empty) {
  Rot13Ptr rot13_ = rot13();
  fidl::StringPtr message = "bogus";
  rot13_->Encrypt("", [&](::fidl::StringPtr retval) { message = retval; });
  RunLoopUntilIdle();
  EXPECT_STREQ("", message->data());
}

TEST_F(Rot13ServerAppTest, Checksum_Empty) {
  Rot13Ptr rot13_ = rot13();
  uint32_t value = -1;
  rot13_->Checksum("", [&](uint32_t retval) { value = retval; });
  RunLoopUntilIdle();
  EXPECT_EQ(static_cast<uint32_t>(0), value);
}

TEST_F(Rot13ServerAppTest, Checksum_HelloWorld) {
  Rot13Ptr rot13_ = rot13();
  uint32_t value = -1;
  rot13_->Checksum("Hello World!", [&](uint32_t retval) { value = retval; });
  RunLoopUntilIdle();
  EXPECT_EQ(static_cast<uint32_t>(1085), value);
}

}  // namespace testing
}  // namespace rot13
