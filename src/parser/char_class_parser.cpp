#include "parser/char_class_parser.h"
#include "base/string_hash.h"
#include "base/utils.h"
#include "parser/ast_node_char_class.h"
#include "parser/char_class_common.h"

#include <cctype>
#include <initializer_list>
#include <string_view>
#include <unordered_map>

namespace regex_engine::parser {

auto CharClassParser::emitPrevious() -> void {
  if (prevCategory == Category::PRINTABLE_CHAR) {
    matchingRanges.emplace_back(*prevPos, *prevPos);
  }
}

auto CharClassParser::emitPrintableChars(Scanner::Iterator begin, Scanner::Iterator end)
    -> std::expected<void, StatusCode> {
  if (begin == end) {
    return {};
  }

  if (prevCategory == Category::LOWER_BOUND) {
    // Emit the first character as an upper bound.
    auto upperBound = *begin;
    if (lowerBound > upperBound) {
      return std::unexpected{StatusCode::INVALID_BOUNDS};
    }
    matchingRanges.emplace_back(lowerBound, upperBound);
    ++begin;
  }

  if (begin < end) {
    for (const auto ch : std::string_view(begin, end)) {
      matchingRanges.emplace_back(ch, ch);
    }
  }

  if (prevCategory == Category::LOWER_BOUND && begin == end) {
    // The last emitted character was an upper bound.
    prevCategory = Category::UPPER_BOUND;
  } else {
    // The last emitted character was a printable character.
    prevCategory = Category::PRINTABLE_CHAR;
  }

  return {};
}

auto CharClassParser::emitPosixClass(std::string_view className, bool isNegated)
    -> std::expected<void, StatusCode> {
  using PosixClassBuilder = std::function<void(ASTNodeCharClass::Ranges&)>;

  using PosixClassBuilderByName =
      std::unordered_map<std::string, PosixClassBuilder, base::StringHash, std::equal_to<>>;

  static const auto POSIX_CLASS_BUILDER_BY_NAME = PosixClassBuilderByName{
      {"alnum", appendAlnum}, {"alpha", appendAlpha},  {"ascii", appendASCII},
      {"blank", appendBlank}, {"cntrl", appendCntrl},  {"digit", appendDigit},
      {"graph", appendGraph}, {"lower", appendLower},  {"print", appendPrint},
      {"punct", appendPunct}, {"space", appendSpace},  {"upper", appendUpper},
      {"word", appendWord},   {"xdigit", appendXdigit}};

  if (const auto it = POSIX_CLASS_BUILDER_BY_NAME.find(className);
      it != POSIX_CLASS_BUILDER_BY_NAME.end()) {
    const auto& append = it->second;
    append(isNegated ? nonMatchingRanges : matchingRanges);
    prevCategory = Category::POSIX_CHAR_CLASS;
    return {};
  } else {
    return std::unexpected{StatusCode::INVALID_POSIX_CLASS};
  }
}

auto CharClassParser::emitResidual() -> void {
  if (prevCategory == Category::LOWER_BOUND) {
    // We are inside an unfinished range.
    // We treat it as a sequence of characters.
    matchingRanges.emplace_back(lowerBound, lowerBound);
    matchingRanges.emplace_back('-', '-');
  } else {
    emitPrevious();
  }
}

auto CharClassParser::processOpeningBracket() -> std::expected<void, StatusCode> {
  auto begin = curPos;

  emitPrevious();

  if (auto result = advance(); !result) {
    return std::unexpected{result.error()};
  }
  if (scanner.getCurrent() != ':') {
    return emitPrintableChars(begin, curPos);
  }
  if (auto result = advance(); !result) {
    return std::unexpected{result.error()};
  }

  const auto isNegated = scanner.getCurrent() == '^';
  if (isNegated) {
    if (auto result = processNegated(); !result) {
      return std::unexpected{result.error()};
    }
  }

  // Consume an alphabetic character sequence.
  const auto beginSeq = curPos;
  while (std::isalpha(scanner.getCurrent())) {
    if (auto result = advance(); !result) {
      return std::unexpected{result.error()};
    }
  }
  const auto endSeq = curPos;
  if (beginSeq == endSeq) {
    return emitPrintableChars(begin, curPos);
  }

  if (scanner.getCurrent() != ':') {
    return emitPrintableChars(begin, curPos);
  }
  if (auto result = advance(); !result) {
    return std::unexpected{result.error()};
  }
  if (scanner.getCurrent() != ']') {
    return emitPrintableChars(begin, curPos);
  }

  if (prevCategory == Category::LOWER_BOUND) {
    // The bounds of a range must be characters.
    return std::unexpected{StatusCode::INVALID_BOUNDS};
  }

  if (auto result = emitPosixClass(std::string_view(beginSeq, endSeq), isNegated); result) {
    return advance();
  } else {
    return std::unexpected{StatusCode::INVALID_POSIX_CLASS};
  }
}

auto CharClassParser::processPrintableChar() -> std::expected<void, StatusCode> {
  emitPrevious();

  if (prevCategory == Category::LOWER_BOUND) {
    if (auto result = emitRange(); !result) {
      return std::unexpected{result.error()};
    }
  } else {
    prevCategory = Category::PRINTABLE_CHAR;
  }

  return advance();
}

auto CharClassParser::processNonPrintableChar() -> std::expected<void, StatusCode> {
  emitPrevious();

  prevPos = curPos;

  auto result = scanner.consumeNonPrintableChar();
  if (!result) {
    return std::unexpected{result.error()};
  }
  const auto ch = *result;

  if (!scanner.hasPending()) {
    return std::unexpected{StatusCode::BRACKET_IMBALANCE};
  }

  curPos = scanner.getCurrentPosition();

  if (prevCategory == Category::LOWER_BOUND) {
    // Actually we're not a range.
    matchingRanges.emplace_back(lowerBound, lowerBound);
    matchingRanges.emplace_back('-', '-');
  }

  matchingRanges.emplace_back(ch, ch);
  prevCategory = Category::NON_PRINTABLE_CHAR;

  return {};
}

auto CharClassParser::processEscapedChar() -> std::expected<void, StatusCode> {
  emitPrevious();

  // Skip backslash.
  if (auto result = advance(); !result) {
    return std::unexpected{result.error()};
  }

  const auto ch = scanner.getCurrent();
  if (scanner.isShorthandCharClass(ch)) {
    return processShorthandCharClass(ch);
  } else if (scanner.isMetachar(ch) || ch == ']') {
    return processPrintableChar();
  } else if (scanner.isNonPrintableChar(ch)) {
    return processNonPrintableChar();
  } else {
    return std::unexpected{StatusCode::INVALID_ESCAPED_CHAR};
  }
}

auto CharClassParser::processHyphen() -> std::expected<void, StatusCode> {
  if (prevCategory == Category::LOWER_BOUND) {
    if (auto result = emitRange(); !result) {
      return std::unexpected{result.error()};
    }
  } else if (prevCategory == Category::PRINTABLE_CHAR) {
    // We've started a new range.
    // Store the lower bound.
    lowerBound = *prevPos;
    prevCategory = Category::LOWER_BOUND;
  } else if (prevCategory == Category::INVALID || prevCategory == Category::UPPER_BOUND ||
             prevCategory == Category::NON_PRINTABLE_CHAR) {
    // The hyphen is an ordinary character.
    prevCategory = Category::PRINTABLE_CHAR;
  } else {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }

  return advance();
}

auto CharClassParser::processShorthandCharClass(char ch) -> std::expected<void, StatusCode> {
  if (prevCategory == Category::LOWER_BOUND) {
    // The bounds of a range must be characters.
    return std::unexpected{StatusCode::INVALID_BOUNDS};
  }
  if (auto result = advance(); !result) {
    return std::unexpected{result.error()};
  }
  if (!shorthandCharClassToRanges(ch, islower(ch) ? matchingRanges : nonMatchingRanges)) {
    return std::unexpected{StatusCode::INVALID_SHORTHAND_CHAR_CLASS};
  }
  prevCategory = Category::SHORTHAND_CHAR_CLASS;
  return {};
}

auto CharClassParser::processNegated() noexcept -> std::expected<void, StatusCode> {
  prevCategory = Category::NEGATION;
  return advance();
}

auto CharClassParser::emitRange() -> std::expected<void, StatusCode> {
  const auto upperBound = scanner.getCurrent();
  if (lowerBound > upperBound) {
    return std::unexpected{StatusCode::INVALID_BOUNDS};
  }
  matchingRanges.emplace_back(lowerBound, upperBound);
  prevCategory = Category::UPPER_BOUND;
  return {};
}

auto CharClassParser::advance() noexcept -> std::expected<void, StatusCode> {
  prevPos = curPos;
  if (auto result = scanner.advance(); !result) {
    return std::unexpected{result.error()};
  }
  if (!scanner.hasPending()) {
    // We must have a closing bracket at the very end.
    return std::unexpected{StatusCode::BRACKET_IMBALANCE};
  }
  curPos = scanner.getCurrentPosition();
  return {};
}

auto CharClassParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  // Opening character class.
  if (auto result = scanner.consume('['); !result) {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }

