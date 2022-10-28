#include <memory>

int main() {
  // auto_ptr is deprecated.
  // std::auto_ptr<int> deprecated;

  // Prefer std::unique_ptr over std::shared_ptr if possible.
  // Use unique_ptr when you want a single pointer to an object that will be
  // reclaimed when that single pointer is destroyed.
  // https://google.github.io/styleguide/cppguide.html#Ownership_and_Smart_Pointers
  std::unique_ptr<int> p1 = std::make_unique<int>(42);
  // auto p2 = p1;          // error: can't copy unique_ptr
  auto p2 = std::move(p1); // okay, but transfers ownership

  // Use shared_ptr when you want multiple pointers to the same resource.
  std::shared_ptr<int> p3 = std::make_shared<int>(11);
  std::shared_ptr<int> p4 = p3; // Both p3 and p4 point to the same resource.

  // Use weak_ptr when you want an additional copy to a shared_ptr that
  // does not participate in reference counting.
  std::weak_ptr<int> p5{p4}; // Both p5 and p4 point to the same resource.
}
