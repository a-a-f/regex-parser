#include "parser/group_parser.h"
#include "parser/ast_node_group.h"
#include "parser/regex_parser.h"

namespace regex_engine::parser {

auto GroupParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  if (auto result = scanner.consume('('); !result) {
    return std::unexpected{result.error()};
  }

  if (!scanner.hasPending()) {
    // We must have a closing parenthesis at the very end.
    return std::unexpected{StatusCode::PARENTHESIS_IMBALANCE};
  }

  auto isCapturing = true;
  if (scanner.getCurrent() == '?') {
    // Parse non-capturing group.
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    if (scanner.hasPending() && scanner.getCurrent() == ':') {
      isCapturing = false;
      if (auto result = scanner.advance(); !result) {
        return std::unexpected{result.error()};
      }
    } else {
      return std::unexpected{StatusCode::SYNTAX_ERROR};
    }
  }

  auto result = RegexParser{scanner, false}.parse();
  if (!result) {
    return std::unexpected{result.error()};
  }
  if (!*result) [[unlikely]] {
    return std::unexpected{StatusCode::PARSER_ERROR};
  }
  auto regex = std::move(*result);

  if (auto result = scanner.consume(')'); !result) {
    return std::unexpected{StatusCode::PARENTHESIS_IMBALANCE};
  }

  auto node = std::make_unique<ASTNodeGroup>();
  node->isCapturing = isCapturing;
  node->children.push_back(std::move(regex));
  return node;
}

} // namespace regex_engine::parser
