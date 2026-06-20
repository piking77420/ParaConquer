#pragma once

#include <array>
#include <vector>
#include <string>

#include <Reflection/Reflector.hpp>
#include <LowRenderer/RhiTypedef.h>
#include <LowRenderer/RhiRenderPass.hpp>
#include <LowRenderer/RhiPipeline.hpp>
#include <Resources/ShaderSourceBinary.hpp>

BEGIN_PCCORE
    using SourceList = std::vector<std::pair<RhiPipeline::ShaderStageTypeBits, WeakObjectPtr<ShaderSourceBinary>>>;

    class ShaderProgram : public Resource
    {
    public:

        PC_CORE_API void OnParentReload(const Guid& _parentGuid) override;

        RhiPipeline::PipelineType GetPipelineType() const
        {
            return m_ShaderProgramPipelineType;
        }

        IMP_DYNAMIC_REFLECT()

        PC_CORE_API ShaderProgram(const std::string& _shaderName,
                                  RhiPipeline::PipelineType _shaderProgramPipelineType,
                                  const std::vector<std::pair<RhiPipeline::ShaderStageTypeBits, WeakObjectPtr<ShaderSourceBinary>>>&
                                  _sources);

        PC_CORE_API ShaderProgram(const std::string& _shaderName,
                                  RhiPipeline::PipelineType _shaderProgramPipelineType,
                                  const WeakObjectPtr<ShaderSourceBinary>& _source);

        PC_CORE_API ShaderProgram(std::string&& _shaderName, RhiPipeline::PipelineType _shaderProgramPipelineType,
                                  const WeakObjectPtr<ShaderSourceBinary>& _source);

        PC_CORE_API ShaderProgram()
        {
            DYNAMIC_REFLECT_INIT
        }

        PC_CORE_API ~ShaderProgram() override
        {
            if (m_RhiShaderProgram.use_count() > 1)
            {
                PC_LOGERROR("There is still a reference to the m_RhiShaderProgram, {}", Name)
            }


            PC_LOG("Destroy ShaderProgram, {}", Name);
        }

        RhiPipeline* Get()
        {
            return m_RhiShaderProgram.get();
        }

        const RhiPipeline* Get() const
        {
            return m_RhiShaderProgram.get();
        }

        RhiPipeline* operator->()
        {
            return m_RhiShaderProgram.get();
        }

        const RhiPipeline* operator->() const
        {
            return m_RhiShaderProgram.get();
        }

    protected:
        std::shared_ptr<RhiPipeline> m_RhiShaderProgram;

        RhiPipeline::PipelineType m_ShaderProgramPipelineType;

        REFLECT(ShaderProgram, Resource);
    };

END_PCCORE
