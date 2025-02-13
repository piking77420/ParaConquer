#pragma once

#include "component.h"
#include "ecs_header.h"
#include <array>

template <typename T, typename... Ts>
struct get_index;

// Found the type T
template <typename T, typename... Ts>
struct get_index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

// Not found yet, keep searching
template <typename T, typename U, typename... Ts>
struct get_index<T, U, Ts...> : std::integral_constant<std::size_t, 1 + get_index<T, Ts...>::value> {};

// Error case when the type is not found in the parameter pack
template <typename T>
struct get_index<T> {
  static_assert(sizeof(T) == 0, "Type not found in the list.");
};

BEGIN_PCCORE


template <ComponentDerived... Components>
struct Archetype
{
private:
  static PC_FORCE_INLINE std::array<const ReflectedType*, sizeof...(Components)> GetType()
  {
    std::array<const ReflectedType*, sizeof...(Components)> types;
    size_t i = 0;
    ((types[i++] = &(Reflector::GetType<Components>())), ...);
    return types;
  }
public:
  static inline std::array<const ReflectedType*, sizeof...(Components)> types = GetType();
};



END_PCCORE
