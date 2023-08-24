#pragma once

#include "parser/base_ast_node.h"

#include <cstdint>
#include <optional>

namespace regex_engine::parser {

class ASTNodeRepetition : public BaseASTNode {
public:
  ASTNodeRepetition() = default;
  [[nodiscard]] auto getType() const noexcept -> ASTNodeType final {
    return ASTNodeType::REPETITION;
  }
  [[nodiscard]] auto isQuantifiable() const noexcept -> bool final { return false; }

  uint16_t min = 0;
  std::optional<uint16_t> max;
};

} // namespace regex_engine::parser
