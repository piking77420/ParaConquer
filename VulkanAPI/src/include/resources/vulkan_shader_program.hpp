#pragma once

#include "resources/shader_program.h"

namespace Vulkan
{
    class VulkanShaderProgram : public PC_CORE::ShaderProgram
    {
    public:

        VulkanShaderProgram(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo);
    
        VulkanShaderProgram() = default;
    
        ~VulkanShaderProgram() override = default;
    }; 
}


