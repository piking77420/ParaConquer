#pragma once

#include <cstdint>
#include <type_traits>

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
    inline enumName& operator|=(enumName& lhs, const enumName rhs) { return lhs = static_cast<enumName>(lhs | rhs); }


#define DEFAULT_CONSTRUCTOR_DESTRUCTOR(type) \
type() = default; \
~type() = default;
#define DEFAULT_COPY_MOVE_OPERATIONS(type)         \
    type(const type& other) noexcept = default;             \
    type(type&& other) noexcept = default;                  \
    type& operator=(const type& other) noexcept = default;  \
    type& operator=(type&& other) noexcept = default;
#define DELETE_COPY_MOVE_OPERATIONS(type)          \
    type(const type& other) = delete;              \
    type(type&& other) = delete;                   \
    type& operator=(const type& other) = delete;   \
    type& operator=(type&& other) = delete;


BEGIN_PCCORE
    template <typename T, typename = void>
    struct IsPolymorphicCustom : std::false_type
    {
    };

    template <typename T>
    struct IsPolymorphicCustom<T, std::void_t<decltype(dynamic_cast<const volatile void*>(std::declval<T*>())
                                 )>> : std::true_type
    {
    };

    template <typename T, typename M>
    // ReSharper disable once CppInconsistentNaming
    static constexpr size_t offset_of(M T::* _member)
    {
        return reinterpret_cast<size_t>(&(reinterpret_cast<const volatile T*>(0)->*_member));
    }

    static inline uint32_t AlignUp(uint32_t value, uint32_t alignment)
    {
        return (value + alignment - 1) & ~(alignment - 1);
    }

END_PCCORE

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;