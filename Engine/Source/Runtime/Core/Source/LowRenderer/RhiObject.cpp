#include "LowRenderer/RhiObject.hpp"

#include "LowRenderer/Rhi.hpp"

namespace PC_CORE
{

RhiObject::RhiObject(Rhi& _Rhi)
	: m_Rhi(_Rhi)
{
	
}

RhiObject::~RhiObject()
{

}

Rhi& RhiObject::GetRhi()
{
	return m_Rhi;
}


}
