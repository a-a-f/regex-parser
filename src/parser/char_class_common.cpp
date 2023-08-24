#include "parser/char_class_common.h"

#include <cctype>
#include <initializer_list>

namespace regex_engine::parser {

auto appendUpper(ASTNodeCharClass::Ranges& ranges) -> void { ranges.emplace_back('A', 'Z'); }

auto appendLower(ASTNodeCharClass::Ranges& ranges) -> void { ranges.emplace_back('a', 'z'); }

auto appendDigit(ASTNodeCharClass::Ranges& ranges) -> void { ranges.emplace_back('0', '9'); }

auto appendAlpha(ASTNodeCharClass::Ranges& ranges) -> void {
  appendUpper(ranges);
  appendLower(ranges);
}

auto appendAlnum(ASTNodeCharClass::Ranges& ranges) -> void {
  appendAlpha(ranges);
  appendDigit(ranges);
}

auto appendASCII(ASTNodeCharClass::Ranges& ranges) -> void { ranges.emplace_back('\x00', '\x7F'); }

auto appendBlank(ASTNodeCharClass::Ranges& ranges) -> void {
  ranges.emplace_back(' ', ' ');
  ranges.emplace_back('\t', '\t');
}

auto appendCntrl(ASTNodeCharClass::Ranges& ranges) -> void {
  ranges.emplace_back('\x00', '\x1F');
  ranges.emplace_back('\x7F', '\x7F');
}

auto appendGraph(ASTNodeCharClass::Ranges& ranges) -> void { ranges.emplace_back('\x21', '\x7E'); }

auto appendPrint(ASTNodeCharClass::Ranges& ranges) -> void { ranges.emplace_back('\x20', '\x7E'); }

auto appendSpace(ASTNodeCharClass::Ranges& ranges) -> void {
  ranges.emplace_back(' ', ' ');
  ranges.emplace_back('\t', '\t');
  ranges.emplace_back('\n', '\n');
  ranges.emplace_back('\r', '\r');
  ranges.emplace_back('\f', '\f');
  ranges.emplace_back('\v', '\v');
}

auto appendWord(ASTNodeCharClass::Ranges& ranges) -> void {
  appendAlnum(ranges);
  ranges.emplace_back('_', '_');
}

auto appendXdigit(ASTNodeCharClass::Ranges& ranges) -> void {
  ranges.emplace_back('A', 'F');
  ranges.emplace_back('a', 'f');
  appendDigit(ranges);
}

auto appendPunct(ASTNodeCharClass::Ranges& ranges) -> void {
  for (auto ch : std::initializer_list<char>{'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*',
                                             '+', ',', '-', '.', '/', ':', ';',  '<', '=', '>',
                                             '?', '@', '[', ']', '^', '_', '{',  '|', '}', '~'}) {
    ranges.emplace_back(ch, ch);
  }
}

auto shorthandCharClassToRanges(char ch, ASTNodeCharClass::Ranges& ranges) -> bool {
  switch (std::tolower(ch)) {
  case 'w': {
    appendWord(ranges);
    break;
  }
  case 'd': {
    appendDigit(ranges);
    break;
  }
  case 's': {
    appendSpace(ranges);
    break;
  }
  default: {
    return false;
  }
  }
  return true;
}

} // namespace regex_engine::parser
