#pragma once

namespace regex_engine::parser {

enum class StatusCode {
  ACCEPTED,
  INVALID_BOUNDS,
  INVALID_POSIX_CLASS,
  INVALID_ESCAPED_CHAR,
  INVALID_SHORTHAND_CHAR_CLASS,
  BRACKET_IMBALANCE,
  SYNTAX_ERROR,
  UNQUANTIFIABLE_EXPRESSION,
  INVALID_REPETITION,
  PARSER_ERROR,
  PARENTHESIS_IMBALANCE,
  BRACE_IMBALANCE,
  NO_NUMBER,
  OVERFLOW
};

} // namespace regex_engine::parser
