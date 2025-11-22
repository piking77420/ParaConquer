#include "LowRenderer/RhiObject.hpp"

namespace PC_CORE
{

RhiObject::~RhiObject()
{

}

RhiObject::RhiObject(Rhi& _Rhi, const std::string& _name)
	: m_Rhi(_Rhi)
	, m_Name(_name)
{
}

RhiObject::RhiObject(Rhi& _Rhi, std::string&& _name)
	: m_Rhi(_Rhi)
	, m_Name(std::move(_name))
{
}

}
