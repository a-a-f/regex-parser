#pragma once

#include "parser/base_ast_node.h"
#include "parser/status_code.h"

#include <string_view>

namespace regex_engine::parser {

// Return type of the parseRegex() function.
struct ParserResult {
  // AST of the parsed regex.
  ASTNodePtr node;
  // Parser diagnostic.
  StatusCode code;
  // Last scanned position in the input string.
  std::size_t lastPos;
};

// Parse the given input as a POSIX Extended Regular Expression.
auto parseRegex(std::string_view input) -> ParserResult;

} // namespace regex_engine::parser
