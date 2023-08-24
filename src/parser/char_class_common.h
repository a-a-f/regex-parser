#pragma once

#include "parser/ast_node_char_class.h"

namespace regex_engine::parser {

auto appendUpper(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendLower(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendDigit(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendAlpha(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendAlnum(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendASCII(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendBlank(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendCntrl(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendGraph(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendPrint(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendSpace(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendWord(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendXdigit(ASTNodeCharClass::Ranges& ranges) -> void;
auto appendPunct(ASTNodeCharClass::Ranges& ranges) -> void;

[[nodiscard]] auto shorthandCharClassToRanges(char ch, ASTNodeCharClass::Ranges& ranges) -> bool;

} // namespace regex_engine::parser
