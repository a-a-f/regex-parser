#pragma once

#include "base/refine_cast.h"
#include "base/visitor_helpers.h"
#include "parser/ast_node_add.h"
#include "parser/ast_node_anchor.h"
#include "parser/ast_node_any.h"
#include "parser/ast_node_char_class.h"
#include "parser/ast_node_char_sequence.h"
#include "parser/ast_node_group.h"
#include "parser/ast_node_or.h"
#include "parser/ast_node_repetition.h"

#include <concepts>
#include <expected>
#include <type_traits>

namespace regex_engine::parser {

namespace impl {
using namespace base;

using ASTNodes = TypeList<ASTNodeAnchor, ASTNodeAdd, ASTNodeAny, ASTNodeCharClass,
                          ASTNodeCharSequence, ASTNodeGroup, ASTNodeOr, ASTNodeRepetition>;

template <typename Functor, typename Base, typename... Args>
concept IsVisitor = std::same_as<std::decay_t<Base>, BaseASTNode> &&
                    FunctorTraits<Functor, ASTNodes>::template IS_VISITOR<Base, Args...>;

template <typename Functor, typename Base, typename... Args>
  requires std::same_as<std::decay_t<Base>, BaseASTNode>
static constexpr auto IsNoexceptFunctor =
    FunctorTraits<Functor, ASTNodes>::template IS_NOEXCEPT<Base, Args...>;
} // namespace impl

enum class VisitASTNodeError { UNHANDLED_AST_NODE };

// Visit a given AST node with help from a visitor.
// The visitor must be a class implementing function call operators
// for each node type that can be visited.
template <typename Visitor, typename... Args>
  requires impl::IsVisitor<Visitor, BaseASTNode, Args...>
auto visitASTNode(const Visitor& visitor, BaseASTNode& node,
                  Args&&... args) noexcept(impl::IsNoexceptFunctor<Visitor, BaseASTNode, Args...>) {
  using namespace base;
  using namespace impl;

  using VisitorReturnType =
      std::invoke_result_t<Visitor, FixConst<BaseASTNode, ASTNodeAnchor>, Args...>;
  using ReturnType = std::expected<VisitorReturnType, VisitASTNodeError>;

  auto applyVisitor = [&visitor](auto&& concreteNode, auto&&... args) noexcept(
                          impl::IsNoexceptFunctor<Visitor, BaseASTNode, Args...>) {
    if constexpr (std::same_as<VisitorReturnType, void>) {
      visitor(concreteNode, std::forward<decltype(args)>(args)...);
      return ReturnType{};
    } else {
      return ReturnType{visitor(concreteNode, std::forward<decltype(args)>(args)...)};
    }
  };

  switch (node.getType()) {
  case ASTNodeType::ANCHOR: {
    auto&& concreteNode = refineCast<FixConst<BaseASTNode, ASTNodeAnchor>>(node);
    return applyVisitor(concreteNode, std::forward<Args>(args)...);
  }
  case ASTNodeType::ADD: {
    auto&& concreteNode = refineCast<FixConst<BaseASTNode, ASTNodeAdd>>(node);
    return applyVisitor(concreteNode, std::forward<Args>(args)...);
  }
  case ASTNodeType::ANY: {
    auto&& concreteNode = refineCast<FixConst<BaseASTNode, ASTNodeAny>>(node);
    return applyVisitor(concreteNode, std::forward<Args>(args)...);
  }
  case ASTNodeType::CHAR_CLASS: {
    auto&& concreteNode = refineCast<FixConst<BaseASTNode, ASTNodeCharClass>>(node);
    return applyVisitor(concreteNode, std::forward<Args>(args)...);
  }
  case ASTNodeType::CHAR_SEQUENCE: {
    auto&& concreteNode = refineCast<FixConst<BaseASTNode, ASTNodeCharSequence>>(node);
    return applyVisitor(concreteNode, std::forward<Args>(args)...);
  }
  case ASTNodeType::REPETITION: {
    auto&& concreteNode = refineCast<FixConst<BaseASTNode, ASTNodeRepetition>>(node);
    return applyVisitor(concreteNode, std::forward<Args>(args)...);
  }
  case ASTNodeType::OR: {
    auto&& concreteNode = refineCast<FixConst<BaseASTNode, ASTNodeOr>>(node);
    return applyVisitor(concreteNode, std::forward<Args>(args)...);
  }
  case ASTNodeType::GROUP: {
    auto&& concreteNode = refineCast<FixConst<BaseASTNode, ASTNodeGroup>>(node);
    return applyVisitor(concreteNode, std::forward<Args>(args)...);
  }
  default: {
    return ReturnType{std::unexpected{VisitASTNodeError::UNHANDLED_AST_NODE}};
  }
  }
}

} // namespace regex_engine::parser
