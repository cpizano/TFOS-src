// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FUCHSIA_ROT13_CLIENT_APP_H
#define FUCHSIA_ROT13_CLIENT_APP_H

#include <fuchsia/examples/rot13/cpp/fidl.h>
#include <fuchsia/sys/cpp/fidl.h>
#include <lib/sys/cpp/component_context.h>

namespace rot13 {
class Rot13ClientApp {
 public:
  Rot13ClientApp();
  Rot13ClientApp(std::unique_ptr<sys::ComponentContext> context);
  ~Rot13ClientApp();
  fuchsia::examples::rot13::Rot13Ptr &rot13() { return rot13_; }

  void Start(std::string server_url);

 private:
  Rot13ClientApp(const Rot13ClientApp &) = delete;
  Rot13ClientApp &operator=(const Rot13ClientApp &) = delete;

  std::unique_ptr<sys::ComponentContext> context_;
  fuchsia::sys::ComponentControllerPtr controller_;
  fuchsia::examples::rot13::Rot13Ptr rot13_;
};

}  // namespace rot13

#endif  // FUCHSIA_ROT13_CLIENT_APP_H
