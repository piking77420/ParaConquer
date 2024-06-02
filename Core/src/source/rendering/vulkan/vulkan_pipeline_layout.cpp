#include "rendering\vulkan\vulkan_pipeline_layout.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkankPipelineLayout::Init(const std::vector<VkDescriptorSetLayout>& _vkDescriptorSetLayouts)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = _vkDescriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = _vkDescriptorSetLayouts.data();

    VkResult res = vkCreatePipelineLayout(VulkanInterface::GetDevice().device, &pipelineLayoutInfo, nullptr, &m_VkPipelineLayout);
    VK_CHECK_ERROR(res,"Failed To create GraphicPipeline")

}

void VulkankPipelineLayout::Destroy()
{
    vkDestroyPipelineLayout(VulkanInterface::GetDevice().device, m_VkPipelineLayout, nullptr);
}

VkPipelineLayout& VulkankPipelineLayout::Get()
{
    return m_VkPipelineLayout;
}

const VkPipelineLayout& VulkankPipelineLayout::Get() const
{
    return m_VkPipelineLayout;
}
