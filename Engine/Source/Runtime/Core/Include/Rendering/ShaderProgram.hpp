#pragma once

#include <array>
#include <vector>
#include <string>

#include "CoreHeader.hpp"
#include "Objectptr.hpp"
#include "Reflection/Reflector.hpp"
#include "LowRenderer/DescriptorSet.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "LowRenderer/RhiRenderPass.hpp"
#include "LowRenderer/RhiShaderProgram.hpp"
#include "LowRenderer/Vertex.hpp"
#include "Resources/ShaderSourceBinary.hpp"

BEGIN_PCCORE
    using SourceList = std::vector<std::pair<RhiShaderProgram::ShaderStageType, WeakObjectPtr<ShaderSourceBinary>>>;

    class ShaderProgram : public Resource
    {
    public:

        PC_CORE_API void OnParentReload(const Guid& _parentGuid) override;

        RhiShaderProgram::PipelineType GetPipelineType() const
        {
            return m_ShaderProgramPipelineType;
        }

        IMP_DYNAMIC_REFLECT()

        PC_CORE_API ShaderProgram(const std::string& _shaderName,
                                  RhiShaderProgram::PipelineType _shaderProgramPipelineType,
                                  const std::vector<std::pair<RhiShaderProgram::ShaderStageType, WeakObjectPtr<ShaderSourceBinary>>>&
                                  _sources);

        PC_CORE_API ShaderProgram(const std::string& _shaderName,
                                  RhiShaderProgram::PipelineType _shaderProgramPipelineType,
                                  const WeakObjectPtr<ShaderSourceBinary>& _source);

        PC_CORE_API ShaderProgram(std::string&& _shaderName, RhiShaderProgram::PipelineType _shaderProgramPipelineType,
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

        RhiShaderProgram* Get()
        {
            return m_RhiShaderProgram.get();
        }

        const RhiShaderProgram* Get() const
        {
            return m_RhiShaderProgram.get();
        }

        RhiShaderProgram* operator->()
        {
            return m_RhiShaderProgram.get();
        }

        const RhiShaderProgram* operator->() const
        {
            return m_RhiShaderProgram.get();
        }

    protected:
        std::shared_ptr<RhiShaderProgram> m_RhiShaderProgram;

        RhiShaderProgram::PipelineType m_ShaderProgramPipelineType;

        static std::vector<RhiShaderProgram::ShaderModule> SourceListToShaderModules(const SourceList& _sourceList);

        REFLECT(ShaderProgram, Resource);
    };

END_PCCORE
