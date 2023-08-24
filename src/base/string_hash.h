#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace regex_engine::base {

struct StringHash {
  using is_transparent = void;

  [[nodiscard]] inline auto operator()(std::string_view str) const noexcept -> std::size_t {
    return std::hash<std::string_view>()(str);
  }

  [[nodiscard]] inline auto operator()(const std::string& str) const noexcept -> std::size_t {
    return std::hash<std::string>()(str);
  }
};

} // namespace regex_engine::base
