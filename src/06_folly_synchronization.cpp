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



using namespace std::chrono_literals;
using namespace folly::coro;

constexpr auto kTimeout = 5s;

// Running coroutines sequentially vs concurrently.
Task<int> coroTask(const std::string& name) {
  std::cout << name << ": Executing..." << std::endl;
  std::cout << name << ": done" << std::endl;
  co_return 1;
}

Task<int> coroTaskWithBaton(const std::string& name, Baton& baton) {
  std::cout << name << ": Executing..." << std::endl;
  co_await baton;
  std::cout << name << ": done" << std::endl;
  co_return 1;
}

Task<int> coroTaskWithBatonAndSemaphore(const std::string& name, Baton& baton, folly::fibers::Semaphore& semaphore) {
  semaphore.signal();
  std::cout << name << ": Executing..." << std::endl;
  co_await baton;
  std::cout << name << ": done" << std::endl;
  co_return 1;
}


int main(int argc, char** argv) {
  auto executor = folly::getGlobalCPUExecutor();

  // Running examples

  {
    std::cout << "===== coroutines not synchronized" << std::endl;
    folly::coro::AsyncScope scope;
    std::vector<Task<int>> tasks;
    tasks.emplace_back(coroTask("task 1"));
    tasks.emplace_back(coroTask("task 2"));
    tasks.emplace_back(coroTask("task 3"));

    scope.add(co_invoke([&]() -> Task<void> {
        co_await collectAllRange(std::move(tasks));
    }).scheduleOn(executor));

    blockingWait([&scope]()->Task<> {co_await scope.joinAsync();}().scheduleOn(executor.get()));
  }


  {
    std::cout << "===== coroutines synchronized with folly::coro::Baton" << std::endl;
    folly::coro::AsyncScope scope;
    folly::coro::Baton baton1, baton2, baton3;
    std::vector<Task<int>> tasks;
    tasks.emplace_back(coroTaskWithBaton("task 1", baton1));
    tasks.emplace_back(coroTaskWithBaton("task 2", baton2));
    tasks.emplace_back(coroTaskWithBaton("task 3", baton3));

    scope.add(co_invoke([&]() -> Task<void> {
        co_await collectAllRange(std::move(tasks));
    }).scheduleOn(executor));

    std::cout << "Sleeping for 1 sec to wait for coroutines to start waiting..." << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Calling baton.post()" << std::endl;
    baton3.post();
    baton1.post();
    baton2.post();
    blockingWait([&scope]()->Task<> {co_await scope.joinAsync();}().scheduleOn(executor.get()));
  }

  {
    std::cout << "===== coroutines synchronized with folly::coro::Baton and folly::fibers::Semaphore" << std::endl;
    folly::coro::AsyncScope scope;
    folly::fibers::Semaphore semaphore{3};
    folly::coro::Baton baton1, baton2, baton3;
    std::vector<Task<int>> tasks;
    tasks.emplace_back(coroTaskWithBatonAndSemaphore("task 1", baton1, semaphore));
    tasks.emplace_back(coroTaskWithBatonAndSemaphore("task 2", baton2, semaphore));
    tasks.emplace_back(coroTaskWithBatonAndSemaphore("task 3", baton3, semaphore));

    scope.add(co_invoke([&]() -> Task<void> {
        co_await collectAllRange(std::move(tasks));
    }).scheduleOn(executor));

    blockingWait([&semaphore]()->Task<> {co_await semaphore.co_wait();;}().scheduleOn(executor.get()));
    // semaphore.wait();
    baton3.post();
    baton1.post();
    baton2.post();
    blockingWait([&scope]()->Task<> {co_await scope.joinAsync();}().scheduleOn(executor.get()));
  }

  std::cout << "===== Done" << std::endl;

  return 0;
}
