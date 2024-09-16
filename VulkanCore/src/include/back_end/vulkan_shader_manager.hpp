#pragma once

#include "rhi_typedef.h"
#include "vulkan_header.h"

namespace VK_NP
{
    
    class VulkanShaderManager
    {
    public:
        uint32_t CreateShaderFromSource(const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

        bool DestroyShader(uint32_t shaderHandle);
        
        VulkanShaderManager() = default;

        ~VulkanShaderManager();
        
    private:
        
        struct ShaderInternalFront
        {
            std::string shaderName;
            std::vector<PC_CORE::LowLevelShaderType> shaderTypes;
        };
        
        struct ShaderInternalBack
        {
            vk::Pipeline pipeline = VK_NULL_HANDLE;
            vk::PipelineLayout pipelineLayout = VK_NULL_HANDLE;
        };

        struct ShaderInternal
        {
            ShaderInternalFront shaderInternalFront;
            ShaderInternalBack shaderInternalBack;
        };
        
        std::unordered_map<uint32_t, ShaderInternal> m_InternalShadersMap;

        
    };    
}

