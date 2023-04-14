#include <chrono>
#include <iostream>
#include <vector>

#include <folly/experimental/coro/Baton.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>
#include <folly/experimental/coro/Generator.h>
#include <folly/experimental/coro/Sleep.h>
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/Timeout.h>
#include <gtest/gtest.h>


using namespace std::chrono_literals;
using namespace folly::coro;

constexpr auto kTimeout = 5s;

Task<void> coroThrowsException() {
  throw std::logic_error("Expected");
  co_return;
}

Task<void> coroHandlesException() {
  auto future42 = folly::makeSemiFuture(42);
  EXPECT_EQ(42, co_await std::move(future42));

  // Keep in mind that an Awaitable may result in an exception,
  // o you'll have to use try-catch blocks to handle errors.
  try {
    co_await coroThrowsException();
  } catch (const std::logic_error&) {
    std::cerr << "Catching the exception" << std::endl;
  }
}


// Running coroutines sequentially vs concurrently.
Task<int> foreverTask() {
  std::cout << __func__ << ": Executing..." << std::endl;
  while (true) {
    co_await sleep(10s);
  }
  std::cout << __func__ << ": Executing... done" << std::endl;
  co_return 1;
}
Task<int> slowerTask() {
  std::cout << __func__ << ": Executing..." << std::endl;
  co_await sleep(1s);
  std::cout << __func__ << ": Executing... done" << std::endl;
  co_return 1;
}
Task<int> fasterTask() {
  std::cout << __func__ << ": Executing..." << std::endl;
  co_await sleep(500ms);
  std::cout << __func__ << ": Executing... done" << std::endl;
  co_return 2;
}


Task<> coroWithBaton(Baton& baton) {
  std::cout << __func__ << ": Waiting for baton..." << std::endl;
  co_await baton;
  std::cout << __func__ << ": Waiting for baton... done" << std::endl;
  co_return;
}

Task<int> coroRunsSequentially() {
  auto t1 = slowerTask();
  auto t2 = fasterTask();
  int result1 = co_await std::move(t1);
  int result2 = co_await std::move(t2);
  co_return result1 + result2;
}

Task<int> coroRunsConcurrently() {
  auto [result1, result2] =
      co_await collectAll(slowerTask(), fasterTask());
  co_return result1 + result2;
}

Task<int> coroReturnEarly() {
  CancellableAsyncScope scope;
  std::vector<Task<int>> tasks;

  auto simpleWrapper = [](auto task) -> Task<int> {
    auto results = co_await co_awaitTry( // try to get a response
        timeout(std::move(task), kTimeout)); // within a timeout
    if (results.hasException()) {
      std::cout << "exception: " << results.exception().what() << std::endl;
    }
    co_return results;
  };

  tasks.emplace_back(simpleWrapper(foreverTask()));
  tasks.emplace_back(simpleWrapper(slowerTask()));
  tasks.emplace_back(simpleWrapper(fasterTask()));

  auto results = makeUnorderedAsyncGenerator(scope, std::move(tasks));
  int result{};
  while (auto next = co_await results.next()) {
    result = next.value();
    break;
  }
  std::cout << "Fastest result: " << result << std::endl;
  std::cout << "Cancelling..." << std::endl;
  co_await scope.cancelAndJoinAsync();
  co_return result;
}

Task<int> coroReturnEarlyButKeepExecuting(folly::CancellationSource& cs) {
  CancellableAsyncScope scope;
  std::vector<Task<int>> tasks;

  auto withCancellationSourceWrapper = [](auto task, auto& cancellationSource)  -> Task<int> {
    auto results = co_await co_awaitTry( // try to get a response
        co_withCancellation( // wrap in co_withCancellation to
            cancellationSource.getToken(), // override the cancellation token
            timeout(std::move(task), kTimeout))); // within a timeout
    if (results.hasException()) {
      std::cout << "exception: " << results.exception().what() << std::endl;
    }
    co_return results;
  };

  tasks.emplace_back(withCancellationSourceWrapper(foreverTask(), cs));
  tasks.emplace_back(withCancellationSourceWrapper(slowerTask(), cs));
  tasks.emplace_back(withCancellationSourceWrapper(fasterTask(), cs));

  auto results = makeUnorderedAsyncGenerator(scope, std::move(tasks));
  int result{};
  while (auto next = co_await results.next()) {
    result = next.value();
    break;
  }
  std::cout << "Fastest result: " << result << std::endl;
  std::cout << "Cancelling..." << std::endl;
  co_await scope.cancelAndJoinAsync();
  co_return result;
}

Task<int> coroRunsLambda() {
  // Use co_invoke to invoke the lambda to prevent the lambda from being destroyed.
  auto t1 = co_invoke([]() -> Task<int> {
    auto inner_result = co_await slowerTask();
    co_return inner_result;
  });
  auto t2 = fasterTask();

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

  {
    folly::coro::AsyncScope scope;
    std::cout << "===== coroWithBaton" << std::endl;
    folly::coro::Baton baton;
    scope.add(coroWithBaton(baton).scheduleOn(executor));
    std::cout << "Sleeping for 1 sec then calling baton.post()" << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Calling baton.post()" << std::endl;
    baton.post();
    blockingWait([&scope]()->Task<> {co_await scope.joinAsync();}().scheduleOn(executor.get()));
  }

  std::cout << "===== coroRunsSequentially()" << std::endl;
  blockingWait(coroRunsSequentially().scheduleOn(executor.get()));

  std::cout << "===== coroRunsConcurrently()" << std::endl;
  blockingWait(coroRunsConcurrently().scheduleOn(executor.get()));

  {
    std::cout << "===== coroRunsConcurrently() - non-blocking" << std::endl;
    TaskWithExecutor task_with_executor = coroRunsConcurrently().scheduleOn(executor);
    auto sf1 = std::move(task_with_executor).start();
    sf1.wait();
  }

  std::cout << "===== coroReturnEarly()" << std::endl;
  blockingWait(coroReturnEarly().scheduleOn(executor.get()));

  {
    folly::CancellationSource cancellationSource;
    std::cout << "===== coroReturnEarlyButKeepExecuting()" << std::endl;
    blockingWait(coroReturnEarlyButKeepExecuting(cancellationSource).scheduleOn(executor.get()));
  }

  std::cout << "===== coroRunsLambda()" << std::endl;
  blockingWait(coroRunsLambda().scheduleOn(executor.get()));

  std::cout << "===== coroGenerator()" << std::endl;
  blockingWait(coroGenerator("single task").scheduleOn(executor.get()));

  {
    std::cout << "===== coroGenerator() x 3" << std::endl;
    std::vector<Task<int>> tasks;
    tasks.push_back(coroGenerator("task 1"));
    tasks.push_back(coroGenerator("task 2"));
    tasks.push_back(coroGenerator("task 3"));
    blockingWait(co_invoke([&]() -> Task<void> {
        co_await collectAllRange(std::move(tasks));
    }));
  }

  std::cout << "===== Done" << std::endl;

  return 0;
}
