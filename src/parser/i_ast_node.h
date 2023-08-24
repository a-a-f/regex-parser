#pragma once

namespace regex_engine::parser {

enum class ASTNodeType {
  ADD,
  ANCHOR,
  ANY,
  CHAR_CLASS,
  CHAR_SEQUENCE,
  GROUP,
  NORMAL,
  OR,
  REPETITION
};

class IASTNode {
public:
  virtual ~IASTNode() = default;
  [[nodiscard]] virtual auto getType() const noexcept -> ASTNodeType = 0;
  // Can we apply a repetition, i.e. '+', '?','*', or '{min, max}', to the current node?
  [[nodiscard]] virtual auto isQuantifiable() const noexcept -> bool = 0;
};

} // namespace regex_engine::parser
