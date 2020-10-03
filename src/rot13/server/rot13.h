// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
namespace rot13 {
std::string DoRot13(const char *str);
uint32_t DoChecksum(const char *str);
}  // namespace rot13
