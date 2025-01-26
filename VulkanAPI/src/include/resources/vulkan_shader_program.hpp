#pragma once

#include <vulkan_header.h>
#include "resources/shader_program.h"

namespace Vulkan
{
    class VulkanShaderProgram : public PC_CORE::ShaderProgram
    {
    protected:
        static constexpr  std::array<vk::DynamicState,9> dynamicStateArray =
       {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
            vk::DynamicState::eLineWidth,
            vk::DynamicState::eDepthBias,
            vk::DynamicState::eBlendConstants,
            vk::DynamicState::eDepthBounds,
            vk::DynamicState::eStencilCompareMask,
            vk::DynamicState::eStencilWriteMask,
            vk::DynamicState::eStencilReference
        };
    
    public:

        VulkanShaderProgram(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo);
    
        VulkanShaderProgram() = default;
    
        ~VulkanShaderProgram() override;

    protected:

        vk::PipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        vk::Pipeline m_Pipeline = VK_NULL_HANDLE;
        
        void ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo, const PC_CORE::RasterizerInfo& _rasterizerInfo);

        void ParsePipelineColorBlendAttachmentState(vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState/*, const PC_CORE::*/);

        void ParseParsePipelineColorBlendAttachmentState(vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo, const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState );

    }; 
}


