#include "parser/repetition_parser.h"
#include "parser/ast_node_repetition.h"
#include "parser/constants.h"

namespace regex_engine::parser {

auto RepetitionParser::parseRepetitionRange(ASTNodeRepetition& node) noexcept
    -> std::expected<void, StatusCode> {
  if (auto result = scanner.consume('{'); !result) {
    return std::unexpected{result.error()};
  }

  auto result = scanner.consumeNumber<Base::BASE_10, uint16_t>();
  if (!result) {
    return std::unexpected{result.error()};
  }
  node.min = *result;

  if (scanner.getCurrent() == ',') {
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
    if (scanner.hasPending() && std::isdigit(scanner.getCurrent())) {
      if (auto result = scanner.consumeNumber<Base::BASE_10, uint16_t>(); result) {
        node.max = *result;
      } else {
        return std::unexpected{result.error()};
      }
    }
  } else {
    node.max = node.min;
  }
  if (node.max && node.min > *node.max) {
    return std::unexpected{StatusCode::INVALID_REPETITION};
  }
  if (auto result = scanner.consume('}'); !result) {
    return std::unexpected{StatusCode::BRACE_IMBALANCE};
  }
  return {};
}

auto RepetitionParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  auto node = std::make_unique<ASTNodeRepetition>();

  const auto ch = scanner.getCurrent();
  if (ch == '?') {
    node->min = 0;
    node->max = 1;
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
  } else if (ch == '+') {
    node->min = 1;
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
  } else if (ch == '*') {
    node->min = 0;
    if (auto result = scanner.advance(); !result) {
      return std::unexpected{result.error()};
    }
  } else if (ch == '{') {
    auto result = parseRepetitionRange(*node);
    if (!result) {
      return std::unexpected{result.error()};
    }
  } else {
    return std::unexpected{StatusCode::PARSER_ERROR};
  }

  return node;
}

} // namespace regex_engine::parser
