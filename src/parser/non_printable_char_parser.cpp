#include "parser/non_printable_char_parser.h"
#include "parser/ast_node_char_sequence.h"

namespace regex_engine::parser {

auto NonPrintableCharParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  if (auto result = scanner.consumeNonPrintableChar(); !result) {
    return std::unexpected{result.error()};
  } else {
    auto node = std::make_unique<ASTNodeCharSequence>();
    node->chars.push_back(*result);
    return node;
  }
}

} // namespace regex_engine::parser
