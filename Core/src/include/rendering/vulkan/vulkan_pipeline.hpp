#pragma once

#include "vulkan_header.h"
#include "vulkan_shader_stage.hpp"

BEGIN_PCCORE
    class VulkanPipeline
{
public:
    VkPipeline pipeline = VK_NULL_HANDLE;
    

    void Init(const VkGraphicsPipelineCreateInfo* _vkGraphicsPipelineCreateInfo, const VulkanShaderStage& _vulkanShaderStage, const
              VkPipelineLayout& _pipelineLayout, const VkRenderPass _renderPass);
    
private:
};

END_PCCORE