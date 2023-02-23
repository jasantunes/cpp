#include <memory>
#include <vector>
#include <iostream>
#include <fmt/format.h>

using namespace std;

template <typename T>
class Foo {
};

class Bar : public Foo<int> {
};

int main() {
  {
    int var = -10;
    std::cout << fmt::format("var: {}", var) << std::endl;
  }

}

