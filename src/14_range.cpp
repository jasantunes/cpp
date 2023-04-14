#include <memory>
#include <vector>
#include <iostream>
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <functional> // std::reference_wrapper
#include <range/v3/all.hpp>
#include <folly/Benchmark.h>
#include <initializer_list> // std::initializer_list
#include <span> // std::span

using optional_string_t = std::optional<std::string>;
using optional_ref_string_t = std::optional<std::reference_wrapper<std::string>>;

constexpr auto filter_nonempty = ranges::views::filter([](const std::string& elem){return elem.length()>0;});
constexpr auto filter_valid = ranges::views::filter([](const optional_string_t& elem){return elem.has_value();});
constexpr auto filter_valid_sv = ranges::views::filter([](const std::optional<std::string_view>& elem){return elem.has_value();});
constexpr auto filter_valid2 = ranges::views::filter([](const optional_ref_string_t& elem){return elem.has_value();});
std::string prefix = "prefix3_";
std::string res;

std::array<std::string, 2> arrPrefixes{{"prefix1_", "prefix2_"}};

std::vector<std::string> vecPrefixes{arrPrefixes.begin(), arrPrefixes.end()};
std::vector<optional_string_t> prefixesOptional{arrPrefixes.begin(), arrPrefixes.end()};;


template<typename T>
inline std::array<T, 3> buildTempArray(const std::vector<std::string>& vec, const T& elem) {
  std::array<T, 3> arr{};
  std::copy(std::begin(vec), std::end(vec), std::begin(arr));
  arr[vec.size()] = elem;
  return arr;
}

BENCHMARK(vector_ugly_code) {
  res = prefix;
  res = prefix;

  for (const auto& p : vecPrefixes) {
    for (const auto& s : vecPrefixes) {
      res = p + s;
    }
  }
}

BENCHMARK(vector_mutation) {
  vecPrefixes.push_back(prefix);

  for (const auto& p : vecPrefixes) {
    for (const auto& s : vecPrefixes) {
      res = p + s;
    }
  }

  vecPrefixes.pop_back();
}

BENCHMARK(array_copy_with_optional_and_filter) {
  auto arr = buildTempArray<std::optional<std::string>>(vecPrefixes, prefix);
  for (const auto& p : arr | filter_valid) {
    for (const auto& s : arr | filter_valid) {
      res = *p + *s;
    }
  }
}

BENCHMARK(array_copy_with_optional_ugly_code) {
  auto arr = buildTempArray<std::optional<std::string>>(vecPrefixes, prefix);
  for (auto iter0 = arr.begin(); iter0 != arr.end() && iter0->has_value(); ++iter0) {
    for (auto iter1 = arr.begin(); iter1 != arr.end() && iter1->has_value(); ++iter1) {
      const auto& p = **iter0;
      const auto& s = **iter1;
      res = p + s;
    }
  }
}

BENCHMARK(vector_copy) {
  std::vector<std::string> temp;
  temp.reserve(vecPrefixes.size() + 1);
  auto it = temp.insert(temp.begin(), vecPrefixes.begin(), vecPrefixes.end());
  temp.insert(it, prefix);
  for (const auto& p : temp) {
    for (const auto& s : temp) {
      res = p + s;
    }
  }
}

BENCHMARK(vector_copy_with_refs) {
  std::vector<std::reference_wrapper<std::string>> temp;
  temp.reserve(vecPrefixes.size() + 1);
  auto it = temp.insert(temp.begin(), vecPrefixes.begin(), vecPrefixes.end());
  temp.insert(it, prefix);
  for (const auto& p : temp) {
    for (const auto& s : temp) {
      res = p.get() + s.get();
    }
  }
}

auto lambda = [](const std::vector<std::string>& vec, const std::string& prefix){
  if (prefix.size() == 0) {
    std::array<std::string, 1> arr{};
    return ranges::views::concat(vec, arr);
  }
  std::array<std::string, 1> arr{{prefix}};
  return ranges::views::concat(vec, arr);
};

BENCHMARK(array_range_concat) {
  auto final_rng = lambda(vecPrefixes, prefix);
  // std::array<std::string, 1> arr{{prefix}};
  // auto final_rng = ranges::views::concat(vecPrefixes, arr);
  for (const auto& p : final_rng) {
    for (const auto& s : final_rng) {
        res = p + s;
      }
  }
}

BENCHMARK(array_range_concat_with_optional) {
  std::array<optional_string_t, 1> arr{{optional_string_t{prefix}}};
  auto final_rng = ranges::views::concat(vecPrefixes, arr);
  for (const auto& p : final_rng | filter_valid) {
    for (const auto& s : final_rng | filter_valid) {
        res = *p + *s;
      }
  }
}

BENCHMARK(vector_range_concat_with_optional) {
  std::vector<optional_string_t> arr{optional_string_t{prefix}};
  auto final_rng = ranges::views::concat(vecPrefixes, arr);
  for (const auto& p : final_rng | filter_valid) {
    for (const auto& s : final_rng | filter_valid) {
        res = *p + *s;
      }
  }
}

BENCHMARK(vector_join_view) {
  std::vector<std::vector<std::string>> vec{vecPrefixes, std::vector<std::string>{prefix}};
  auto final_rng =  vec | ranges::views::join;
  for (const auto& p : final_rng) {
    for (const auto& s : final_rng) {
        res = p + s;
      }
  }
}

BENCHMARK(span_with_optionals_and_concat_view_array) {
  std::array<optional_string_t, 1> arr{{optional_string_t{prefix}}};
  std::span<std::string> sp{vecPrefixes};
  auto final_rng = ranges::views::concat(sp, arr);
  for (const auto& p : final_rng | filter_valid) {
    for (const auto& s : final_rng | filter_valid) {
        res = *p + *s;
      }
  }
}


int main() {
  folly::runBenchmarks();
}
