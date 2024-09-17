#pragma once

#include "rhi_typedef.h"
#include "vulkan_header.h"
#include <spirv_reflect.h>


#include "vulkan_shader_compiler.hpp"


namespace VK_NP
{
    class VulkanShaderManager
    {
    public:
        bool CreateShaderFromSource(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreatInfo,
                                    const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

        bool DestroyShader(const std::string& _shaderName);

        VulkanShaderManager();

        ~VulkanShaderManager();

    private:
        struct ShaderStageInfo
        {
            std::string name;
            PC_CORE::LowLevelShaderStageType shaderStage;
            SpvReflectShaderModule reflectShaderModule;
        };

        struct ShaderInternalBack
        {
            std::vector<ShaderStageInfo> shaderStages;

            vk::Pipeline pipeline = VK_NULL_HANDLE;
            vk::PipelineLayout pipelineLayout = VK_NULL_HANDLE;
        };


        std::unordered_map<std::string, ShaderInternalBack> m_InternalShadersMap;

        vk::Device m_Device;

        vk::Extent2D m_SwapChainExtent;

        void ShaderReflexion(ShaderInternalBack* _shaderInternalBack,
                             const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

        void CreatePipelineFromModule(const std::vector<ShaderStageInfo>& shaderStages,
                                      const std::vector<vk::ShaderModule>& _shaderModules, vk::Pipeline* _ooutPipeline, vk::PipelineLayout* _outLayout);

        void CreateModuleForProgram(const std::string _shaderName,
                                    const ShaderStageInfo& shaderStageInfo, const std::vector<uint8_t>& _shaderCode,
                                    vk::ShaderModule* _pipileLineModulee);

        
        // Static func
        static vk::PipelineShaderStageCreateInfo CreateShaderStageCreateInfoFromSource(
            PC_CORE::LowLevelShaderStageType type, const SpvReflectShaderModule& _spvReflectShaderModule);

        static vk::ShaderStageFlagBits ShaderBitFromType(PC_CORE::LowLevelShaderStageType _shaderType);
    };
}
