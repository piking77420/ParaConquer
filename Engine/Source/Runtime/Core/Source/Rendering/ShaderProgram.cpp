#include "Rendering/ShaderProgram.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Resources/ResourceManager.hpp"

using namespace PC_CORE;


PC_CORE_API void ShaderProgram::OnParentReload(const Guid& _parentGuid)
{
    PERF_REGION_SCOPED;

    // TODO FILTER PARENT 

    auto p = GetParentResource();

    std::vector<RhiPipeline::ShaderModule> sources;

    for (auto& code : p)
    {
        ObjectPtr<ShaderSourceBinary> shaderSourceBinary;
        if (ResourceManager::TryGetAs<ShaderSourceBinary>(code, &shaderSourceBinary))
        {
            sources.emplace_back(shaderSourceBinary->GetShaderStageType(), shaderSourceBinary->GetCode());
        }
    }

    if (!sources.empty())
        m_RhiShaderProgram->HotReload(sources);
}

ShaderProgram::ShaderProgram(const std::string& _shaderName, RhiPipeline::PipelineType _shaderProgramPipelineType,
                             const std::vector<std::pair<RhiPipeline::ShaderStageTypeBits, WeakObjectPtr<ShaderSourceBinary>>>&
                             _sources) : Resource(_shaderName), m_ShaderProgramPipelineType(_shaderProgramPipelineType)
{
    PERF_REGION_SCOPED;

    DYNAMIC_REFLECT_INIT

    // TODO lOOK if foreach shadersource binary if suitable for pipelyne type

    // if suitable

    for (const auto& source : _sources)
    {
        LinkDependencies(source.second.lock().get(), this);
    }
}

ShaderProgram::ShaderProgram(const std::string& _shaderName, RhiPipeline::PipelineType _shaderProgramPipelineType,
                             const WeakObjectPtr<ShaderSourceBinary>& _source)
    : Resource(_shaderName)
    , m_ShaderProgramPipelineType(_shaderProgramPipelineType)
{
    LinkDependencies(_source.lock().get(), this);
}

ShaderProgram::ShaderProgram(std::string&& _shaderName, RhiPipeline::PipelineType _shaderProgramPipelineType,
                             const WeakObjectPtr<ShaderSourceBinary>& _source) : Resource(_shaderName),
    m_ShaderProgramPipelineType(_shaderProgramPipelineType)
{
    LinkDependencies(_source.lock().get(), this);
}


std::vector<RhiPipeline::ShaderModule> ShaderProgram::SourceListToShaderModules(
    const SourceList& _sourceList)
{
    PERF_REGION_SCOPED;

    std::vector<RhiPipeline::ShaderModule> output;
    output.reserve(_sourceList.size());

    for (size_t i = 0; i < _sourceList.size(); i++)
    {
        if (auto s = _sourceList[i].second.lock())
        {
            output.emplace_back(_sourceList[i].first, s->GetCode());
        }
    }

    return output;
}
