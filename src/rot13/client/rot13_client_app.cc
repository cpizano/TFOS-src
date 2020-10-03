// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "rot13_client_app.h"

namespace rot13 {

Rot13ClientApp::Rot13ClientApp()
    : Rot13ClientApp(sys::ComponentContext::CreateAndServeOutgoingDirectory()) {}

Rot13ClientApp::Rot13ClientApp(std::unique_ptr<sys::ComponentContext> context)
    : context_(std::move(context)) {}

Rot13ClientApp::~Rot13ClientApp() {}

void Rot13ClientApp::Start(std::string server_url) {
  fidl::InterfaceHandle<fuchsia::io::Directory> directory;
  fuchsia::sys::LaunchInfo launch_info;
  launch_info.url = server_url;
  launch_info.directory_request = directory.NewRequest().TakeChannel();
  fuchsia::sys::LauncherPtr launcher;
  context_->svc()->Connect(launcher.NewRequest());
  launcher->CreateComponent(std::move(launch_info), controller_.NewRequest());
  sys::ServiceDirectory rot13_provider(std::move(directory));
  rot13_provider.Connect(rot13_.NewRequest());
}
}  // namespace rot13
