#include "rendering/vulkan/vulkan_descriptor_pool.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanDescriptorPool::Init(const std::vector<VkDescriptorSetLayoutBinding>& _vkDescriptorSetLayoutBindings,
        size_t _allocationCout)
{
    m_MaxAllocationFromPool = static_cast<uint32_t>(_allocationCout);

    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.resize(_vkDescriptorSetLayoutBindings.size());

    for (size_t i = 0; i < poolSizes.size(); i++)
    {
        poolSizes[i].type =  _vkDescriptorSetLayoutBindings[i].descriptorType;
        poolSizes[i].descriptorCount = _vkDescriptorSetLayoutBindings[i].descriptorCount;
    }
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = m_MaxAllocationFromPool;

    const VkResult result = vkCreateDescriptorPool(VulkanInterface::GetDevice().device, &poolInfo, nullptr, &m_VkDescriptorPool);
    VK_CHECK_ERROR(result,"failed to create descriptor pool!");
}

void VulkanDescriptorPool::Destroy()
{
    vkDestroyDescriptorPool(VulkanInterface::GetDevice().device, m_VkDescriptorPool, nullptr);

}

const VkDescriptorPool& VulkanDescriptorPool::Get() const
{
    return m_VkDescriptorPool;
}

VkDescriptorPool& VulkanDescriptorPool::Get()
{
    return m_VkDescriptorPool;
}

void VulkanDescriptorPool::CreateDescriptorSet(VkDescriptorSetLayout _setLayout, uint32_t _nbr, VkDescriptorSet* _data) const
{
    std::vector<VkDescriptorSetLayout> layouts(VulkanInterface::GetNbrOfImage(), _setLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_VkDescriptorPool;
    allocInfo.descriptorSetCount = _nbr;
    allocInfo.pSetLayouts = layouts.data();
    
    if (vkAllocateDescriptorSets(VulkanInterface::GetDevice().device, &allocInfo, _data) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}
