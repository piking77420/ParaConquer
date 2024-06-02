#include "rendering/vulkan/vulkan_descriptor_pool.hpp"

using namespace PC_CORE;

const VkDescriptorPool& VulkanDescriptorPool::Get() const
{
    return m_VkDescriptorPool;
}

VkDescriptorPool& VulkanDescriptorPool::Get()
{
    return m_VkDescriptorPool;
}
