#pragma once

#include "vulkan_header.h"
#include "vulkan_shader_stage.hpp"

BEGIN_PCCORE
    class VulkanPipeline
{
public:
    void Init(const VkGraphicsPipelineCreateInfo* _vkGraphicsPipelineCreateInfo, const VulkanShaderStage& _vulkanShaderStage, const
              VkPipelineLayout& _pipelineLayout, const VkRenderPass _renderPass);

    void Destroy();

    VkPipeline& Get();

    const VkPipeline Get() const;
    
private:
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    
};

END_PCCORE