#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>

// Don't use `#define` or `static const`, instead use `static constexpr`.
static constexpr int THRESHOLD{5};

class Interface {
 public:
  virtual size_t getSize() const = 0; // Pure virtual function
  virtual ~Interface() = default; // TODO: why do we need this?
};

class Base : public Interface {
 public:
  Base(int param1) : member1_(param1) {}

  // Always mark read-only methods with `const`.
  size_t getSize() const {
    return (size_t)member1_;
  }

  // Factory Pattern
  static std::shared_ptr<Base> factoryCreate(int param) {
    return std::make_shared<Base>(param);
  }

 protected:
  // TODO: Should we always initialize members explictly?
  int member1_{};
};

class Derived : public Base {
 public:
  // Parameters should be passed by ref unless they're basic types.
  Derived(const std::string& param2) : Base(0), member2_(param2) {}

  // OK to override because method is virtual.
  size_t getSize() const override {
    return member2_.size();
  }

 private:
  const std::string member2_{};
};

int foo(int /*unused*/) {
  return 0;
}

int main() {
  Derived derived("foo");
  constexpr std::array<int, 10> myArray{7, 4, 9, 1, 6, 2, 3, 5, 8, 0};

  // Prefer auto over explicitly specifying the type.
  auto n = sizeof(myArray) / sizeof(myArray[0]);
  std::cout << "Array with " << n << " elements." << std::endl;

  // Prefer ranged-for loops over old loops.
  std::cout << "Larger than 5" << std::endl;
  for (const auto& elem : myArray) {
    if (elem > 5) {
      std::cout << "elem: " << elem << std::endl;
    }
  }

  // Or use functional programming <algorithms> over loops.
  std::cout << "Smaller than 5" << std::endl;
  auto print_if_less_than_5 = [&](const auto& elem) {
    if (elem < 5) {
      std::cout << "elem: " << elem << std::endl;
    }
  };
  std::for_each(myArray.begin(), myArray.end(), print_if_less_than_5);

  std::cout << "Hello World! (" << THRESHOLD << ")" << std::endl;
  return 0;
}
