#include "rendering/vulkan/vulkan_descriptor_set_layout.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE; 

void VulkanDescriptorSetLayout::Init(const std::vector<VkDescriptorSetLayoutBinding>& _vkDescriptorSetLayoutBindings)
{
    m_desciptorSetLayoutBinding = _vkDescriptorSetLayoutBindings;
    InitDescriptorLayout();
}

void VulkanDescriptorSetLayout::CreateDesciptorPool(VkDescriptorPool* descriptorPool)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.resize(m_desciptorSetLayoutBinding.size());

    for (size_t i = 0; i < poolSizes.size(); i++)
    {
        poolSizes[i].type =  m_desciptorSetLayoutBinding[i].descriptorType;
        poolSizes[i].descriptorCount = m_desciptorSetLayoutBinding[i].descriptorCount;
    }
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    // ?
    poolInfo.maxSets = static_cast<uint32_t>(VulkanInterface::GetNbrOfImage());

    const VkResult result = vkCreateDescriptorPool(VulkanInterface::GetDevice().device, &poolInfo, nullptr, descriptorPool);
    VK_CHECK_ERROR(result,"failed to create descriptor pool!");
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

void VulkanDescriptorSetLayout::CreateDescriptorSet(VkDescriptorPool vkDescriptorPool,uint32_t _nbr, VkDescriptorSet* _data) const
{
    std::vector<VkDescriptorSetLayout> layouts(VulkanInterface::GetNbrOfImage(), Get());

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vkDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanInterface::GetNbrOfImage());
    allocInfo.pSetLayouts = layouts.data();
    
    if (vkAllocateDescriptorSets(VulkanInterface::GetDevice().device, &allocInfo, _data) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

void VulkanDescriptorSetLayout::InitDescriptorLayout()
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = m_desciptorSetLayoutBinding.size();
    layoutInfo.pBindings = m_desciptorSetLayoutBinding.data();
    
    const VkResult result = vkCreateDescriptorSetLayout(VulkanInterface::GetDevice().device, &layoutInfo, nullptr, &m_VkDescriptorSetLayout);
    VK_CHECK_ERROR(result,"Failed to create vkCreateDescriptorSetLayout ");
    
}


