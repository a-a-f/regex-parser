#include "parser/atom_op_parser.h"
#include "parser/ast_node_repetition.h"
#include "parser/atom_parser.h"
#include "parser/repetition_parser.h"

namespace regex_engine::parser {

auto AtomOpParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  auto result = AtomParser{scanner}.parse();
  if (!result) {
    return std::unexpected{result.error()};
  }
  if (!*result) [[unlikely]] {
    return std::unexpected{StatusCode::PARSER_ERROR};
  }
  auto atom = std::move(*result);

  if (scanner.hasPending() && scanner.isOpPrefix(scanner.getCurrent())) {
    if (!atom->isQuantifiable()) {
      return std::unexpected{StatusCode::UNQUANTIFIABLE_EXPRESSION};
    }

    result = RepetitionParser{scanner}.parse();
    if (!result) {
      return std::unexpected{result.error()};
    }
    if (!*result) [[unlikely]] {
      return std::unexpected{StatusCode::PARSER_ERROR};
    }
    auto repetition = std::move(*result);

    if (scanner.hasPending() && scanner.isOpPrefix(scanner.getCurrent())) {
      // A repetition cannot be applied more than once.
      return std::unexpected{StatusCode::UNQUANTIFIABLE_EXPRESSION};
    }

    repetition->children.push_back(std::move(atom));
    atom = std::move(repetition);
  }

  return atom;
}

} // namespace regex_engine::parser
