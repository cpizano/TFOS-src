// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "hello_shared.h"
#include "hello_static.h"

void *pthread_example(void *arg) {
  printf("pthreads: hello\n");
  return NULL;
}

void cppthread_example() {
  int count = 0;
  std::cout << "cppthreads: hello" << std::endl;
}

int main(int argc, char *argv[]) {
  pthread_t t1;
  pthread_create(&t1, NULL, &pthread_example, NULL);
  std::thread t2(cppthread_example);

  printf("%s, %s\n", GetStaticText(), GetSharedText());

  pthread_join(t1, NULL);
  t2.join();

  return 0;
}
