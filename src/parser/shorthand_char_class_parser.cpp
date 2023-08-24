#include "parser/shorthand_char_class_parser.h"
#include "parser/ast_node_char_class.h"
#include "parser/char_class_common.h"

#include <cctype>

namespace regex_engine::parser {

auto ShorthandCharClassParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  const auto ch = scanner.getCurrent();

  if (auto result = scanner.advance(); !result) {
    return std::unexpected{result.error()};
  }

  auto ranges = ASTNodeCharClass::Ranges{};
  if (!shorthandCharClassToRanges(ch, ranges)) {
    return std::unexpected{StatusCode::INVALID_SHORTHAND_CHAR_CLASS};
  }

  auto node = std::make_unique<ASTNodeCharClass>();
  if (std::islower(ch)) {
    node->matchingRanges = std::move(ranges);
  } else {
    node->nonMatchingRanges = std::move(ranges);
  }
  return node;
}

} // namespace regex_engine::parser
