#pragma once

#include <array>
#include <vector>
#include <string>
#include <vulkan/vulkan_core.h>

#include "core_header.hpp"
#include "resource.hpp"
#include "low_renderer/descriptor_set.hpp"
#include "low_renderer/rhi_typedef.h"
#include "low_renderer/rhi_render_pass.hpp"
#include "low_renderer/rhi_shader_program.hpp"
#include "low_renderer/vertex.hpp"

BEGIN_PCCORE



    class ShaderProgram : public Resource , public IGpuResource
    {
    public:

        PC_CORE_API virtual std::shared_ptr<RhiResource> GetRhiHandle() const
        {
            return m_ShaderProgram;
        }

        // TODO ABSTRACT THIS 
        PC_CORE_API void AllocDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets, size_t set);
        // TODO ABSTRACT THIS 
        PC_CORE_API void FreeDescriptorSet(ShaderProgramDescriptorSets** _shaderProgramDescriptorSets);

        IMP_DYNAMIC_REFLECT()
    
        PC_CORE_API ShaderProgram(const std::string& _shaderName, const ProgramShaderCreateInfo& _programShaderCreateInfo);

        PC_CORE_API ShaderProgram()
        {
            DYNAMIC_REFLECT_INIT
        }

        PC_CORE_API virtual ~ShaderProgram() = default;

    private:
        std::shared_ptr<RhiShaderProgram> m_ShaderProgram;

        REFLECT(ShaderProgram, Resource);
    
    };

END_PCCORE
