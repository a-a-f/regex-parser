#pragma once

#include "parser/constants.h"
#include "parser/status_code.h"

#include <cctype>
#include <concepts>
#include <expected>
#include <limits>
#include <optional>
#include <string_view>
#include <type_traits>

namespace regex_engine::parser {

// Class which provides means to scan a given input.
// Recognizes tokens (characters and numbers) and various classes of
// characters.
class Scanner {
public:
  using Iterator = std::string_view::const_iterator;

public:
  Scanner(std::string_view input) noexcept
      : begin(input.cbegin()), current(input.cbegin()), end(input.cend()) {}
  Scanner(const Scanner&) = delete;
  auto operator=(const Scanner&) -> Scanner& = delete;

  [[nodiscard]] inline auto hasPending() const noexcept -> bool { return current < end; }
  [[nodiscard]] inline auto getCurrent() const noexcept -> char { return *current; }
  [[nodiscard]] inline auto getCurrentPosition() const noexcept -> Iterator { return current; }
  [[nodiscard]] inline auto getCurrentOffset() const noexcept -> std::size_t {
    return current - begin;
  }

  auto advance() noexcept -> std::expected<void, StatusCode>;
  auto consume(char ch) noexcept -> std::expected<void, StatusCode>;
  [[nodiscard]] auto consumeNonPrintableChar() noexcept -> std::expected<unsigned char, StatusCode>;

  template <Base base, std::integral T>
  [[nodiscard]] auto consumeNumber() noexcept -> std::expected<T, StatusCode> {
    // TODO Replace this statement with [[assume()]] once it is supported.
    __builtin_assume(static_cast<int8_t>(base) > 0);

    auto result = tryConsumeDigit<base>();
    if (!result) {
      return std::unexpected{StatusCode::NO_NUMBER};
    }
    auto val = static_cast<T>(*result);

    while (true) {
      auto result = tryConsumeDigit<base>();
      if (!result) {
        return val;
      }
      const auto digit = *result;

      if (val > (std::numeric_limits<T>::max() / static_cast<int8_t>(base))) {
        return std::unexpected{StatusCode::OVERFLOW};
      }

      val *= static_cast<int8_t>(base);

      if (val > std::numeric_limits<T>::max() - digit) {
        return std::unexpected{StatusCode::OVERFLOW};
      }

      val += digit;
    }
  }

  [[nodiscard]] static auto isMetachar(char ch) noexcept -> bool;
  [[nodiscard]] static auto isOrdinaryChar(char ch) noexcept -> bool;
  [[nodiscard]] static auto isOpPrefix(char ch) noexcept -> bool;
  [[nodiscard]] static auto isShorthandCharClass(char ch) noexcept -> bool;
  [[nodiscard]] static auto isAtomOpPrefix(char ch) noexcept -> bool;
  [[nodiscard]] static auto isNonPrintableChar(char ch) noexcept -> bool;

private:
  template <Base base>
  [[nodiscard]] auto tryConsumeDigit() noexcept -> std::expected<int8_t, StatusCode> {
    const auto ch = getCurrent();

    if constexpr (base == Base::BASE_16) {
      static constexpr auto OFFSET = 10;
      if (ch >= 'A' && ch <= 'F') {
        if (auto result = advance(); !result) {
          return std::unexpected{result.error()};
        } else {
          return static_cast<int8_t>(OFFSET + ch - 'A');
        }
      } else if (ch >= 'a' && ch <= 'f') {
        if (auto result = advance(); !result) {
          return std::unexpected{result.error()};
        } else {
          return static_cast<int8_t>(OFFSET + ch - 'a');
        }
      }
    }

    const auto digit = static_cast<int8_t>(ch - '0');
    if (digit < 0 || digit >= static_cast<int8_t>(base)) {
      return std::unexpected{StatusCode::SYNTAX_ERROR};
    }
    if (auto result = advance(); !result) {
      return std::unexpected{result.error()};
    }
    return digit;
  }

  Iterator begin;
  Iterator current;
  Iterator end;
};

} // namespace regex_engine::parser
