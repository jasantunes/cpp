#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


// Don't use `#define` or `static const`, instead use `static constexpr`.
static constexpr int BASE{2};

class IFoo {
 public:
  virtual size_t getSize() const = 0; // Pure virtual function
  virtual ~IFoo() = default; // TODO: why do we need this?
};

class Foo : public IFoo {
 public:
  Foo(int param1) : member1_(param1) {}

  // Always mark read-only methods with `const`.
  size_t getSize() const {
    return (size_t)member1_;
  }

  // Factory Pattern
  static std::shared_ptr<Foo> factoryCreate(int param) {
    return std::make_shared<Foo>(param);
  }

 protected:
  // Always initialize members explictly
  int member1_{};
};

class Bar : public Foo {
 public:
  // Parameters in constructor should be passes by value and then moved.
  Bar(std::string param2) : Foo(0), member2_(std::move(param2)) {}

  // OK to override because method is virtual.
  size_t getSize() const override {
    return member2_.size();
  }

  // Parameters should be passed by ref unless they're basic types.
  void setMember(const std::string& val) {
    member2_ = val;
  }

 private:
  std::string member2_{};
};

int foo(int /*unused*/) {
  return 0;
}

int main() {
  Bar derived("foo");
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

  // Use auto to make code more readable and prevent unwanted casts.
  std::vector<int> vec{1, 2, 3, 4, 5};
  const auto s = vec.size();
  std::cout << "size of vector: " << s << std::endl;

  // Prefer range loops, instead of iterators or indices.
  // Also prefer `for` to `while` statements.
  for (auto& val : vec) {
    val *= BASE; // double it
  }

  // Or use functional programming <algorithms> over loops.
  auto double_it = [=](int i) { return BASE * i; };
  std::transform(vec.begin(), vec.end(), vec.begin(), double_it);


  // Some standard well-known containers.
  std::unordered_map<int, std::string> hash_table;
  hash_table[10] = "ten";
  std::cout << hash_table.at(10) << std::endl;

  std::unordered_set<int> set;
  set.insert(10);
  if (set.find(10) != set.end()) {
    std::cout << "Set contains key" << std::endl;
  }
  return 0;
}
