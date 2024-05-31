#include "rendering/vulkan/vulkan_fence.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanFence::Init(const VkFenceCreateInfo& _semaphoreCreateInfo, size_t _nbr)
{
    const VkDevice& device = VulkanInterface::GetDevice().device;
    Destroy();
    m_Fences.resize(_nbr);
    
    for (size_t i = 0; i < m_Fences.size() ; i++)
    {
        VkResult r = vkCreateFence(device, &_semaphoreCreateInfo, nullptr, &m_Fences[i]);

        VK_CHECK_ERROR(r,"Failed to create fence");
    }
}

VkFence& VulkanFence::operator[](const size_t _index)
{
    return m_Fences.at(_index);
}

const VkFence& VulkanFence::operator[](size_t _index) const
{
    return m_Fences.at(_index);
}

VkFence* VulkanFence::GetPtr()
{
    return m_Fences.data();
}

void VulkanFence::Destroy()
{
    const VkDevice& device = VulkanInterface::GetDevice().device;

    for (size_t i = 0; i < m_Fences.size() ; i++)
    {
        vkDestroyFence(device, m_Fences[i], nullptr);
    }
}
