#include "LowRenderer/RhiFence.hpp"

namespace PC_CORE
{

	RhiFence::RhiFence(Rhi& _Rhi, const std::string& _name, const RhiFenceCreateInfo& _RhiFenceCreateInfo)
		: RhiObject(_Rhi, _name)
		, m_RhiFenceCreateInfo(_RhiFenceCreateInfo)
	{
	}
	
	RhiFence::RhiFence(Rhi& _Rhi, std::string&& _name, const RhiFenceCreateInfo& _RhiFenceCreateInfo)
		: RhiObject(_Rhi, std::move(_name))
		, m_RhiFenceCreateInfo(_RhiFenceCreateInfo)
	{
	}

}
