#include "parser/scanner.h"

#include <cctype>
#include <unordered_set>

namespace regex_engine::parser {

namespace {
const auto METACHARS =
    std::unordered_set<char>{'\\', '^', '$', '.', '|', '?', '*', '+', '(', ')', '[', '{'};

const auto CLASS_LIST = std::unordered_set<char>{'D', 'd', 'W', 'w', 'S', 's'};
} // namespace

auto Scanner::advance() noexcept -> std::expected<void, StatusCode> {
  if (hasPending()) {
    ++current;
    return {};
  } else {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }
}

auto Scanner::consume(char ch) noexcept -> std::expected<void, StatusCode> {
  if (hasPending() && getCurrent() == ch) {
    ++current;
    return {};
  } else {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }
}

auto Scanner::isOpPrefix(char ch) noexcept -> bool {
  return ch == '?' || ch == '+' || ch == '*' || ch == '{';
}

auto Scanner::isMetachar(char ch) noexcept -> bool { return METACHARS.contains(ch); }

auto Scanner::isShorthandCharClass(char ch) noexcept -> bool { return CLASS_LIST.contains(ch); }

auto Scanner::isOrdinaryChar(char ch) noexcept -> bool { return isprint(ch) && !isMetachar(ch); }

auto Scanner::isAtomOpPrefix(char ch) noexcept -> bool {
  return isOrdinaryChar(ch) || ch == '.' || ch == '\\' || ch == '(' || ch == '[' || ch == '^' ||
         ch == '$';
}

auto Scanner::isNonPrintableChar(char ch) noexcept -> bool {
  return ch == 't' || ch == 'r' || ch == 'n' || ch == 'a' || ch == 'e' || ch == 'f' || ch == 'v' ||
         ch == 'c' || ch == 'x' || ch == 'o';
}

[[nodiscard]] auto Scanner::consumeNonPrintableChar() noexcept
    -> std::expected<unsigned char, StatusCode> {
  static constexpr auto NUL = 0x00;
  static constexpr auto TAB = 0x09;
  static constexpr auto CR = 0x0D;
  static constexpr auto LF = 0x0A;
  static constexpr auto BEL = 0x07;
  static constexpr auto ESC = 0x1B;
  static constexpr auto FF = 0x0C;
  static constexpr auto VT = 0x0B;

  auto ch = getCurrent();
  if (auto result = advance(); !result) {
    return std::unexpected{result.error()};
  }

  switch (ch) {
  case 't': {
    return TAB;
  }
  case 'r': {
    return CR;
  }
  case 'n': {
    return LF;
  }
  case 'a': {
    return BEL;
  }
  case 'e': {
    return ESC;
  }
  case 'f': {
    return FF;
  }
  case 'v': {
    return VT;
  }
  case 'c': {
    // \cX means ctrl + X (can be from A to Z or a to z)
    if (!hasPending()) {
      return std::unexpected{StatusCode::SYNTAX_ERROR};
    }

    auto ch = getCurrent();

    if (auto result = advance(); !result) {
      return std::unexpected{result.error()};
    }

    if (ch >= 'A' && ch <= 'Z') {
      return static_cast<unsigned char>(0x01 + ch - 'A');
    } else if (ch >= 'a' && ch <= 'z') {
      return static_cast<unsigned char>(0x01 + ch - 'a');
    } else {
      return std::unexpected{StatusCode::SYNTAX_ERROR};
    }
  }
  case 'x': {
    auto result = consumeNumber<Base::BASE_16, unsigned char>();
    if (!result) {
      if (result.error() == StatusCode::NO_NUMBER) {
        // \x alone matches NUL
        return NUL;
      } else {
        return std::unexpected{result.error()};
      }
    }
    return *result;
  }
  case 'o': {
    if (!consume('{')) {
      return std::unexpected{StatusCode::SYNTAX_ERROR};
    }
    auto result = consumeNumber<Base::BASE_8, unsigned char>();
    if (!result) {
      return std::unexpected{result.error()};
    }
    if (!consume('}')) {
      return std::unexpected{StatusCode::BRACE_IMBALANCE};
    }
    return *result;
  }
  default: {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }
  }
}

} // namespace regex_engine::parser
