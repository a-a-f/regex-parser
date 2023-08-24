#pragma once

#include "parser/i_parser.h"
#include "parser/scanner.h"

namespace regex_engine::parser {

// Class from which all the various parsers must derive.
class ParserBase : public IParser {
public:
  ParserBase(Scanner& scanner) noexcept : scanner(scanner) {}

protected:
  Scanner& scanner;
};

} // namespace regex_engine::parser
