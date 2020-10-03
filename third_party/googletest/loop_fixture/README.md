# Test loop fixture

If you have code that runs using the async-cpp library, you can use this test
loop fixture to drive your code.

The canonical source for this library is the Fuchsia source tree at
[//src/lib/testing/loop_fixture](https://fuchsia.googlesource.com/fuchsia/+/refs/heads/master/src/lib/testing/loop_fixture/).

## Using the loop fixture

Add the loop fixture as a dependency of your test.

```gn
import("//third_party/googletest/loop_fixture")
```

Add the following to your test code:

* The include statement.
* A subclass of TestLoopFixture.
* A call to run the loop.

```cpp
#include <third_party/googletest/loop_fixture/test_loop_fixture.h>

// The fixture for testing the Engine class.
class EngineDeviceUnitTest : public gtest::TestLoopFixture {
 public:
  void SetUp() override {
    TestLoopFixture::SetUp();
    // Other setup.
  }

  void TearDown() override {
    // Other teardown.
    TestLoopFixture::TearDown();
  }
}

TEST_F(EngineDeviceUnitTest, Negation) {
  // Call function under test.

  RunLoopUntilIdle();

  // Do verifications.
}
```
