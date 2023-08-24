#pragma once

#include "parser/base_ast_node.h"
#include <ostream>

namespace regex_engine::tests {

auto print(std::ostream& ostr, const regex_engine::parser::ASTNodePtr& node) -> void;

}
