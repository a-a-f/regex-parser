#include "parser/tests/printers.h"
#include "parser/visit_ast_node.h"

#include <cctype>
#include <cstdint>
#include <limits>

namespace regex_engine::tests {

using namespace regex_engine::parser;

namespace {

struct Printer {
  auto operator()(const ASTNodeAnchor& node, std::ostream& ostr) const -> void;
  auto operator()(const ASTNodeOr& node, std::ostream& ostr) const -> void;
  auto operator()(const ASTNodeAny& /*node*/, std::ostream& ostr) const -> void;
  auto operator()(const ASTNodeRepetition& node, std::ostream& ostr) const -> void;
  auto operator()(const ASTNodeCharClass& node, std::ostream& ostr) const -> void;
  auto operator()(const ASTNodeCharSequence& node, std::ostream& ostr) const -> void;
  auto operator()(const ASTNodeAdd& node, std::ostream& ostr) const -> void;
  auto operator()(const ASTNodeGroup& node, std::ostream& ostr) const -> void;
};

auto Printer::operator()(const ASTNodeAnchor& node, std::ostream& ostr) const -> void {
  ostr << "anchor{";
  switch (node.type) {
  case ASTNodeAnchor::Type::LINE_START: {
    ostr << "line start";
    break;
  }
  case ASTNodeAnchor::Type::LINE_END: {
    ostr << "line end";
    break;
  }
  case ASTNodeAnchor::Type::STRING_START: {
    ostr << "string start";
    break;
  }
  case ASTNodeAnchor::Type::STRING_END: {
    ostr << "string end";
    break;
  }
  default: {
    break;
  }
  }
  ostr << "}";
}

auto Printer::operator()(const ASTNodeOr& node, std::ostream& ostr) const -> void {
  ostr << "orr (";
  auto isFirst = true;
  for (const auto& child : node.children) {
    if (isFirst) {
      isFirst = false;
    } else {
      ostr << ", ";
    }
    visitASTNode(*this, *child, ostr);
  }
  ostr << ")";
}

auto Printer::operator()(const ASTNodeAny& /*node*/, std::ostream& ostr) const -> void {
  ostr << "any ()";
}

auto Printer::operator()(const ASTNodeRepetition& node, std::ostream& ostr) const -> void {
  if (node.min == 0 && !node.max) {
    ostr << "zeroOrMore (";
    visitASTNode(*this, *node.children[0], ostr);
    ostr << ")";
  } else if (node.min == 1 && !node.max) {
    ostr << "oneOrMore (";
    visitASTNode(*this, *node.children[0], ostr);
    ostr << ")";
  } else if (node.min == 0 && node.max && *node.max == 1) {
    ostr << "zeroOrOne (";
    visitASTNode(*this, *node.children[0], ostr);
    ostr << ")";
  } else {
    ostr << "repetition[" << node.min << ",";
    if (node.max) {
      ostr << *node.max;
    } else {
      ostr << "inf";
    }
    ostr << "] (";
    visitASTNode(*this, *node.children[0], ostr);
    ostr << ")";
  }
}

auto Printer::operator()(const ASTNodeCharClass& node, std::ostream& ostr) const -> void {
  auto emit = [&ostr](char ch) {
    if (!std::isspace(ch)) {
      ostr << ch;
    } else {
      ostr << "$" << std::hex << static_cast<uint16_t>(ch);
    }
  };

  ostr << "charclass (";
  ostr << "include [";
  for (auto [first, last] : node.matchingRanges) {
    ostr << "[";
    emit(first);
    ostr << ", ";
    emit(last);
    ostr << "]";
  }
  ostr << "] exclude [";
  for (auto [first, last] : node.nonMatchingRanges) {
    ostr << "[";
    emit(first);
    ostr << ", ";
    emit(last);
    ostr << "]";
  }
  ostr << "])";
}

auto Printer::operator()(const ASTNodeCharSequence& node, std::ostream& ostr) const -> void {
  auto emit = [&ostr](char ch) {
    if (!std::isspace(ch)) {
      ostr << "'" << ch << "'";
    } else {
      ostr << "'$" << std::hex << static_cast<uint16_t>(ch) << "'";
    }
  };

  ostr << "charsequence (";
  auto isFirst = true;
  for (auto c : node.chars) {
    if (isFirst) {
      isFirst = false;
    } else {
      ostr << ", ";
    }
    emit(c);
  }
  ostr << ")";
}

auto Printer::operator()(const ASTNodeAdd& node, std::ostream& ostr) const -> void {
  ostr << "add (";
  auto isFirst = true;
  for (const auto& child : node.children) {
    if (isFirst) {
      isFirst = false;
    } else {
      ostr << ", ";
    }
    visitASTNode(*this, *child, ostr);
  }
  ostr << ")";
}

auto Printer::operator()(const ASTNodeGroup& node, std::ostream& ostr) const -> void {
  ostr << (node.isCapturing ? "capturing" : "non-capturing") << " group (";
  visitASTNode(*this, *node.children[0], ostr);
  ostr << ")";
}

} // namespace

auto print(std::ostream& ostr, const ASTNodePtr& node) -> void {
  auto performer = Printer();
  visitASTNode(performer, *node, ostr);
}

} // namespace regex_engine::tests
