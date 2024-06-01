#include "rendering/vulkan/vulkan_index_buffer.hpp"

#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanIndexBuffer::Init(const std::vector<uint32_t>& _indicies)
{
    // Send data to host memory then copy this buffer in to device memory
    const VkDevice& device = VulkanInterface::GetDevice().device;
    const VkDeviceSize bufferSize = sizeof(_indicies[0]) * _indicies.size();
    
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        , &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _indicies.data(), (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        , &m_Buffer, &m_BufferMemory);
    
    CopyBuffer(stagingBuffer, m_Buffer, bufferSize);
    
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}
