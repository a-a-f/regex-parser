#pragma once

#include "parser/parser_base.h"

namespace regex_engine::parser {

class ShorthandCharClassParser : public ParserBase {
public:
  ShorthandCharClassParser(Scanner& scanner) noexcept : ParserBase(scanner) {}
  [[nodiscard]] auto parse() -> std::expected<ASTNodePtr, StatusCode> final;
};

} // namespace regex_engine::parser
