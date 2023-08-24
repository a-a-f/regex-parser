#pragma once

#include <type_traits>

namespace regex_engine::base {

//
// Replacement for dynamic_cast<> in the case we don't use multiple inheritance
// and we already know, by other means, that the downcast will succeed.
//
template <typename Derived, typename Base>
[[nodiscard]] auto refineCast(Base&& base) noexcept -> Derived {
  static_assert(std::is_base_of_v<std::remove_pointer_t<std::decay_t<Base>>,
                                  std::remove_pointer_t<std::decay_t<Derived>>>);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
  return static_cast<Derived>(std::forward<Base>(base));
}

} // namespace regex_engine::base
