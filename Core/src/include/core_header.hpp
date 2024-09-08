#pragma once


#include <cmath>
#include <cstdint>


#define BEGIN_PCCORE namespace PC_CORE {
#define END_PCCORE }

#ifdef PC_CORE_DLL

#ifdef PC_CORE_EXPORT 
#define PC_CORE_API __declspec(dllexport)
#elif PC_CORE_IMPORT
#define PC_CORE_API __declspec(dllimport)
#endif
#else
#define PC_CORE_API
#endif // PC_CORE_API




#define DEFAULT_COPY_MOVE_OPERATIONS(type)         \
type(const type& other) noexcept = default;             \
type(type&& other) noexcept = default;                  \
type& operator=(const type& other) noexcept = default;  \
type& operator=(type&& other) noexcept = default;

