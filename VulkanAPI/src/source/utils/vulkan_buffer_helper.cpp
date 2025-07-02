#include "utils/vulkan_buffer_helper.hpp"

#include "utils/rhi_vulkan_parser.hpp"

using namespace Vulkan::Utils;


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
