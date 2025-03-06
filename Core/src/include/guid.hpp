#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <string>

#include "core_header.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

class Guid
{ 
public:
    static constexpr size_t Data4Size = 8;

    PC_CORE_API constexpr Guid() = default;

    PC_CORE_API static constexpr Guid Empty() { return Guid(); }

    PC_CORE_API static Guid New();
    
    PC_CORE_API static Guid FromString(const char* str);

    [[nodiscard]]
    PC_CORE_API bool operator==(const Guid& other) const;
    
    [[nodiscard]]
    PC_CORE_API bool operator!=(const Guid& other) const;

    PC_CORE_API explicit operator std::string() const;

    uint32_t data1 = 0;
    uint16_t data2 = 0;
    uint16_t data3 = 0;
    std::array<uint8_t, Data4Size> data4 = {};

};

REFLECT(Guid)
REFLECT_MEMBER(Guid, data1)
REFLECT_MEMBER(Guid, data2)
REFLECT_MEMBER(Guid, data3)
REFLECT_MEMBER(Guid, data4)



END_PCCORE

template <>
struct std::hash<PC_CORE::Guid>
{
    static constexpr inline size_t RandomValue = 0x9E3779B9;
    
    size_t operator()(const PC_CORE::Guid& guid) const noexcept
    {

        size_t result = 0;
        result ^= std::hash<decltype(guid.data1)>()(guid.data1) + RandomValue;
        result ^= std::hash<decltype(guid.data2)>()(guid.data2) + RandomValue + (result << 6) + (result >> 2);
        result ^= std::hash<decltype(guid.data3)>()(guid.data3) + RandomValue + (result << 6) + (result >> 2);

        for (size_t i = 0; i < PC_CORE::Guid::Data4Size; i++)
        {
            result ^= std::hash<std::remove_cvref_t<decltype(guid.data4[i])>>()(guid.data4[i]) + RandomValue + (result << 6) + (result >> 2);
        }

        return result;
    }
};
