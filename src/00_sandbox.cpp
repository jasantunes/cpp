#include <memory>
#include <vector>
#include <iostream>

#include "00_sandbox.h"

using namespace std;


void Sandbox::foo(Foo foo) {
  return;
}


int main() {
  auto foo = Foo();
  auto sandbox = Sandbox();
  sandbox.foo(foo);

  std::vector<bool> vec(10, true);
  for (auto i : vec) {
    std::cout << i << std::endl;
  }
}

