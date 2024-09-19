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

        void BindProgram(const std::string& _shaderName, vk::CommandBuffer _commandBuffer);

        void ReloadShader(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreatInfo,
                                                 const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

        VulkanShaderManager();

        ~VulkanShaderManager();

    private:
        struct ShaderStageInfo
        {
            std::string name;
            PC_CORE::LowLevelShaderStageType shaderStage;
            SpvReflectShaderModule reflectShaderModule;
        };

        struct ShaderInternal
        {
            std::vector<ShaderStageInfo> shaderStages;

            vk::Pipeline pipeline = VK_NULL_HANDLE;
            vk::PipelineLayout pipelineLayout = VK_NULL_HANDLE;
        };

        VulkanShaderCompiler m_ShaderCompiler;
        
        std::unordered_map<std::string, ShaderInternal> m_InternalShadersMap;

        vk::Device m_Device;

        vk::Extent2D m_SwapChainExtent;

        void DestroyInternalShaders(ShaderInternal* _shaderInternalBack);

        static void FillShaderInfo(ShaderInternal* _shaderInternalBack,
                             const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

        void CreatePipelineFromModule(const std::vector<vk::PipelineShaderStageCreateInfo>& _shaderStageCreateInfos, vk::Pipeline* _outPipeline, vk::PipelineLayout* _outLayout);

        void CreateModuleForProgram(const std::string _shaderName,
                                    const ShaderStageInfo& shaderStageInfo, const std::vector<uint8_t>& _shaderCode,
                                    vk::ShaderModule* _pipileLineModulee);

        VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo();
        
        static vk::PipelineShaderStageCreateInfo FromSourceToModule();

        static vk::ShaderStageFlagBits ShaderBitFromType(PC_CORE::LowLevelShaderStageType _shaderType);
    };
}
