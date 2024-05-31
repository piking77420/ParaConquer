#include "rendering/vulkan/vulkan_pipeline.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_shader_stage.hpp"

using namespace PC_CORE;

void VulkanPipeline::Init(const VkGraphicsPipelineCreateInfo* _vkGraphicsPipelineCreateInfo, const VulkanShaderStage& _vulkanShaderStage, const VkPipelineLayout& _pipelineLayout
    ,const VkRenderPass _renderPass)
{
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo = *_vkGraphicsPipelineCreateInfo;
    constexpr  std::array<VkDynamicState,2> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    vkGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    vkGraphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(_vulkanShaderStage.vkPipelineShaderStageCreateInfo.size());
    vkGraphicsPipelineCreateInfo.pStages = _vulkanShaderStage.vkPipelineShaderStageCreateInfo.data();
    vkGraphicsPipelineCreateInfo.pDynamicState = &dynamicState;
    vkGraphicsPipelineCreateInfo.layout = _pipelineLayout;
    vkGraphicsPipelineCreateInfo.renderPass = _renderPass;
    vkGraphicsPipelineCreateInfo.subpass = 0;
    vkGraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    
    VkResult res = vkCreateGraphicsPipelines(VulkanInterface::GetDevice().device, VK_NULL_HANDLE, 1, &vkGraphicsPipelineCreateInfo, nullptr, &pipeline);
    VK_CHECK_ERROR(res,"Failed To create GraphicPipeline")
    
}
