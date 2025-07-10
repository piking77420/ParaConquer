#include "rendering/shader_program.hpp"

#include "low_renderer/rhi.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;


PC_CORE_API void ShaderProgram::OnParentReload(const Guid& _parentGuid)
{
    PERF_REGION_SCOPED;

    // TODO FILTER PARENT 

    auto p = GetParentResource();

    std::vector<std::pair<PC_CORE::ShaderStageType, std::string>> sources;

    
    for (auto& code : p)
    {
        std::shared_ptr<ShaderSourceBinary> shaderSourceBinary;
        if (ResourceManager::TryGetAs<PC_CORE::ShaderSourceBinary>(code, &shaderSourceBinary))
        {
            sources.emplace_back(shaderSourceBinary->GetShaderStageType(), shaderSourceBinary->GetPath());
        }
    }

    if (!sources.empty())
        m_RhiShaderProgram->HotReload(sources);
}

void ShaderProgram::AllocDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets, size_t set)
{
    PERF_REGION_SCOPED;
    m_RhiShaderProgram->AllocDescriptorSet(_shaderProgramDescriptorSets, set);
}

void ShaderProgram::FreeDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets)
{
    PERF_REGION_SCOPED;
    m_RhiShaderProgram->FreeDescriptorSet(_shaderProgramDescriptorSets);
}

ShaderProgram::ShaderProgram(const std::string& _shaderName, ShaderProgramPipelineType _shaderProgramPipelineType,
    const std::vector<std::pair<ShaderStageType, std::weak_ptr<ShaderSourceBinary>>>& _sources) : Resource(_shaderName), m_ShaderProgramPipelineType(_shaderProgramPipelineType)
{
    PERF_REGION_SCOPED;
    
    DYNAMIC_REFLECT_INIT

    // TODO lOOK if foreach shadersource binary if suitable for pipelyne type

    // if suitable
    
    for (const auto& source : _sources)
    {
        Resource::LinkDependencies(source.second.lock().get(), this);
    }
}

std::vector<std::pair<PC_CORE::ShaderStageType, std::string>> PC_CORE::ShaderProgram::SourceListToSourcePath(
    const SourceList& _sourceList)
{

    PERF_REGION_SCOPED;
    
    std::vector<std::pair<PC_CORE::ShaderStageType, std::string>> output;
    output.reserve(_sourceList.size());

    for (size_t i = 0; i < _sourceList.size(); i++)
    {
        if (auto s = _sourceList[i].second.lock())
        {
            output.emplace_back(_sourceList[i].first, s->GetPath());
        }
    }

    return output;
}

