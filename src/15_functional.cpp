#include <memory>
#include <vector>
#include <iostream>
#include <array>
#include <optional>
#include <algorithm> // copy
#include <numeric>


bool foo(std::string& str) {
  str.assign("new value");
  return true;
}
auto comma = [](std::string a, std::string b) {
    return (a.empty()) ? std::move(b) : std::move(a) + ", " + std::move(b);
};
int main() {
  std::string str{};
  std::cout << "str: " << str << std::endl;
  if (foo(str)) {
    std::cout << "modified to: " << str << std::endl;
  }

  auto vec = std::vector<std::string>{{"key1", "key2"}};

  {
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(std::cout, ", "));
    std::cout << std::endl;
  }

  {
    std::string s = std::accumulate(vec.begin(), vec.end(), std::string{}, comma);
    std::cout << "vec: " << s << std::endl;
  }

  {
    std::string s = std::reduce(vec.begin(), vec.end(), std::string{}, comma);
    std::cout << "vec: " << s << std::endl;
  }


}

