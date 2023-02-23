#include <iostream>
#include <string>

struct Var {
  Var(int i) {
    value = i;
  }
  Var(std::string i) {
    value = std::stoi(i);
  }
  int value;
};

namespace foo {
class Foo {
public:
  explicit Foo() : data(0) {}
  explicit Foo(int data) : data(data) {}
  void set(int data) { data = data; }

  // nonspecialized template member function
  template <typename V> V get();

  // specialized member function
  std::string get();

protected:
  int data;
};

// nonspecialized template member function
template <typename V>
V Foo::get() {
  std::cout << "calling template method" << std::endl;
  return V(data);
}

// specialized member function
template <>
std::string Foo::get() {
  std::cout << "calling specialized template method" << std::endl;
  return "";
}
} // namespace foo


struct Type {};

namespace bar {
template <typename T>
class Bar {
public:
  explicit Bar() : data(0) {}
  explicit Bar(int data) : data(data) {}
  void set(int data) { data = data; }

  // nonspecialized template member function
  template <typename V> V get();

  // specialized member function
  // std::string get();
  // cannot specialize (with 'template<>') a member of an unspecialized template
  // instead we create a regular function

protected:
  int data;
};

// nonspecialized template function (non-member)
template <typename V>
V get_(int data) {
  std::cout << "calling template method" << std::endl;
  return V(data);
}
// specialized template function (non-member)
template <>
std::string get_(int data) {
  std::cout << "calling specialized template method" << std::endl;
  return "";
}

// nonspecialized template member function that calls (non)specialized template function
template <typename T>
template <typename V>
V Bar<T>::get() {
  return get_<V>(data);
}
} // namespace bar

using namespace foo;
using namespace bar;
int main() {

  {
    std::cout << "===== regular class w/ template method =====" << std::endl;
    auto f = Foo(10);
    f.set(20);
    std::string out = f.get<std::string>();
    Var v = f.get<Var>();
  }

  {
    std::cout << "===== template class w/ template method =====" << std::endl;
    auto b = Bar<Type>(10);
    b.set(20);
    std::string out = b.get<std::string>();
    Var v = b.get<Var>();
  }

  {
    using BarType = Bar<Type>;
    std::cout << "===== template class w/ template method =====" << std::endl;
    auto b = BarType(10);
    b.set(20);
    std::string out = b.get<std::string>();
    Var v = b.get<Var>();
  }

  return 0;
}
