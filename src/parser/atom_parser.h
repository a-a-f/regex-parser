#pragma once

#include "parser/parser_base.h"

namespace regex_engine::parser {

class AtomParser : public ParserBase {
public:
  AtomParser(Scanner& scanner) noexcept : ParserBase(scanner) {}
  [[nodiscard]] auto parse() -> std::expected<ASTNodePtr, StatusCode> final;

private:
  [[nodiscard]] auto parseEscaped() -> std::expected<ASTNodePtr, StatusCode>;
};

} // namespace regex_engine::parser
