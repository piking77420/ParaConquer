#include "Guid.hpp"

#ifdef _WIN32
    #include <objbase.h>
#else
    #include <uuid/uuid.h>
#endif

using namespace PC_CORE;

Guid Guid::New()
{
    Guid guid;
#ifdef _WIN32
    const HRESULT result = CoCreateGuid(reinterpret_cast<UUID*>(&guid));
#else
    uuid_t nativeGuid {};
    uuid_generate_random(nativeGuid);

    guid.data1 =
        (static_cast<uint32_t>(nativeGuid[0]) << 24U) |
        (static_cast<uint32_t>(nativeGuid[1]) << 16U) |
        (static_cast<uint32_t>(nativeGuid[2]) << 8U) |
        static_cast<uint32_t>(nativeGuid[3]);

    guid.data2 =
        static_cast<uint16_t>(
            (static_cast<uint16_t>(nativeGuid[4]) << 8U) |
            static_cast<uint16_t>(nativeGuid[5]));

    guid.data3 =
        static_cast<uint16_t>(
            (static_cast<uint16_t>(nativeGuid[6]) << 8U) |
            static_cast<uint16_t>(nativeGuid[7]));

    std::copy_n(nativeGuid + 8, guid.data4.size(), guid.data4.begin());
#endif
    return guid;
}

Guid Guid::FromString(const char* str)
{
    if(!str)
    return Guid::Empty();

    const std::string_view value {str};

    // Canonical UUID format:
    // XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    if(value.size() != 36 ||
       value[8]  != '-' ||
       value[13] != '-' ||
       value[18] != '-' ||
       value[23] != '-') {
        return Guid::Empty();
    }

    unsigned int data1 {};
    unsigned int data2 {};
    unsigned int data3 {};
    unsigned int data4[8] {};

    const int parsed = std::sscanf(
        str,
        "%8x-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x",
        &data1,
        &data2,
        &data3,
        &data4[0],
        &data4[1],
        &data4[2],
        &data4[3],
        &data4[4],
        &data4[5],
        &data4[6],
        &data4[7]
    );

    if(parsed != 11)
        return Guid::Empty();

    Guid guid {};
    guid.data1 = static_cast<uint32_t>(data1);
    guid.data2 = static_cast<uint16_t>(data2);
    guid.data3 = static_cast<uint16_t>(data3);

    for(size_t i = 0; i < 8; ++i)
        guid.data4[i] = static_cast<uint8_t>(data4[i]);

    return guid;

    return Guid::Empty();
}

Guid Guid::FromString(const std::string& _str)
{
    return FromString(_str.c_str());
}


bool Guid::operator==(const Guid& other) const
{
    return reinterpret_cast<const uint64_t*>(this)[0] == reinterpret_cast<const uint64_t*>(&other)[0] &&
        reinterpret_cast<const uint64_t*>(this)[1] == reinterpret_cast<const uint64_t*>(&other)[1];
}

bool Guid::operator!=(const Guid& other) const { return !(*this == other); }

Guid::operator std::string() const
{
    return std::format("{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}", data1, data2, data3, data4[0],
                       data4[1], data4[2], data4[3], data4[4], data4[5], data4[6], data4[7]);
}
