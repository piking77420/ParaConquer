#include "LowRenderer/DescriptorSet.hpp"

#include "LowRenderer/Rhi.hpp"

namespace PC_CORE
{

ShaderProgramDescriptorSets::ShaderProgramDescriptorSets(Rhi& _Rhi, const std::string& _name)
	: RhiObject(_Rhi, _name)
{

}

ShaderProgramDescriptorSets::ShaderProgramDescriptorSets(Rhi& _Rhi, std::string&& _name)
	: RhiObject(_Rhi, std::move(_name))
{
}

ShaderProgramDescriptorSets::~ShaderProgramDescriptorSets()
{

}

}
