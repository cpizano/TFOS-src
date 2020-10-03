// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXAMPLES_ROT13_SERVER_ROT13_SERVER_APP_H_
#define EXAMPLES_ROT13_SERVER_ROT13_SERVER_APP_H_

#include <fuchsia/examples/rot13/cpp/fidl.h>
#include <lib/fidl/cpp/binding_set.h>
#include <lib/sys/cpp/component_context.h>

namespace rot13
{
class Rot13ServerApp : public fuchsia::examples::rot13::Rot13
{
public:
  explicit Rot13ServerApp();
  ~Rot13ServerApp() override;
  void Encrypt(::fidl::StringPtr value, EncryptCallback callback) override;
  void Checksum(::fidl::StringPtr value, ChecksumCallback callback) override;

protected:
  Rot13ServerApp(std::unique_ptr<sys::ComponentContext> context);

private:
  Rot13ServerApp(const Rot13ServerApp &) = delete;
  Rot13ServerApp &operator=(const Rot13ServerApp &) = delete;
  std::unique_ptr<sys::ComponentContext> context_;
  fidl::BindingSet<Rot13> bindings_;
};
} // namespace rot13

#endif // EXAMPLES_ROT13_SERVER_ROT13_SERVER_APP_H_
