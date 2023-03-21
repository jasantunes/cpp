#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <magic_enum.hpp>


enum class Source { UNKNOWN = 0, BIGCACHE, ZIPPYDB };
template <typename Enum>
std::string getLowerCaseEnumName(Enum type) {
  auto lowerCaseEnumName =
      static_cast<std::string>(magic_enum::enum_name(type));
  std::transform(
      lowerCaseEnumName.begin(),
      lowerCaseEnumName.end(),
      lowerCaseEnumName.begin(),
      ::tolower);
  return lowerCaseEnumName;
}

template <typename Enum>
std::string getFormattedStatsName(Enum type) {
  return getLowerCaseEnumName(type) + "_";
}

int main() {
  {
    int var = -10;
    std::cout << fmt::format("var: {}", var) << std::endl;
  }

  {
    std::string name = std::string(magic_enum::enum_name(Source::BIGCACHE));
    std::cout << name << std::endl;
  }

  {
    std::string name = getFormattedStatsName(Source::BIGCACHE);
    std::cout << name << std::endl;
  }
}