  curPos = scanner.getCurrentPosition();

  if (!scanner.hasPending()) {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }

  const auto isNegated = scanner.getCurrent() == '^';
  if (isNegated) {
    if (auto result = processNegated(); !result) {
      return std::unexpected{result.error()};
    }
  }

  // If the first encountered character is ']', it does not close the
  // character class.
  if (scanner.getCurrent() == ']') {
    if (auto result = processPrintableChar(); !result) {
      return std::unexpected{result.error()};
    }
  }

  while (scanner.getCurrent() != ']') {
    switch (scanner.getCurrent()) {
    case '[': {
      if (auto result = processOpeningBracket(); !result) {
        return std::unexpected{result.error()};
      }
      break;
    }
    case '-': {
      if (auto result = processHyphen(); !result) {
        return std::unexpected{result.error()};
      }
      break;
    }
    case '\\': {
      if (auto result = processEscapedChar(); !result) {
        return std::unexpected{result.error()};
      }
      break;
    }
    default: {
      if (auto result = processPrintableChar(); !result) {
        return std::unexpected{result.error()};
      }
      break;
    }
    }
  }

  emitResidual();

  if (matchingRanges.empty() && nonMatchingRanges.empty()) {
    return std::unexpected{StatusCode::PARSER_ERROR};
  }

  // Closing character class.
  if (auto result = scanner.consume(']'); !result) {
    return std::unexpected{StatusCode::BRACKET_IMBALANCE};
  }

  base::mergeRanges(matchingRanges);
  base::mergeRanges(nonMatchingRanges);
  if (isNegated) {
    std::swap(matchingRanges, nonMatchingRanges);
  }

  auto node = std::make_unique<ASTNodeCharClass>();
  node->matchingRanges = std::move(matchingRanges);
  node->nonMatchingRanges = std::move(nonMatchingRanges);

  return node;
}

} // namespace regex_engine::parser
