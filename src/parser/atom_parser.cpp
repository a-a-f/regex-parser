#include "parser/atom_parser.h"
#include "parser/ast_node_anchor.h"
#include "parser/ast_node_any.h"
#include "parser/ast_node_char_sequence.h"
#include "parser/ast_node_group.h"
#include "parser/char_class_parser.h"
#include "parser/group_parser.h"
#include "parser/non_printable_char_parser.h"
#include "parser/shorthand_char_class_parser.h"

namespace regex_engine::parser {

auto AtomParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  if (!scanner.hasPending()) {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }

  auto ch = scanner.getCurrent();
  if (scanner.isOrdinaryChar(ch)) {
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    auto node = std::make_unique<ASTNodeCharSequence>();
    node->chars.push_back(ch);
    return node;
  } else if (ch == '.') {
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    return std::make_unique<ASTNodeAny>();
  } else if (ch == '\\') {
    return parseEscaped();
  } else if (ch == '(') {
    return GroupParser{scanner}.parse();
  } else if (ch == '[') {
    return CharClassParser{scanner}.parse();
  } else if (ch == '^') {
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    return std::make_unique<ASTNodeAnchor>(ASTNodeAnchor::Type::LINE_START);
  } else if (ch == '$') {
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    return std::make_unique<ASTNodeAnchor>(ASTNodeAnchor::Type::LINE_END);
  } else {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }
}

auto AtomParser::parseEscaped() -> std::expected<ASTNodePtr, StatusCode> {
  if (auto result = scanner.advance(); !result) {
    return std::unexpected{result.error()};
  }
  if (!scanner.hasPending()) {
    return std::unexpected{StatusCode::SYNTAX_ERROR};
  }

  const auto ch = scanner.getCurrent();
  if (scanner.isMetachar(ch)) {
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    auto node = std::make_unique<ASTNodeCharSequence>();
    node->chars.push_back(ch);
    return node;
  } else if (scanner.isShorthandCharClass(ch)) {
    return ShorthandCharClassParser{scanner}.parse();
  } else if (scanner.isNonPrintableChar(ch)) {
    return NonPrintableCharParser{scanner}.parse();
  } else if (ch == 'A') {
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    return std::make_unique<ASTNodeAnchor>(ASTNodeAnchor::Type::STRING_START);
  } else if (ch == 'Z') {
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    return std::make_unique<ASTNodeAnchor>(ASTNodeAnchor::Type::STRING_END);
  } else {
    return std::unexpected{StatusCode::INVALID_ESCAPED_CHAR};
  }
}

} // namespace regex_engine::parser
