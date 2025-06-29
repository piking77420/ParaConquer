#include "buffer/vulkan_vertex_buffer.hpp"

#include "utils/helper_functions.hpp"
#include "utils/vulkan_buffer_helper.hpp"


Vulkan::VulkanVertexBuffer::VulkanVertexBuffer(const void* _data, uint32_t _sizeInByte, PC_CORE::BufferMemoryUsage _usage) : m_VulkanBuffer(_usage)
{
    auto& context = VulkanContext::GetContext();

    vk::BufferUsageFlags outBufferUsage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;

    std::vector<BufferAndAlloc> stagingBufferAndAlloc;
    stagingBufferAndAlloc.resize(m_VulkanBuffer.bufferAndAlloc.size());

    for (size_t i = 0; i < stagingBufferAndAlloc.size(); i++)
    {
        Utils::CreateBuffer(context.allocator, _sizeInByte, outBufferUsage,
             VMA_MEMORY_USAGE_GPU_ONLY, reinterpret_cast<VkBuffer*>(&m_VulkanBuffer.bufferAndAlloc[i].buffer), &m_VulkanBuffer.bufferAndAlloc[i].alloc);

        Utils::CreateBuffer(context.allocator, _sizeInByte, vk::BufferUsageFlagBits::eTransferSrc,
            VMA_MEMORY_USAGE_CPU_TO_GPU, reinterpret_cast<VkBuffer*>(&stagingBufferAndAlloc[i].buffer), &stagingBufferAndAlloc[i].alloc);
    }
    
    Utils::SingleCommandBeginInfo singleCommandBeginInfo =
    {
        .device = context.GetDevice()->GetDevice(),
        .commandPool = context.transferCommandPool,
        .queue = context.mainQueue
    };


    std::unique_ptr<void* []> mappedData = std::make_unique<void* []>(stagingBufferAndAlloc.size());

    vk::BufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = _sizeInByte;

    for (size_t i = 0; i < stagingBufferAndAlloc.size(); i++)
    {
        vmaMapMemory(context.allocator, stagingBufferAndAlloc[i].alloc, &mappedData[i]);
        memcpy(mappedData[i], _data, _sizeInByte);
        vmaUnmapMemory(context.allocator, stagingBufferAndAlloc[i].alloc);
    }


    vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

    for (size_t i = 0; i < stagingBufferAndAlloc.size(); i++)
        commandBuffer.copyBuffer(stagingBufferAndAlloc[i].buffer, m_VulkanBuffer.bufferAndAlloc[i].buffer, copyRegion);
    EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);


    for (auto& bufferAndAlloc : stagingBufferAndAlloc)
        vmaDestroyBuffer(context.allocator, bufferAndAlloc.buffer, bufferAndAlloc.alloc);
    
}
