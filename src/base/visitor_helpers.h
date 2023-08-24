#pragma once

#include "base/type_list.h"

#include <concepts>
#include <type_traits>

namespace regex_engine::base {

// Ensure that a class type has the same constness as its base class type.
template <typename Base, typename Derived>
  requires std::is_base_of_v<Base, Derived>
using FixConst =
    std::conditional_t<std::is_const_v<std::remove_reference_t<Base>>, const Derived&, Derived&>;

template <typename Functor, typename Base, typename ObjectType, typename... Args>
concept IsObjectVisitor =
    requires(Functor&& functor, FixConst<Base, ObjectType>& base, Args&&... args) {
      { functor.operator()(base, std::forward<Args>(args)...) } -> std::same_as<void>;
    };

template <typename Functor, typename ObjectType, typename Base, typename... Args>
concept IsNoexcept = requires(Functor&& functor, FixConst<Base, ObjectType>& base, Args&&... args) {
  noexcept(functor.operator()(base, std::forward<Args>(args)...));
};

template <typename Functor, typename... ObjectTypes> struct FunctorTraits;

//
// Some traits we need to constrain a visitor.
// We require a visitor to be a class implementing function call operators
// for each type of object to be visited.
template <typename Functor, typename... ObjectTypes>
struct FunctorTraits<Functor, TypeList<ObjectTypes...>> {
  // Check if the functor statisfies visitor requirements for a set of object types
  // derived from a given base class type.
  template <typename Base, typename... Args>
  static constexpr auto IS_VISITOR = (IsObjectVisitor<Functor, Base, ObjectTypes, Args...> && ...);

  // The functor is considered as noexcept if all of its implemented function call
  // operators are themselves noexcept.
  template <typename Base, typename... Args>
  static constexpr bool IS_NOEXCEPT = (IsNoexcept<Functor, ObjectTypes, Base, Args...> && ...);
};

} // namespace regex_engine::base
