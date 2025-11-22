#include "LowRenderer/RhiResource.hpp"

namespace PC_CORE
{

RhiResource::RhiResource(Rhi& _Rhi, const std::string& _name, MemoryUsage memoryUsage)
	: RhiObject(_Rhi, _name)
	, m_MemoryUsage(memoryUsage)
{
}

RhiResource::RhiResource(Rhi& _Rhi, std::string&& _name, MemoryUsage memoryUsage)
	: RhiObject(_Rhi, _name)
	, m_MemoryUsage(memoryUsage)
{
}

}
