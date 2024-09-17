#pragma once

#include "vulkan_header.h"
#include "rhi_typedef.h"

namespace VK_NP
{
    
    class VulkanShaderCompiler
    {
    public:
        
        static void CompileShaderToSpv(const std::string _shaderName, const std::string& shaderSource, PC_CORE::LowLevelShaderStageType  _lowLevelShaderStage, std::vector<uint32_t>* _sprivSource);
        
    private:

    };

}