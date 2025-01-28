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

        vk::PipelineBindPoint GetPipelineBindPoint() const;

        vk::Pipeline GetPipeline() const;

    protected:

        vk::PipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        vk::Pipeline m_Pipeline = VK_NULL_HANDLE;
        
        void ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo, const PC_CORE::RasterizerInfo& _rasterizerInfo);

        void ParsePipelineColorBlendAttachmentState(vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState/*, const PC_CORE::*/);

        void ParseParsePipelineColorBlendAttachmentState(vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo, const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState );
        
        vk::VertexInputBindingDescription ParseVertexInputBindingDescription(const PC_CORE::VertexInputBindingDescrition& _vertexInputBindingDescrition);

        vk::VertexInputAttributeDescription ParseVertexInputAttributeDescription(const PC_CORE::VertexAttributeDescription& _vertexAttributeDescription);

        vk::PipelineVertexInputStateCreateInfo ParseVertexInputState(
            const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInfo,
            std::vector<vk::VertexInputBindingDescription>* _vertexInputBindingDescriptions
            , std::vector<vk::VertexInputAttributeDescription>* _vertexInputAttributeDescriptions);

    }; 
}


