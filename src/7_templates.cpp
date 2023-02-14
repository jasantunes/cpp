#include <memory>
#include <vector>
#include <iostream>
#include <chrono>


namespace foo {

struct Var {
  Var(int i) {
    value = i;
  }
  Var(std::string i) {
    value = std::stoi(i);
  }
  int value;
};

struct Data : Var {
  Data(int i) : Var(i) {}
  Data() : Var(0) {}
  std::string to_string() {
    return "Data";
  }
};

template <typename T>
class Foo {
  public:
  explicit Foo() {}
  explicit Foo(T data) {}

  void set();

  template <typename V>
  V get(T i);

  std::string get(T i);
};
} // namespace foo

namespace foo {
  template <typename T>
  void Foo<T>::set() {}

  template <typename T>
  template <typename V>
  V Foo<T>::get(T i) {
    return V(i);
  }

  template <typename T>
  std::string Foo<T>::get(T i) {
    return i.to_string();
  }

} // namespace foo

namespace foo {
  template <>
  std::string Foo<int>::get(int i) {
    return "template specialization: string get(): " + std::to_string(i);
  }

  template <>
  template <typename V>
  V Foo<int>::get(int i) {
    std::cout << "template specialization: V get(): ";
    return V(i);
  }
} // namespace foo

using FooInt = foo::Foo<int>;

using namespace foo;
int main() {

  {
    std::cout << "===== template =====" << std::endl;
    auto f = foo::Foo<Data>(Data());
    auto two = Data(2);
    f.set();
    std::cout << "V get<Var>() -> " << f.get<Var>(two).value << std::endl;
    std::string out1 = f.get(4);
    std::cout << "string get() -> " << out1 << std::endl;
  }

  {
    std::cout << "===== specialized template =====" << std::endl;
    auto f = foo::Foo<int>();
    f.set();
    std::cout << "V get<Var>() -> " << f.get<Var>(2).value << std::endl;
    std::string out1 = f.get(4);
    std::cout << "string get() -> " << out1 << std::endl;
  }

  {
    std::cout << "===== FooInt =====" << std::endl;
    auto f = FooInt();
    f.set();
    std::cout << "V get<Var>() -> " << f.get<Var>(2).value << std::endl;
    std::string out1 = f.get(4);
    std::cout << "string get() -> " << out1 << std::endl;
  }


  return 0;
}
