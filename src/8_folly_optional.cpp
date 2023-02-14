#include <iostream>
#include <gtest/gtest.h>
#include <folly/Optional.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/Sleep.h>
// #include <folly/experimental/coro/Collect.h>
#include <chrono>
#include <vector>

using namespace folly::coro;

Task<int> task1() {
  std::cout << __func__ << ": Waiting..." << std::endl;
  co_await sleep(std::chrono::seconds{1});
  std::cout << __func__ << ": Waiting... done" << std::endl;
  co_return 1;
}
Task<int> task2() {
  std::cout << __func__ << ": Waiting..." << std::endl;
  co_await sleep(std::chrono::seconds{1});
  std::cout << __func__ << ": Waiting... done" << std::endl;
  co_return 2;
}

Task<int> coroRunsSequentially() {
  folly::Optional<Task<int>> t1 = task1();
  auto t2 = task2();
  int result1 = co_await std::move(*t1);
  int result2 = co_await std::move(t2);
  co_return result1 + result2;
}

int main(int argc, char** argv) {
  std::cout << "FOLLY_CPLUSPLUS: " << FOLLY_CPLUSPLUS << std::endl;
  std::cout << "FOLLY_HAS_COROUTINES: " << FOLLY_HAS_COROUTINES << std::endl;

  auto executor = folly::getGlobalCPUExecutor();

  // Running examples
  blockingWait(coroRunsSequentially().scheduleOn(executor.get()));

  std::cout << "===== Done" << std::endl;

  return 0;
}
