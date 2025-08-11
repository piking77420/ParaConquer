#pragma once

#include <Array>
#include <Cstdint>
#include <Format>
#include <String>

#include "CoreHeader.hpp"
#include "Reflection/Reflector.hpp"

BEGIN_PCCORE

class PC_CORE_API Guid
{ 
public:
    static constexpr size_t Data4Size = 8;

    DEFAULT_COPY_MOVE_OPERATIONS(Guid)

    constexpr explicit Guid() = default;

    ~Guid() = default;


    static constexpr Guid Empty() { return Guid(); }

    static Guid New();
    
    static Guid FromString(const char* str);

    [[nodiscard]]
    bool operator==(const Guid& other) const;
    
    [[nodiscard]]
    bool operator!=(const Guid& other) const;

    explicit operator std::string() const;

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

namespace std {
    template <>
    struct hash<PC_CORE::Guid> {
        static constexpr inline size_t RandomValue = 0x9E3779B9;

        size_t operator()(const PC_CORE::Guid& guid) const noexcept {
            size_t result = 0;

            result ^= std::hash<decltype(guid.data1)>{}(guid.data1) + RandomValue;
            result ^= std::hash<decltype(guid.data2)>{}(guid.data2) + RandomValue + (result << 6) + (result >> 2);
            result ^= std::hash<decltype(guid.data3)>{}(guid.data3) + RandomValue + (result << 6) + (result >> 2);

            for (size_t i = 0; i < PC_CORE::Guid::Data4Size; ++i) {
                result ^= std::hash<std::remove_cv_t<std::remove_reference_t<decltype(guid.data4[i])>>>{}(guid.data4[i]) 
                          + RandomValue + (result << 6) + (result >> 2);
            }

            return result;
        }
    };
}