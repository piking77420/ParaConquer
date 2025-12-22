#include "LowRenderer/RhiResource.hpp"

namespace PC_CORE
{

RhiResource::RhiResource(Rhi& _Rhi, const std::string& _name)
	: RhiObjectT(_Rhi, _name)
{
}

RhiResource::RhiResource(Rhi& _Rhi, std::string&& _name)
	: RhiObjectT(_Rhi, std::move(_name))
{
}

}
