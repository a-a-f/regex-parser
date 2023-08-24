#pragma once

#include "parser/base_ast_node.h"
#include "parser/status_code.h"
#include <expected>

namespace regex_engine::parser {

class IParser {
public:
  virtual ~IParser() = default;
  [[nodiscard]] virtual auto parse() -> std::expected<ASTNodePtr, StatusCode> = 0;
};

} // namespace regex_engine::parser
