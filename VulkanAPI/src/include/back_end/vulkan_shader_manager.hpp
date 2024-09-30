﻿#pragma once

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

        vk::CommandPool m_CommandPool;

        vk::CommandBuffer m_CommandBuffer;

        void DestroyInternalShaders(ShaderInternal* _shaderInternalBack);

        static void FillShaderInfo(ShaderInternal* _shaderInternalBack,
                             const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource);

        void CreatePipelineGraphicPointFromModule(const PC_CORE::ShaderInfo& ShaderInfo,
            const std::vector<vk::PipelineShaderStageCreateInfo>& _shaderStageCreateInfos, vk::PipelineLayout _pipelineLayout, vk::Pipeline* _outPipeline);

        void CreatePipelineLayoutFromSpvReflectModule(vk::Device _device, ShaderInternal* _shaderInternal);
        
        vk::PipelineVertexInputStateCreateInfo GetVertexInputStateCreateInfoFromShaderStruct(const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInfo, std::vector<vk::VertexInputBindingDescription>*
            _bindingDescriptions, std::vector<vk::VertexInputAttributeDescription>* _attributeDescriptions);
        
        static vk::ShaderStageFlagBits ShaderBitFromType(PC_CORE::LowLevelShaderStageType _shaderType);
        
    };
}
