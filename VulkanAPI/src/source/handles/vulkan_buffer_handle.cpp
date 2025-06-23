#include "handles/vulkan_buffer_handle.hpp"

#include "low_renderer/rhi_typedef.h"

#include "helper_functions.hpp"
#include "rhi_vulkan_parser.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_context.hpp"


Vulkan::VulkanBufferHandle::VulkanBufferHandle(const PC_CORE::GPUBufferCreateInfo& _createInfo)
{
    auto& context = VulkanContext::GetContext();
    vk::BufferUsageFlags bufferUsageFlags = GetVulkanBufferUsageFlagsClient(_createInfo.usage);

    // Create Client Buffer
    CreateBuffer(context.allocator, _createInfo.dataSize, bufferUsageFlags, GetVmaMemoryUsage(_createInfo.usage)
                 , &m_VulkanBuffer, &m_VmaAllocation);

    switch (_createInfo.usage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
    case PC_CORE::BufferUsage::IndexBuffer:
        {
            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VmaAllocation stagingBufferAllocation = VK_NULL_HANDLE;

            CreateBuffer(context.allocator, _createInfo.dataSize, vk::BufferUsageFlagBits::eTransferSrc,
                         VMA_MEMORY_USAGE_CPU_TO_GPU, &stagingBuffer, &stagingBufferAllocation);
            SingleCommandBeginInfo singleCommandBeginInfo =
            {
                .device = context.GetDevice()->GetDevice(),
                .commandPool = context.transferCommandPool,
                .queue = context.mainQueue
            };

            vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

            void* data = nullptr;
            vmaMapMemory(context.allocator, stagingBufferAllocation, &data);
            memcpy(data, _createInfo.data, _createInfo.dataSize);

            vk::BufferCopy copyRegion = {};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = _createInfo.dataSize;

            commandBuffer.copyBuffer(stagingBuffer, m_VulkanBuffer, copyRegion);
            vmaUnmapMemory(context.allocator, stagingBufferAllocation);

            EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);

            DestroyBuffer(context.GetDevice()->GetDevice(), context.allocator, stagingBuffer, stagingBufferAllocation);
        }
        break;
    case PC_CORE::BufferUsage::UniformBuffer:
    case PC_CORE::BufferUsage::ShaderStorageBuffer:
        break;
    case PC_CORE::BufferUsage::Count:
    default:
        throw std::invalid_argument("Invalid buffer usage");
    }
    
}

Vulkan::VulkanBufferHandle::~VulkanBufferHandle()
{
    if (m_VulkanBuffer == VK_NULL_HANDLE || m_VmaAllocation == VK_NULL_HANDLE)
        return;
    auto& context = VulkanContext::GetContext();

    DestroyBuffer(context.GetDevice()->GetDevice(), context.allocator, m_VulkanBuffer, m_VmaAllocation);
}

void Vulkan::VulkanBufferHandle::MapBuffer(void** _mapPtr)
{
    auto& context = VulkanContext::GetContext();

    VK_CALL(static_cast<vk::Result>(vmaMapMemory(context.allocator, m_VmaAllocation, _mapPtr))) ;
}

void Vulkan::VulkanBufferHandle::UnMapBuffer()
{
    auto& context = VulkanContext::GetContext();

    vmaUnmapMemory(context.allocator, m_VmaAllocation);
}
