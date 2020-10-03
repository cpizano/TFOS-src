// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "client.h"

namespace calculator_cli {
CalculatorClient::CalculatorClient() : CalculatorClient(sys::ComponentContext::CreateAndServeOutgoingDirectory()) {}

CalculatorClient::CalculatorClient(std::unique_ptr<sys::ComponentContext> context)
    : context_(std::move(context)) {}

void CalculatorClient::Start(std::string server_url) {
  // TODO(fxb/42963): Add error checking.
  fidl::InterfaceHandle<fuchsia::io::Directory> directory;
  fuchsia::sys::LaunchInfo launch_info;
  launch_info.url = server_url;
  launch_info.directory_request = directory.NewRequest().TakeChannel();
  fuchsia::sys::LauncherPtr launcher;
  context_->svc()->Connect(launcher.NewRequest());
  launcher->CreateComponent(std::move(launch_info), controller_.NewRequest());
  sys::ServiceDirectory calculator_provider(std::move(directory));
  calculator_provider.Connect(calculator_.NewRequest());
}
}  // namespace calculator_cli
