#pragma once

#include "parser/base_ast_node.h"

namespace regex_engine::parser {

class ASTNodeAny : public BaseASTNode {
public:
  ASTNodeAny() = default;
  [[nodiscard]] auto getType() const noexcept -> ASTNodeType final { return ASTNodeType::ANY; }
};

} // namespace regex_engine::parser
