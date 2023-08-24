#pragma once

#include "parser/base_ast_node.h"

namespace regex_engine::parser {

class ASTNodeAnchor : public BaseASTNode {
public:
  enum class Type { LINE_START, LINE_END, STRING_START, STRING_END };

  inline ASTNodeAnchor(Type type) noexcept : type(type) {}
  [[nodiscard]] auto getType() const noexcept -> ASTNodeType final { return ASTNodeType::ANCHOR; }
  [[nodiscard]] auto isQuantifiable() const noexcept -> bool final { return false; }

  Type type;
};

} // namespace regex_engine::parser
