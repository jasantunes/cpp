#include <memory>
#include <vector>
#include <iostream>
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <range/v3/all.hpp>
#include <folly/Benchmark.h>

using optional_strings_t = std::array<std::optional<std::string>, 2>;

template<typename T>
inline bool hasValue(const std::optional<T>& param) {
  return bool{param};
}

constexpr auto filter_string = ranges::views::filter([](const std::optional<std::string>& elem){return elem.has_value();});
constexpr auto filter_sv = ranges::views::filter(hasValue<std::string_view>);

template<typename T>
inline ranges::filter_view<ranges::ref_view<std::array<std::optional<T>, 2>>, bool (*)(const std::optional<T> &)> getRangeView(const T& elem0, const T& elem1) {
  std::array<std::optional<T>, 2> arr  = (elem0 != elem1)
                  ? std::array<std::optional<T>, 2>{elem0, elem1}
                  : std::array<std::optional<T>, 2>{elem0, std::nullopt};
  return arr | ranges::views::filter(hasValue<T>);
}

template<typename T>
inline std::array<std::optional<T>, 2> buildArray(const T& elem0, const T& elem1) {
  return (elem0 != elem1)
                  ? std::array<std::optional<T>, 2>{elem0, elem1}
                  : std::array<std::optional<T>, 2>{elem0, std::nullopt};
}

class Foo {
  public:
  Foo (const std::string& prefix) : prefix_(prefix) {}
  std::string prefix_;
};

auto foo = Foo{"p1"};
auto prefix = "p0";
std::string res;

BENCHMARK(baseline) {
  res = prefix;
  if (prefix != foo.prefix_) {
    res = foo.prefix_;
  }
}

BENCHMARK(vector) {
  std::vector<std::string> prefixes;
  prefixes.push_back(foo.prefix_);
  if (foo.prefix_ != prefix) {
    prefixes.push_back(prefix);
  }

  for (const auto& p : prefixes) {
      res = p;
  }
}

BENCHMARK(static_vector) {
  std::vector<std::string> prefixes;
  BENCHMARK_SUSPEND {
    prefixes.reserve(5);
  }
  prefixes.push_back(foo.prefix_);
  if (foo.prefix_ != prefix) {
    prefixes.push_back(prefix);
  }

  for (const auto& p : prefixes) {
      res = p;
  }
}

BENCHMARK(static_vector2) {
  std::unique_ptr<std::vector<std::string>> prefixes;
  BENCHMARK_SUSPEND {
    prefixes = std::make_unique<std::vector<std::string>>();
    prefixes->reserve(2);
  }
  prefixes->push_back(foo.prefix_);
  if (foo.prefix_ != prefix) {
    prefixes->push_back(prefix);
  }

  for (const auto& p : *prefixes) {
      res = p;
  }
}

BENCHMARK(arrays_with_optional) {
  optional_strings_t prefixes  = (foo.prefix_ != prefix) ? optional_strings_t{foo.prefix_, prefix} : optional_strings_t{foo.prefix_, std::nullopt};
  for (const auto& p : prefixes) {
    if (p)
      res = *p;
  }
}

BENCHMARK(arrays_with_range_filter) {
  optional_strings_t prefixes  = (foo.prefix_ != prefix) ? optional_strings_t{foo.prefix_, prefix} : optional_strings_t{foo.prefix_, std::nullopt};
  for (const auto& p : prefixes | ranges::views::filter(hasValue<std::string>)) {
    res = *p;
  }
}

BENCHMARK(arrays_with_range_filter_string_bug) {
  for (const auto& p : getRangeView<std::string>(foo.prefix_, prefix)) {
    res = *p;
  }
}

BENCHMARK(arrays_with_range_filter_string) {
  auto arr = buildArray<std::string>(foo.prefix_, prefix);
  for (const auto& p : arr | filter_string) {
    res = *p;
  }
}

BENCHMARK(arrays_with_range_filter_string_view) {
  auto arr = buildArray<std::string_view>(foo.prefix_, prefix);
  for (const auto& p : arr | filter_sv) {
    res = *p;
  }
}

int main() {
  folly::runBenchmarks();
}
