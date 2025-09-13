#pragma once

#include <Cmath>
#include <Cstdint>



#define BEGIN_PCCORE namespace PC_CORE {
#define END_PCCORE }


#ifdef PC_CORE_EXPORT
#define PC_CORE_API __declspec(dllexport)
#else PC_CORE_IMPORT
#define PC_CORE_API __declspec(dllimport)
#endif

#define PC_FORCE_INLINE __forceinline


#define DEFAULT_COPY_MOVE_OPERATIONS(type)         \
type(const type& other) noexcept = default;             \
type(type&& other) noexcept = default;                  \
type& operator=(const type& other) noexcept = default;  \
type& operator=(type&& other) noexcept = default;

#define ENUM_FLAGS(enumName)                                                                                                           \
    static_assert(std::is_enum_v<enumName>, #enumName " must be an enum");                                                             \
                                                                                                                                        \
      inline std::underlying_type_t<enumName> operator&(const enumName lhs, const enumName rhs) { return static_cast<std::underlying_type_t<enumName>>(lhs) & static_cast<std::underlying_type_t<enumName>>(rhs); } \
    inline enumName operator|(const enumName lhs, const enumName rhs) { return static_cast<enumName>(static_cast<std::underlying_type_t<enumName>>(lhs) | static_cast<std::underlying_type_t<enumName>>(rhs)); } \
    inline std::underlying_type_t<enumName> operator&(const std::underlying_type_t<enumName> lhs, const enumName rhs) { return lhs & static_cast<std::underlying_type_t<enumName>>(rhs); } \
    inline std::underlying_type_t<enumName> operator|(const std::underlying_type_t<enumName> lhs, const enumName rhs) { return lhs | static_cast<std::underlying_type_t<enumName>>(rhs); } \
    inline std::underlying_type_t<enumName> operator&(const enumName lhs, const std::underlying_type_t<enumName> rhs) { return static_cast<std::underlying_type_t<enumName>>(lhs) & rhs; } \
    inline std::underlying_type_t<enumName> operator|(const enumName lhs, const std::underlying_type_t<enumName> rhs) { return static_cast<std::underlying_type_t<enumName>>(lhs) | rhs; } \
    inline std::underlying_type_t<enumName>& operator&=(std::underlying_type_t<enumName>& lhs, const enumName rhs) { return lhs = lhs & rhs; } \
    inline std::underlying_type_t<enumName>& operator|=(std::underlying_type_t<enumName>& lhs, const enumName rhs) { return lhs = lhs | rhs; } \
    inline enumName& operator&=(enumName& lhs, const enumName rhs) { return lhs = static_cast<enumName>(lhs & rhs); } \
    inline enumName& operator|=(enumName& lhs, const enumName rhs) { return lhs = static_cast<enumName>(lhs | rhs); } \

                                                                                                                                                                                                                            

#define DEFAULT_CONSTRUCTOR_DESTRUCTOR(type) \
type() = default; \
~type() = default; \

#define DEFAULT_COPY_MOVE_OPERATIONS(type)         \
    type(const type& other) noexcept = default;             \
    type(type&& other) noexcept = default;                  \
    type& operator=(const type& other) noexcept = default;  \
    type& operator=(type&& other) noexcept = default; \

#define DELETE_COPY_MOVE_OPERATIONS(type)          \
    type(const type& other) = delete;              \
    type(type&& other) = delete;                   \
    type& operator=(const type& other) = delete;   \
    type& operator=(type&& other) = delete; \




BEGIN_PCCORE

template <typename T, typename = void>
struct is_polymorphic_custom : std::false_type {};

template <typename T>
struct is_polymorphic_custom<T, std::void_t<decltype(dynamic_cast<const volatile void*>(std::declval<T*>()))>> : std::true_type {};

// DO BOT USE IT out side Reflector
template <typename T, typename M>
  static constexpr size_t offset_of(M T::*member) { 
    return reinterpret_cast<size_t>(&(reinterpret_cast<T const volatile*>(0)->*member));
}
END_PCCORE