#pragma once

#include "parser/base_ast_node.h"

#include <cstdint>

namespace regex_engine::parser {

class ASTNodeGroup : public BaseASTNode {
public:
  ASTNodeGroup() = default;
  [[nodiscard]] auto getType() const noexcept -> ASTNodeType final { return ASTNodeType::GROUP; }
  bool isCapturing = false;
};

} // namespace regex_engine::parser
