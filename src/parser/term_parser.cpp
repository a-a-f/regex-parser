#include "parser/term_parser.h"
#include "base/refine_cast.h"
#include "parser/ast_node_add.h"
#include "parser/ast_node_char_sequence.h"
#include "parser/atom_op_parser.h"

namespace regex_engine::parser {

using namespace regex_engine::base;

auto TermParser::parse() -> std::expected<ASTNodePtr, StatusCode> {
  auto atomOpParser = AtomOpParser{scanner};

  auto result = atomOpParser.parse();
  if (!result) {
    return std::unexpected{result.error()};
  }
  if (!*result) [[unlikely]] {
    return std::unexpected{StatusCode::PARSER_ERROR};
  }
  auto atomOp = std::move(*result);

  if (scanner.hasPending() && scanner.isAtomOpPrefix(scanner.getCurrent())) {
    auto node = std::make_unique<ASTNodeAdd>();

    node->children.push_back(std::move(atomOp));
    do {
      result = atomOpParser.parse();
      if (!result) {
        return std::unexpected{result.error()};
      }
      if (!*result) [[unlikely]] {
        return std::unexpected{StatusCode::PARSER_ERROR};
      }
      auto rightAtomOp = std::move(*result);

      if (rightAtomOp->getType() == node->children.back()->getType() &&
          rightAtomOp->getType() == ASTNodeType::CHAR_SEQUENCE) {
        // Merge adjacent char sequences.
        const auto& from = refineCast<const ASTNodeCharSequence&>(*rightAtomOp).chars;
        auto& to = refineCast<ASTNodeCharSequence&>(*node->children.back()).chars;
        to.insert(to.end(), from.begin(), from.end());
      } else {
        node->children.push_back(std::move(rightAtomOp));
      }
    } while (scanner.hasPending() && scanner.isAtomOpPrefix(scanner.getCurrent()));

    if (node->children.size() == 1) {
      atomOp = std::move(node->children[0]);
    } else {
      atomOp = std::move(node);
    }
  }

  return atomOp;
}

} // namespace regex_engine::parser
