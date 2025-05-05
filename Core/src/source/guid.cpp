#include "guid.hpp"

#include <Windows.h>


using namespace PC_CORE;

Guid Guid::New()
{
	Guid guid;

	const HRESULT result = CoCreateGuid(reinterpret_cast<UUID*>(&guid));
	
	return guid;
}

Guid Guid::FromString(const char* str)
{
	Guid g;
	(void) sscanf_s(str, "%x-%hx-%hx-%hhx-%hhx-%hhx-%hhx-%hhx-%hhx-%hhx-%hhx", &g.data1, &g.data2, &g.data3, &g.data4[0], &g.data4[1], &g.data4[2], &g.data4[3], &g.data4[4], &g.data4[5], &g.data4[6], &g.data4[7]);

	return g;
}


bool Guid::operator==(const Guid& other) const
{
	return reinterpret_cast<const uint64_t*>(this)[0] == reinterpret_cast<const uint64_t*>(&other)[0] &&
		reinterpret_cast<const uint64_t*>(this)[1] == reinterpret_cast<const uint64_t*>(&other)[1];
}

bool Guid::operator!=(const Guid& other) const { return !(*this == other); }

Guid::operator std::string() const
{
	return std::format("{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}-{:X}", data1, data2, data3, data4[0], data4[1], data4[2], data4[3], data4[4], data4[5], data4[6], data4[7]);
}

