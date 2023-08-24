#pragma once

#include "base/utils.h"
#include "parser/base_ast_node.h"

namespace regex_engine::parser {

class ASTNodeCharClass : public BaseASTNode {
public:
  ASTNodeCharClass() = default;
  [[nodiscard]] auto getType() const noexcept -> ASTNodeType final {
    return ASTNodeType::CHAR_CLASS;
  }

  using Ranges = base::Ranges<unsigned char>;
  Ranges matchingRanges;
  Ranges nonMatchingRanges;
};

} // namespace regex_engine::parser
