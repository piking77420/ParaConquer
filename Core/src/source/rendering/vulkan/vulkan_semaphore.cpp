#include "rendering/vulkan/vulkan_semaphore.h"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanSemaphore::Init(const VkSemaphoreCreateInfo& semaphoreCreateInfo, size_t nbr)
{
    const VkDevice& device = VulkanInterface::GetDevice().device;
    Destroy();
    
    m_Semaphores.resize(nbr);
    for (size_t i = 0; i < m_Semaphores.size() ; i++)
    {
        const VkResult r = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_Semaphores[i]);

        VK_CHECK_ERROR(r,"Failed to create Semaphore");
    }
}

VkSemaphore& VulkanSemaphore::operator[](size_t index)
{
    return m_Semaphores.at(index);
}

const VkSemaphore& VulkanSemaphore::operator[](size_t index) const
{
    return m_Semaphores.at(index);
}

VkSemaphore* VulkanSemaphore::GetPtr()
{
    return m_Semaphores.data();
}

void VulkanSemaphore::Destroy()
{
    const VkDevice& device = VulkanInterface::GetDevice().device;
    
    for (size_t i = 0; i < m_Semaphores.size(); i++)
    {
        vkDestroySemaphore(device, m_Semaphores[i], nullptr);
    }
}
