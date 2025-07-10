#include "buffer/vulkan_buffer.hpp"

#include "low_renderer/rhi.hpp"
#include "utils/helper_functions.hpp"
#include "utils/rhi_vulkan_parser.hpp"
#include "utils/vulkan_buffer_helper.hpp"

void Vulkan::VulkanBuffer::MapData(void** _mapData)
{
    auto& context = VulkanContext::GetContext();

    vk::Result r = static_cast<vk::Result>(vmaMapMemory(context.allocator,
                                                        bufferAndAlloc[PC_CORE::Rhi::GetFrameIndex()].alloc, _mapData));
    VK_CALL(r);

    if (r != vk::Result::eSuccess)
        *_mapData = nullptr;
}

void Vulkan::VulkanBuffer::UnMapData()
{
    auto& context = VulkanContext::GetContext();
    vmaUnmapMemory(context.allocator, bufferAndAlloc[PC_CORE::Rhi::GetFrameIndex()].alloc);
}

Vulkan::VulkanBuffer::VulkanBuffer(const void* _data, uint32_t _sizeInByte, vk::BufferUsageFlags bufferUsage, PC_CORE::MemoryLocalisation _visibility, PC_CORE::MemoryUsage memoryUsage)
{
   if (_sizeInByte <= 0)
   {
       PC_LOGERROR("Trying to create a Vulkan buffer with zero size");
       return;
   }
    
    bufferAndAlloc.resize(MAX_FRAMES_IN_FLIGHT);
    assert(bufferAndAlloc.size() == MAX_FRAMES_IN_FLIGHT && "Unsuported resource dynamci size depender of thier memeory usage");
    
    CreateInternalBuffer(_data, _sizeInByte, bufferUsage, Utils::RhiMemoryUsageToVulkan(_visibility));

    // TODO TAKE ACOUT OF MEMMROY USAGE
    /*
    switch (memoryUsage)
    {
    case PC_CORE::MemoryUsage::Static:
        break;
    case PC_CORE::MemoryUsage::Mutable:
        break;
    case PC_CORE::MemoryUsage::Dynamic:
        break;
    case PC_CORE::MemoryUsage::Count:
    default:
        assert(false && "Invalid memory usage");
    }*/
}

Vulkan::VulkanBuffer::VulkanBuffer(uint32_t _sizeInByte, vk::BufferUsageFlags bufferUsage,
    PC_CORE::MemoryLocalisation _visibility, PC_CORE::MemoryUsage memoryUsage)
{
    if (_sizeInByte <= 0)
    {
        PC_LOGERROR("Trying to create a Vulkan buffer with zero size");
        return;
    }
    
    bufferAndAlloc.resize(MAX_FRAMES_IN_FLIGHT);
    assert(bufferAndAlloc.size() == MAX_FRAMES_IN_FLIGHT && "Unsuported resource dynamci size depender of thier memeory usage");
    
    CreateInternalBuffer(nullptr, _sizeInByte, bufferUsage, Utils::RhiMemoryUsageToVulkan(_visibility));
}

Vulkan::VulkanBuffer::~VulkanBuffer()
{
    for (auto& alloc : bufferAndAlloc)
    {
        if (alloc.buffer == VK_NULL_HANDLE || alloc.alloc == VK_NULL_HANDLE)
            return;
        auto& context = VulkanContext::GetContext();

        Utils::DestroyBuffer(context.GetDevice()->GetDevice(), context.allocator, alloc.buffer,alloc.alloc);
        alloc.buffer = VK_NULL_HANDLE;
        alloc.alloc = VK_NULL_HANDLE;
    }
}

void Vulkan::VulkanBuffer::CreateInternalBuffer(const void* _data, uint32_t _size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
    vk::BufferUsageFlags clientFlag = bufferUsage;

    bool SandDataToGPU = false;

    switch (memoryUsage)
    {
        break;
    case VMA_MEMORY_USAGE_GPU_ONLY:
        clientFlag |= vk::BufferUsageFlagBits::eTransferDst;
        SandDataToGPU = true;
        break;
    case VMA_MEMORY_USAGE_CPU_ONLY:
        break;
    case VMA_MEMORY_USAGE_CPU_TO_GPU:
        clientFlag |= vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
        SandDataToGPU = true;
        break;
    case VMA_MEMORY_USAGE_GPU_TO_CPU:
        clientFlag |= vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
        SandDataToGPU = true;
        break;
    case VMA_MEMORY_USAGE_CPU_COPY:
    case VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED:
    case VMA_MEMORY_USAGE_AUTO:
    case VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE:
    case VMA_MEMORY_USAGE_AUTO_PREFER_HOST:
    case VMA_MEMORY_USAGE_MAX_ENUM:
    case VMA_MEMORY_USAGE_UNKNOWN:
    default:
        assert(false && "Invalid memory usage");
    }
    
    auto& context = VulkanContext::GetContext();
    for (size_t i = 0; i < bufferAndAlloc.size(); i++)
        Utils::CreateBuffer(context.allocator, _size, clientFlag, memoryUsage
                            , reinterpret_cast<VkBuffer*>(&bufferAndAlloc[i].buffer), &bufferAndAlloc[i].alloc);

    if (_data != nullptr && SandDataToGPU)
        SendDataToGPUMemory(_data, _size);
}




void Vulkan::VulkanBuffer::SendDataToGPUMemory(const void* _data, uint32_t _size)
{
    if (_data == nullptr || _size == 0)
        return;
    
    auto& context = VulkanContext::GetContext();
    
    std::vector<BufferAndAlloc> stagingBufferAndAlloc;
    stagingBufferAndAlloc.resize(bufferAndAlloc.size());
    
    for (size_t i = 0; i < stagingBufferAndAlloc.size(); i++)
    {
        Utils::CreateBuffer(context.allocator, _size, vk::BufferUsageFlagBits::eTransferSrc,
                    VMA_MEMORY_USAGE_CPU_TO_GPU, reinterpret_cast<VkBuffer*>(&stagingBufferAndAlloc[i].buffer), &stagingBufferAndAlloc[i].alloc);
    }

    // TODO done not use fence here
    // wait frame n+1
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
    copyRegion.size = _size;

    for (size_t i = 0; i < stagingBufferAndAlloc.size(); i++)
    {
        vmaMapMemory(context.allocator, stagingBufferAndAlloc[i].alloc, &mappedData[i]);
        memcpy(mappedData[i], _data, _size);
        vmaUnmapMemory(context.allocator, stagingBufferAndAlloc[i].alloc);
    }

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

    for (size_t i = 0; i < stagingBufferAndAlloc.size(); i++)
        commandBuffer.copyBuffer(stagingBufferAndAlloc[i].buffer, bufferAndAlloc[i].buffer, copyRegion);
    EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);

    for (auto& bufferAndAlloc : stagingBufferAndAlloc)
        vmaDestroyBuffer(context.allocator, bufferAndAlloc.buffer, bufferAndAlloc.alloc);
}
