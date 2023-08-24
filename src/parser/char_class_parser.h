#pragma once

#include "parser/ast_node_char_class.h"
#include "parser/parser_base.h"

namespace regex_engine::parser {

class CharClassParser : public ParserBase {
public:
  CharClassParser(Scanner& scanner) noexcept : ParserBase(scanner) {}
  ~CharClassParser() = default;

  CharClassParser(const CharClassParser&) = delete;
  auto operator=(const CharClassParser&) -> CharClassParser& = delete;
  CharClassParser(CharClassParser&&) = delete;
  auto operator=(CharClassParser&&) -> CharClassParser& = delete;

  [[nodiscard]] auto parse() -> std::expected<ASTNodePtr, StatusCode> final;

private:
  auto processOpeningBracket() -> std::expected<void, StatusCode>;
  auto processPrintableChar() -> std::expected<void, StatusCode>;
  auto processNonPrintableChar() -> std::expected<void, StatusCode>;
  auto processShorthandCharClass(char ch) -> std::expected<void, StatusCode>;
  auto processNegated() noexcept -> std::expected<void, StatusCode>;
  auto processEscapedChar() -> std::expected<void, StatusCode>;
  auto processHyphen() -> std::expected<void, StatusCode>;

  // If the previously scanned character is a printable character,
  // emit it as a matching range.
  auto emitPrevious() -> void;
  // Given a range of characters known to be printable characters,
  // emit each of them as a matching range.
  auto emitPrintableChars(Scanner::Iterator begin, Scanner::Iterator end)
      -> std::expected<void, StatusCode>;
  // Translate the given POSIX class into a set of matching or
  // non-matching ranges. Returns false if the specified POSIX
  // class name is invalid.
  [[nodiscard]] auto emitPosixClass(std::string_view className, bool isNegated)
      -> std::expected<void, StatusCode>;
  // Called when we reached the end of the character class.
  // Emit remaining unprocessed characters as matching ranges.
  auto emitResidual() -> void;
  // Emit a parsed range as a matching range.
  // Returns false if the parsed range is invalid.
  [[nodiscard]] auto emitRange() -> std::expected<void, StatusCode>;

  auto advance() noexcept -> std::expected<void, StatusCode>;

  enum class Category {
    INVALID,
    NEGATION,
    PRINTABLE_CHAR,
    NON_PRINTABLE_CHAR,
    POSIX_CHAR_CLASS,
    SHORTHAND_CHAR_CLASS,
    LOWER_BOUND,
    UPPER_BOUND
  };

  // Current scanner position.
  Scanner::Iterator curPos{};
  // Previous scanner position.
  Scanner::Iterator prevPos{};
  // Category the previously scanned character belongs to.
  Category prevCategory = Category::INVALID;
  // Lower of bound of the range which is being parsed.
  char lowerBound{};
  // Set of ranges whose elements are matches.
  ASTNodeCharClass::Ranges matchingRanges{};
  // Set of ranges whose elements are not matches.
  ASTNodeCharClass::Ranges nonMatchingRanges{};
};

} // namespace regex_engine::parser
