#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Lifecycle {
  Lifecycle() {
    std::cout << "default ctor" << std::endl;
  }
  ~Lifecycle() {
    std::cout << "dtor" << std::endl;
  }
  Lifecycle(const Lifecycle& /* rhs */) {
    std::cout << "copy ctor" << std::endl;
  }
  Lifecycle(Lifecycle&& /* rhs */) {
    std::cout << "move ctor" << std::endl;
  }
  Lifecycle& operator=(const Lifecycle&) {
    std::cout << "copy assignment" << std::endl;
    return *this;
  }
  Lifecycle& operator=(Lifecycle&&) {
    std::cout << "move assignment" << std::endl;
    return *this;
  }
};

struct A {
  A(std::string) {}
  Lifecycle l;
};

A return_named_variable() {
  A named_var("42");
  return named_var;
}

A return_unnamed_variable() {
  return A("42");
}

A& return_move_rvalue(A&& rvalue) {
  return rvalue;
}

A return_move_lvalue(A& rvalue) {
  return std::move(rvalue);
}

int main() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
  // No copy/move ctors
  std::cout << "##############################" << std::endl;
  {
    std::cout << "# temporary materialization (1): no actual copy or move"
              << std::endl;
    A x = A("x");
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout
        << "# temporary materialization (2): no actual copy or move (NRVO)"
        << std::endl;
    A x = return_named_variable();
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout
        << "# temporary materialization (3): no actual copy or move (NRVO)"
        << std::endl;
    A x = return_unnamed_variable();
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout << "# using ref" << std::endl;
    A x = A("x");
    A& y = x; // it's a reference
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout << "# using copy ctor" << std::endl;
    A x = A("x");
    A y = x;
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout << "# using move ctor" << std::endl;
    A x = A("x");
    A y = std::move(x);
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout << "# using copy assignment" << std::endl;
    A x = A("x");
    A y = A("y");
    x = y;
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout << "# using move assignment" << std::endl;
    A x = A("x");
    x = A("y");
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout << "# using return move (rvalue)" << std::endl;
    return_move_rvalue(A("x"));
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout << "# using return move (rvalue 2)" << std::endl;
    A x = A("x");
    return_move_rvalue(std::move(x));
  }

  std::cout << "##############################" << std::endl;
  {
    std::cout << "# using return move (lvalue)" << std::endl;
    A x = A("x");
    return_move_lvalue(x);
  }

  std::cout << "##############################" << std::endl;
#pragma GCC diagnostic pop

  return 0;
}
