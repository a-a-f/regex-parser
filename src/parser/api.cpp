#include "parser/api.h"
#include "parser/regex_parser.h"

namespace regex_engine::parser {

auto parseRegex(std::string_view input) -> ParserResult {
  auto scanner = Scanner{input};
  auto regexParser = RegexParser{scanner};
  auto result = ParserResult{};

  if (auto out = regexParser.parse(); out) {
    result.lastPos = scanner.getCurrentOffset();
    if (result.lastPos == input.length()) {
      result.code = StatusCode::ACCEPTED;
      result.node = std::move(*out);
    } else {
      // We didn't consume all the input.
      // Something is wrong with the parser itself.
      result.code = StatusCode::PARSER_ERROR;
    }
  } else {
    result.lastPos = scanner.getCurrentOffset();
    result.code = out.error();
  }

  return result;
}

} // namespace regex_engine::parser
