#pragma once


#include <cmath>
#include <cstdint>


#define BEGIN_PCCORE namespace PC_CORE {
#define END_PCCORE }

#if 0

#ifdef PCCORE_DLL
#define PC_CORE_API __declspec(dllexport)
#else
#define PC_CORE_API __declspec(dllimport)
#endif
#else
#define PC_CORE_API
#endif

#define DEFAULT_COPY_MOVE_OPERATIONS(type)         \
type(const type& other) noexcept = default;             \
type(type&& other) noexcept = default;                  \
type& operator=(const type& other) noexcept = default;  \
type& operator=(type&& other) noexcept = default;

