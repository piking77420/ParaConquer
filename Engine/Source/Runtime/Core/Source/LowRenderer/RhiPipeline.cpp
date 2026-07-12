#include <LowRenderer/RhiPipeline.hpp>
#include <LowRenderer/RhiRenderPass.hpp>
#include <Resources/ShaderSourceBinary.hpp>

namespace PC_CORE
{

PC_CORE::RhiPipeline::RhiPipeline(Rhi& _Rhi)
    : RhiObjectT(_Rhi)
{

}

RhiPipeline& RhiPipeline::SetShaderModules(const std::vector<ShaderModuleBinary>& _ShaderModules)
{
    m_Modules.emplace(_ShaderModules);
    return *this;
}


}


