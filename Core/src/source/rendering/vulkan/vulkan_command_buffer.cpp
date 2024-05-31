#include "rendering/vulkan/vulkan_command_buffer.hpp"

using namespace PC_CORE;

void VulkanCommandBuffer::SetNbrofAllocation(size_t size)
{
    m_CommandsBuffer.resize(size);
}

VkCommandBuffer& VulkanCommandBuffer::operator[](size_t index)
{
    return m_CommandsBuffer.at(index);
}

const VkCommandBuffer& VulkanCommandBuffer::operator[](size_t index) const
{
    return m_CommandsBuffer.at(index);
}

VkCommandBuffer* VulkanCommandBuffer::GetPtr()
{
    return m_CommandsBuffer.data();
}
