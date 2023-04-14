#include <chrono>
#include <iostream>
#include <vector>

#include <folly/experimental/coro/AsyncScope.h>
#include <folly/experimental/coro/Baton.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>
#include <folly/experimental/coro/Sleep.h>
#include <folly/experimental/coro/Task.h>
#include <folly/fibers/Semaphore.h>
#include <folly/futures/Future.h>


using namespace std::chrono_literals;

// Running coroutines sequentially vs concurrently.
folly::SemiFuture<int> semiFutureTask(int val) {
  return folly::makeSemiFuture().deferValue([=](auto) {
    std::cout << "task " << val << ": Executing..." << std::endl;
    std::cout << "task " << val << ": done" << std::endl;
    return val;
  });
}

folly::coro::Task<int> coroTask(int val) {
  std::cout << "task " << val << ": Executing..." << std::endl;
  std::cout << "task " << val << ": done" << std::endl;
  co_return val;
}

folly::coro::Task<int> coroTaskWithBaton(int val, folly::coro::Baton& baton) {
  std::cout << "task " << val << ": Executing..." << std::endl;
  co_await baton;
  std::cout << "task " << val << ": done" << std::endl;
  co_return val;
}

folly::coro::Task<int> coroTaskWithBatonAndSemaphore(int val, folly::coro::Baton& baton, folly::fibers::Semaphore& semaphore) {
  semaphore.signal();
  std::cout << "task " << val << ": Executing..." << std::endl;
  co_await baton;
  std::cout << "task " << val << ": done" << std::endl;
  co_return val;
}


int main(int argc, char** argv) {
  auto executor = folly::getGlobalCPUExecutor();

  // Running examples

  {
    std::cout << "===== coroutines not synchronized" << std::endl;
    folly::coro::AsyncScope scope;
    std::vector<folly::coro::Task<int>> tasks;
    tasks.emplace_back(coroTask(1));
    tasks.emplace_back(coroTask(2));
    tasks.emplace_back(coroTask(3));

    scope.add(folly::coro::co_invoke([&]() -> folly::coro::Task<void> {
        co_await collectAllRange(std::move(tasks));
    }).scheduleOn(executor));

    folly::coro::blockingWait([&scope]()->folly::coro::Task<> {co_await scope.joinAsync();}().scheduleOn(executor.get()));
  }


  {
    std::cout << "===== coroutines synchronized with folly::coro::Baton" << std::endl;
    folly::coro::AsyncScope scope;
    folly::coro::Baton baton1, baton2, baton3;
    std::vector<folly::coro::Task<int>> tasks;
    tasks.emplace_back(coroTaskWithBaton(1, baton1));
    tasks.emplace_back(coroTaskWithBaton(2, baton2));
    tasks.emplace_back(coroTaskWithBaton(3, baton3));

    scope.add(folly::coro::co_invoke([&]() -> folly::coro::Task<void> {
        co_await collectAllRange(std::move(tasks));
    }).scheduleOn(executor));

    std::cout << "Sleeping for 1 sec to wait for coroutines to start waiting..." << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Calling baton.post()" << std::endl;
    baton3.post();
    baton1.post();
    baton2.post();
    folly::coro::blockingWait([&scope]()->folly::coro::Task<> {co_await scope.joinAsync();}().scheduleOn(executor.get()));
  }

  {
    std::cout << "===== coroutines synchronized with folly::coro::Baton and folly::fibers::Semaphore" << std::endl;
    folly::coro::AsyncScope scope;
    folly::fibers::Semaphore semaphore{3};
    folly::coro::Baton baton1, baton2, baton3;
    std::vector<folly::coro::Task<int>> tasks;
    tasks.emplace_back(coroTaskWithBatonAndSemaphore(1, baton1, semaphore));
    tasks.emplace_back(coroTaskWithBatonAndSemaphore(2, baton2, semaphore));
    tasks.emplace_back(coroTaskWithBatonAndSemaphore(3, baton3, semaphore));

    scope.add(folly::coro::co_invoke([&]() -> folly::coro::Task<void> {
        co_await collectAllRange(std::move(tasks));
    }).scheduleOn(executor));

    folly::coro::blockingWait([&semaphore]()->folly::coro::Task<> {co_await semaphore.co_wait();}().scheduleOn(executor.get()));
    baton3.post();
    baton1.post();
    baton2.post();
    folly::coro::blockingWait([&scope]()->folly::coro::Task<> {co_await scope.joinAsync();}().scheduleOn(executor.get()));
  }

  std::cout << "===== Done" << std::endl;

  return 0;
}
