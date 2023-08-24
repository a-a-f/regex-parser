#pragma once

#include "parser/base_ast_node.h"

#include <vector>

namespace regex_engine::parser {

class ASTNodeCharSequence : public BaseASTNode {
public:
  ASTNodeCharSequence() = default;
  [[nodiscard]] auto getType() const noexcept -> ASTNodeType final {
    return ASTNodeType::CHAR_SEQUENCE;
  }
  std::vector<unsigned char> chars;
};

} // namespace regex_engine::parser
