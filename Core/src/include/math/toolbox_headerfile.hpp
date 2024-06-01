#pragma once

#include <cmath>
#include <array>

#define ZERO 0.001f

#define TOOLBOX_INLINE _forceinline 

template<typename T>
constexpr TOOLBOX_INLINE bool IsZero(T _zero) { return std::abs(_zero) <= ZERO; }

template<typename T>
constexpr TOOLBOX_INLINE bool IsEqualTo(T _value, T _compare) { return IsZero<T>(std::abs(_compare) - std::abs(_value)); }

