// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "rot13_server_app.h"

#include <cctype>

#include "rot13.h"

namespace rot13 {
Rot13ServerApp::Rot13ServerApp()
    : Rot13ServerApp(sys::ComponentContext::CreateAndServeOutgoingDirectory()) {}

Rot13ServerApp::Rot13ServerApp(std::unique_ptr<sys::ComponentContext> context)
    : context_(std::move(context)) {
  context_->outgoing()->AddPublicService(bindings_.GetHandler(this));
}

Rot13ServerApp::~Rot13ServerApp() {}

void Rot13ServerApp::Encrypt(
    ::fidl::StringPtr value,
    fuchsia::examples::rot13::Rot13::EncryptCallback callback) {
  std::string encrypted = DoRot13(value->data());
  callback(encrypted);
}
void Rot13ServerApp::Checksum(
    ::fidl::StringPtr value,
    fuchsia::examples::rot13::Rot13::ChecksumCallback callback) {
  uint32_t cksum = DoChecksum(value->data());
  callback(cksum);
}

}  // namespace rot13
