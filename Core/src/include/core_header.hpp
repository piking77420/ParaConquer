#pragma once
#include <cmath>
#include <cstdint>



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


#define ENUM_FLAGS(enumName)                                                                                                                                                                                                \
    static_assert(std::is_enum_v<enumName>, "enumName must be a valid enum type");                                                                                                                                          \
                                                                                                                                                                                                                            \
    inline std::underlying_type_t<enumName> operator&(const enumName left, const enumName right) { return static_cast<std::underlying_type_t<enumName>>(left) & static_cast<std::underlying_type_t<enumName>>(right); }     \
    inline std::underlying_type_t<enumName> operator|(const enumName left, const enumName right) { return static_cast<std::underlying_type_t<enumName>>(left) | static_cast<std::underlying_type_t<enumName>>(right); }     \
    inline std::underlying_type_t<enumName> operator&(const std::underlying_type_t<enumName> left, const enumName right) { return left & static_cast<std::underlying_type_t<enumName>>(right); }                            \
    inline std::underlying_type_t<enumName> operator|(const std::underlying_type_t<enumName> left, const enumName right) { return left | static_cast<std::underlying_type_t<enumName>>(right); }                            \
    inline std::underlying_type_t<enumName> operator&(const enumName left, const std::underlying_type_t<enumName> right) { return static_cast<std::underlying_type_t<enumName>>(left) & right; }                            \
    inline std::underlying_type_t<enumName> operator|(const enumName left, const std::underlying_type_t<enumName> right) { return static_cast<std::underlying_type_t<enumName>>(left) | right; }                            \
    inline std::underlying_type_t<enumName>& operator&=(std::underlying_type_t<enumName>& left, const enumName right) { return left = left & right; }                                                                       \
    inline std::underlying_type_t<enumName>& operator|=(std::underlying_type_t<enumName>& left, const enumName right) { return left = left | right; }                                                                       \
                                                                                                                                                                                                                            

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