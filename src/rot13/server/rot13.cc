// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "rot13.h"

namespace rot13 {
std::string DoRot13(const char *str) {
  std::string ret;

  const char *ptr = str;
  if (!ptr) {
    return "";
  }
  do {
    if (isalpha(*ptr)) {
      // add 13 if a - m.
      if (tolower(*ptr) - 'a' < 13) {
        ret.append(1, *ptr + 13);
      } else {
        ret.append(1, *ptr - 13);
      }
    } else {
      ret.append(1, *ptr);
    }
  } while (*(ptr++));

  return ret;
}

uint32_t DoChecksum(const char *str) {
  uint32_t ret = 0;
  const char *ptr = str;
  if (!ptr) {
    return 0;
  }
  do {
    ret += *ptr;
  } while (*(ptr++));

  return ret;
}
}  // namespace rot13
