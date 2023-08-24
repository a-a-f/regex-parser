#pragma once

#include "parser/base_ast_node.h"

namespace regex_engine::parser {

class ASTNodeAdd : public BaseASTNode {
public:
  ASTNodeAdd() = default;
  [[nodiscard]] auto getType() const noexcept -> ASTNodeType final { return ASTNodeType::ADD; }
};

} // namespace regex_engine::parser
