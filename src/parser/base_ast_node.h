#pragma once

#include "parser/i_ast_node.h"

#include <memory>
#include <vector>

namespace regex_engine::parser {

class BaseASTNode;
using ASTNodePtr = std::unique_ptr<BaseASTNode>;
using ASTNodeChildren = std::vector<ASTNodePtr>;

class BaseASTNode : public IASTNode {
public:
  virtual ~BaseASTNode() = default;
  [[nodiscard]] auto isQuantifiable() const noexcept -> bool override { return true; }

  ASTNodeChildren children;
};

} // namespace regex_engine::parser
