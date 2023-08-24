#pragma once

#include "parser/parser_base.h"

namespace regex_engine::parser {

class GroupParser : public ParserBase {
public:
  GroupParser(Scanner& scanner) noexcept : ParserBase(scanner) {}
  [[nodiscard]] auto parse() -> std::expected<ASTNodePtr, StatusCode> final;
};

} // namespace regex_engine::parser
