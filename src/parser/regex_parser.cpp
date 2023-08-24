#include "parser/regex_parser.h"
#include "parser/ast_node_or.h"
#include "parser/term_parser.h"

namespace regex_engine::parser {

auto RegexParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  auto termParser = TermParser{scanner};

  auto result = termParser.parse();
  if (!result) {
    return std::unexpected{result.error()};
  }
  if (!*result) [[unlikely]] {
    return std::unexpected{StatusCode::PARSER_ERROR};
  }
  auto term = std::move(*result);

  if (scanner.hasPending()) {
    auto ch = scanner.getCurrent();
    if (ch == ')' && !isTopLevel) {
      return term;
    } else if (ch == '|') {
      auto node = std::make_unique<ASTNodeOr>();

      node->children.push_back(std::move(term));
      do {
        if (auto result = scanner.advance(); !result) {
          return std::unexpected{result.error()};
        }
        if (auto rightTerm = termParser.parse(); rightTerm) {
          if (!*rightTerm) [[unlikely]] {
            return std::unexpected{StatusCode::PARSER_ERROR};
          }
          node->children.push_back(std::move(*rightTerm));
        } else {
          return std::unexpected{rightTerm.error()};
        }
      } while (scanner.hasPending() && scanner.getCurrent() == '|');

      if (scanner.hasPending()) {
        if (scanner.getCurrent() == ')' && !isTopLevel) {
          ;
        } else {
          return std::unexpected{StatusCode::SYNTAX_ERROR};
        }
      }

      term = std::move(node);
    } else {
      return std::unexpected{StatusCode::SYNTAX_ERROR};
    }
  }

  return term;
}

} // namespace regex_engine::parser
