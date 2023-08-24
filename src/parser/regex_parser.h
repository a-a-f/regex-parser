#pragma once

#include "parser/parser_base.h"

namespace regex_engine::parser {

class RegexParser : public ParserBase {
public:
  RegexParser(Scanner& scanner, bool isTopLevel = true) noexcept
      : ParserBase(scanner), isTopLevel(isTopLevel) {}
  [[nodiscard]] auto parse() -> std::expected<ASTNodePtr, StatusCode> final;

private:
  bool isTopLevel;
};

} // namespace regex_engine::parser
