#pragma once


#include <cmath>
#include <cstdint>

#define BEGIN_PCCORE namespace PC_CORE {
#define END_PCCORE }

#define DEFAULT_COPY_MOVE_OPERATIONS(type)         \
type(const type& other) noexcept = default;             \
type(type&& other) noexcept = default;                  \
type& operator=(const type& other) noexcept = default;  \
type& operator=(type&& other) noexcept = default;