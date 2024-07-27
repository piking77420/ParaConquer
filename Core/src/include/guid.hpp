#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <string>

#include "core_header.hpp"


BEGIN_PCCORE

class Guid
{
    static constexpr size_t Data4Size = 8;
    
public:
    constexpr Guid() = default;

    static constexpr Guid Empty() { return Guid(); }

    static Guid New();
    
    static Guid FromString(const char* str);
    
    [[nodiscard]]
    uint32_t GetData1() const;
    
    [[nodiscard]]
    uint16_t GetData2() const;
    
    [[nodiscard]]
    uint16_t GetData3() const;
    
    [[nodiscard]]
    const std::array<uint8_t, Data4Size>& GetData4() const;

   
    [[nodiscard]]
    bool operator==(const Guid& other) const;
    
    [[nodiscard]]
    bool operator!=(const Guid& other) const;

    explicit operator std::string() const;

private:
    uint32_t m_Data1 = 0;
    uint16_t m_Data2 = 0;
    uint16_t m_Data3 = 0;
    std::array<uint8_t, Data4Size> m_Data4 = {};

    friend struct std::hash<Guid>;
};

END_PCCORE

template <>
struct std::hash<PC_CORE::Guid>
{
    static constexpr inline size_t RandomValue = 0x9E3779B9;
    
    size_t operator()(const PC_CORE::Guid& guid) const noexcept
    {
        size_t result = 0;
        result ^= std::hash<decltype(guid.m_Data1)>()(guid.m_Data1) + RandomValue;
        result ^= std::hash<decltype(guid.m_Data2)>()(guid.m_Data2) + RandomValue + (result << 6) + (result >> 2);
        result ^= std::hash<decltype(guid.m_Data3)>()(guid.m_Data3) + RandomValue + (result << 6) + (result >> 2);

        for (size_t i = 0; i < PC_CORE::Guid::Data4Size; i++)
        {
            result ^= std::hash<std::remove_cvref_t<decltype(guid.m_Data4[i])>>()(guid.m_Data4[i]) + RandomValue + (result << 6) + (result >> 2);
        }

        return result;
    }
};
