#include "rendering/vulkan/vulkan_buffer.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void PC_CORE::VulkanBuffer::Destroy()
{
    if (m_Buffer != VK_NULL_HANDLE)
        vmaDestroyBuffer(VulkanInterface::GetAllocator(), m_Buffer, m_Allocation);
    
}

VkBuffer VulkanBuffer::GetHandle()
{
    return m_Buffer;
}

