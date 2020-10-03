// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include "rot13.h"

namespace rot13 {
namespace testing {

TEST(Rot13Test, TrueTest) { EXPECT_TRUE(true); }

// Check A and Z are rotated correctly.
TEST(Rot13Test, Encrypt_EdgeCases) {
  std::string message = "bogus";
  std::string resp = "bogus";
  message = DoRot13("aa is AA not ZZ zz!");
  resp = DoRot13("nn vf NN abg MM mm!");

  EXPECT_STREQ("nn vf NN abg MM mm!", message.c_str());
  EXPECT_STREQ("aa is AA not ZZ zz!", resp.c_str());
}

// Answer "Hello World" with "Uryyb Jbeyq"
TEST(Rot13Test, Encrypt_HelloWorld) {
  std::string message = "bogus";
  message = DoRot13("Hello World!");
  EXPECT_STREQ("Uryyb Jbeyq!", message.c_str());
}

// Answer "" with ""
TEST(Rot13Test, Encrypt_Empty) {
  std::string message = "bogus";
  message = DoRot13("");
  EXPECT_STREQ("", message.c_str());
}

TEST(Rot13Test, Checksum_Empty) {
  uint32_t value = -1;
  value = DoChecksum("");
  EXPECT_EQ(static_cast<uint32_t>(0), value);
}

TEST(Rot13Test, Checksum_HelloWorld) {
  uint32_t value = -1;
  value = DoChecksum("Hello World!");
  EXPECT_EQ(static_cast<uint32_t>(1085), value);
}

}  // namespace testing
}  // namespace rot13
