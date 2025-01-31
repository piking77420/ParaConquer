#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/descriptor_set.hpp"
#include "low_renderer/rhi_typedef.h"

BEGIN_PCCORE

    class ShaderProgram : public Resource
    {
    public:

        PC_CORE_API virtual void AllocDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets) = 0;
        
        PC_CORE_API virtual void FreeDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets) = 0;
        
        PC_CORE_API ShaderProgram(const ProgramShaderCreateInfo& _programShaderCreateInfo);

        PC_CORE_API ShaderProgram() = default;

        PC_CORE_API ~ShaderProgram() = default;
    protected:
        ProgramShaderCreateInfo m_ProgramShaderCreateInfo;

        
    };

END_PCCORE
