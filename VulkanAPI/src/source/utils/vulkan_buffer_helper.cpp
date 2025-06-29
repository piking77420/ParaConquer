#include "utils/vulkan_buffer_helper.hpp"

#include "utils/rhi_vulkan_parser.hpp"

using namespace Vulkan::Utils;

vk::BufferUsageFlags Vulkan::Utils::GetVulkanBufferUsageFlagsClient(PC_CORE::BufferUsage bufferUsage)
{
    vk::BufferUsageFlags based = RhiToBufferUsage(bufferUsage);
    
    switch (bufferUsage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
    case PC_CORE::BufferUsage::IndexBuffer:
        return based | vk::BufferUsageFlagBits::eTransferDst;
    case PC_CORE::BufferUsage::UniformBuffer:
    case PC_CORE::BufferUsage::ShaderStorageBuffer:
        return based;
    case PC_CORE::BufferUsage::Count:
        break;
    default: ;
    }

}

void Vulkan::Utils::CreateBuffer(VmaAllocator allocator, size_t size, vk::BufferUsageFlags _bufferUsageFlagBits, VmaMemoryUsage _memoryUsage,
                          VkBuffer* _outBuffer, VmaAllocation* _outAllocation)
{
    vk::BufferCreateInfo bufferCreate{};
    bufferCreate.sType = vk::StructureType::eBufferCreateInfo;
    bufferCreate.size = static_cast<VkDeviceSize>(size);
    bufferCreate.usage = _bufferUsageFlagBits;
    bufferCreate.sharingMode = vk::SharingMode::eExclusive;

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = _memoryUsage;


    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&
            bufferCreate), &allocationInfo, _outBuffer, _outAllocation,
        nullptr
    )));
}

void Vulkan::Utils::DestroyBuffer(vk::Device device, VmaAllocator allocator, VkBuffer buffer, VmaAllocation allocation)
{
    vmaDestroyBuffer(  allocator, buffer, allocation);
}

VmaMemoryUsage Vulkan::Utils::GetVmaMemoryUsage(PC_CORE::BufferUsage bufferUsage)
{
    switch (bufferUsage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
    case PC_CORE::BufferUsage::IndexBuffer:
        return VMA_MEMORY_USAGE_GPU_ONLY;
    case PC_CORE::BufferUsage::UniformBuffer:
        return VMA_MEMORY_USAGE_CPU_TO_GPU;
    case PC_CORE::BufferUsage::ShaderStorageBuffer:
        return static_cast<VmaMemoryUsage>(VMA_MEMORY_USAGE_CPU_TO_GPU | VMA_MEMORY_USAGE_GPU_TO_CPU);
    case PC_CORE::BufferUsage::Count:
        break;
    default: ;
    }
}
