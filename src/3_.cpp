#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

static constexpr int BASE{2};

int main() {
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

  //////////////////////////////////////////////////////////////////////////////
  // Smart pointers.
  //////////////////////////////////////////////////////////////////////////////

  // auto_ptr is deprecated.
  // std::auto_ptr<int> deprecated;

  // Use unique_ptr when you want a single pointer to an object that will be
  // reclaimed when that single pointer is destroyed.
  std::unique_ptr<int> p1 = std::make_unique<int>(42);
  // auto p2 = p1;          // error: can't copy unique_ptr
  auto p2 = std::move(p1); // okay, but transfers ownership

  // Use shared_ptr when you want multiple pointers to the same resource.
  std::shared_ptr<int> p3 = std::make_shared<int>(11);
  std::shared_ptr<int> p4 = p3; // Both p3 and p4 point to the same resource.

  // Use weak_ptr when you want an additional copy to a shared_ptr that
  // does not participate in reference counting.
  std::weak_ptr<int> p5{p4}; // Both p5 and p4 point to the same resource.

  // Some standard well-known containers.
  std::unordered_map<int, std::string> hash_table;
  hash_table[10] = "ten";
  std::cout << hash_table.at(10) << std::endl;

  std::unordered_set<int> set;
  set.insert(10);
  if (set.find(10) != set.end()) {
    std::cout << "Set contains key" << std::endl;
  }
}
