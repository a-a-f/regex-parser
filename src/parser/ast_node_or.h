#pragma once

#include "parser/base_ast_node.h"

namespace regex_engine::parser {

class ASTNodeOr : public BaseASTNode {
public:
  ASTNodeOr() = default;
  [[nodiscard]] auto getType() const noexcept -> ASTNodeType final { return ASTNodeType::OR; }
};

} // namespace regex_engine::parser
