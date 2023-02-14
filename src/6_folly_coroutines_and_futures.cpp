#include <iostream>
#include <gtest/gtest.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/Generator.h>
#include <folly/experimental/coro/Sleep.h>
#include <folly/experimental/coro/Collect.h>
#include <chrono>
#include <vector>

using namespace folly::coro;

Task<void> coroThrowsException() {
  throw std::logic_error("Expected");
  co_return;
}

Task<void> coroHandlesException() {
  auto future42 = folly::makeSemiFuture(42);
  EXPECT_EQ(42, co_await std::move(future42));

  // Keep in mind that an Awaitable may result in an exception,
  // or you'll have to use try-catch blocks to handle errors.
  try {
    co_await coroThrowsException();
  } catch (const std::logic_error&) {
    std::cerr << "Catching the exception" << std::endl;
  }
}


// Running coroutines sequentially vs concurrently.
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
  auto t1 = task1();
  auto t2 = task2();
  int result1 = co_await std::move(t1);
  int result2 = co_await std::move(t2);
  co_return result1 + result2;
}

Task<int> coroRunsConcurrently() {
  auto [result1, result2] =
      co_await collectAll(task1(), task2());
  co_return result1 + result2;
}

Task<int> coroRunsLambda() {
  // Use co_invoke to invoke the lambda to prevent the lambda from being destroyed.
  auto t1 = co_invoke([]() -> Task<int> {
    auto inner_result = co_await task1();
    co_return inner_result;
  });
  auto t2 = task2();

  auto [result1, result2] =
    co_await collectAll(std::move(t1), std::move(t2));
  co_return result1 + result2;
}


Generator<int> generator() {
  for (int i=0; i<10; i++) {
    co_yield i;
  }
}

Task<int> coroGenerator(std::string name) {
  auto gen = generator();
  int result = 0;
  for (auto i : gen) {
    std::cout << "generator: " << i << " (" << name << ")" << std::endl;
    result += i;
    co_await sleep(std::chrono::milliseconds{200});
  }
  co_return result;
}



int main(int argc, char** argv) {
  std::cout << "FOLLY_CPLUSPLUS: " << FOLLY_CPLUSPLUS << std::endl;
  std::cout << "FOLLY_HAS_COROUTINES: " << FOLLY_HAS_COROUTINES << std::endl;

  auto executor = folly::getGlobalCPUExecutor();

  // Running examples

  std::cout << "===== coroHandlesException()" << std::endl;
  blockingWait(coroHandlesException().scheduleOn(executor.get()));
  std::cout << "===== coroRunsSequentially()" << std::endl;
  blockingWait(coroRunsSequentially().scheduleOn(executor.get()));
  std::cout << "===== coroRunsConcurrently()" << std::endl;
  blockingWait(coroRunsConcurrently().scheduleOn(executor.get()));
  std::cout << "===== coroRunsLambda()" << std::endl;
  blockingWait(coroRunsLambda().scheduleOn(executor.get()));
  std::cout << "===== coroGenerator()" << std::endl;
  blockingWait(coroGenerator("single task").scheduleOn(executor.get()));

  std::cout << "===== coroGenerator() x 3" << std::endl;
  std::vector<Task<int>> tasks;
  tasks.push_back(coroGenerator("task 1"));
  tasks.push_back(coroGenerator("task 2"));
  tasks.push_back(coroGenerator("task 3"));
  blockingWait(co_invoke([&]() -> Task<void> {
      co_await collectAllRange(std::move(tasks));
  }));

  std::cout << "===== Task with executor" << std::endl;



  std::cout << "===== Done" << std::endl;

  return 0;
}
