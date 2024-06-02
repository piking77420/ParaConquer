#include "rendering/vulkan/vulkan_descriptor_set_layout.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE; 

void VulkanDescriptorSetLayout::Init(const std::vector<VkDescriptorSetLayoutBinding>& _vkDescriptorSetLayoutBindings)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = _vkDescriptorSetLayoutBindings.size();
    layoutInfo.pBindings = _vkDescriptorSetLayoutBindings.data();
    
    const VkResult result = vkCreateDescriptorSetLayout(VulkanInterface::GetDevice().device, &layoutInfo, nullptr, &m_VkDescriptorSetLayout);
    VK_CHECK_ERROR(result,"Failed to create vkCreateDescriptorSetLayout ");

}

void VulkanDescriptorSetLayout::Destroy()
{
    vkDestroyDescriptorSetLayout(VulkanInterface::GetDevice().device, m_VkDescriptorSetLayout, nullptr);
}

const VkDescriptorSetLayout& VulkanDescriptorSetLayout::Get() const
{
    return m_VkDescriptorSetLayout;
}

VkDescriptorSetLayout& VulkanDescriptorSetLayout::Get()
{
    return m_VkDescriptorSetLayout;
}
