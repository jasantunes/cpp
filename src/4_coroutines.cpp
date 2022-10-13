#include <folly/executors/ThreadedExecutor.h>
#include <folly/experimental/coro/Task.h>
#include <folly/futures/Future.h>
#include <iostream>
#include <chrono>

void foo(int x) {
  // do something with x
  std::cout << "foo(" << x << ")" << std::endl;
}

// Coroutine function that returns an RAII container for the coro handle.
// The coroutine frame and handle are created when the function is called.
folly::coro::Task<int> task42() {
  co_return 42;
}

folly::coro::Task<int> taskSlow43() {
  auto duration = std::chrono::seconds{1};
  std::cout << duration.count() << std::endl;
  co_await folly::futures::sleep(duration);
  // co_return co_await keywords:
  //      turns the function into a coroutine and triggers compiler
  //      transformations on the fn body.
  co_return co_await task42() + 1;
}

int main() {
  // ...
  folly::ThreadedExecutor executor;
  std::cout << "making Promise" << std::endl;
  folly::Promise<int> p;
  folly::Future<int> f = p.getSemiFuture().via(&executor);
  auto f2 = std::move(f).thenValue(foo);
  std::cout << "Future chain made" << std::endl;

  // ... now perhaps in another event callback

  std::cout << "fulfilling Promise" << std::endl;
  p.setValue(42);
  std::move(f2).get();
  std::cout << "Promise fulfilled" << std::endl;

  // Coroutines

  return 0;
}
