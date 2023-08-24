#pragma once

#include "parser/parser_base.h"

namespace regex_engine::parser {

class ASTNodeRepetition;

class RepetitionParser : public ParserBase {
public:
  RepetitionParser(Scanner& scanner) noexcept : ParserBase(scanner) {}
  [[nodiscard]] auto parse() -> std::expected<ASTNodePtr, StatusCode> final;

private:
  auto parseRepetitionRange(ASTNodeRepetition& node) noexcept -> std::expected<void, StatusCode>;
};

} // namespace regex_engine::parser
