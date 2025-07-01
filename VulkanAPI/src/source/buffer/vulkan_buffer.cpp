#include "buffer/vulkan_buffer.hpp"

#include "utils/helper_functions.hpp"
#include "utils/vulkan_buffer_helper.hpp"

Vulkan::VulkanBuffer::VulkanBuffer(const void* _data, uint32_t _sizeInByte, vk::BufferUsageFlags bufferUsage, PC_CORE::BufferMemoryUsage memoryUsage)
{
   if (_sizeInByte <= 0)
   {
       PC_LOGERROR("Trying to create a Vulkan buffer with zero size");
       return;
   }
    
    bufferAndAlloc.resize(MAX_FRAMES_IN_FLIGHT);
    assert(bufferAndAlloc.size() == MAX_FRAMES_IN_FLIGHT && "Unsuported resource dynamci size depender of thier memeory usage");

    switch (memoryUsage)
    {
    case PC_CORE::BufferMemoryUsage::Static:
        CreateStaticBuffer(_data, _sizeInByte, bufferUsage);
        break;
    case PC_CORE::BufferMemoryUsage::Mutable:
        break;
    case PC_CORE::BufferMemoryUsage::Dynamic:
        break;
    case PC_CORE::BufferMemoryUsage::Count:
    default:
        assert(false && "Invalid memory usage");
    }
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

void Vulkan::VulkanBuffer::CreateStaticBuffer(const void* _data, uint32_t _size, vk::BufferUsageFlags bufferUsage)
{
    auto& context = VulkanContext::GetContext();
    for (size_t i = 0; i < bufferAndAlloc.size(); i++)
        Utils::CreateBuffer(context.allocator, _size, bufferUsage, VMA_MEMORY_USAGE_GPU_ONLY
                            , reinterpret_cast<VkBuffer*>(&bufferAndAlloc[i].buffer), &bufferAndAlloc[i].alloc);
    
    SendDataToGPUMemory(_data, _size);
}

void Vulkan::VulkanBuffer::CreateMutableBuffer(const void* _data, uint32_t _size, vk::BufferUsageFlags bufferUsage)
{
    auto& context = VulkanContext::GetContext();
    for (size_t i = 0; i < bufferAndAlloc.size(); i++)
        Utils::CreateBuffer(context.allocator, _size, bufferUsage, VMA_MEMORY_USAGE_CPU_TO_GPU
                            , reinterpret_cast<VkBuffer*>(&bufferAndAlloc[i].buffer), &bufferAndAlloc[i].alloc);
    

    if (_data == nullptr)
        return;

    SendDataToGPUMemory(_data, _size);
}

void Vulkan::VulkanBuffer::CreateDynamicBuffer(const void* _data, uint32_t _size, vk::BufferUsageFlags bufferUsage)
{
    auto& context = VulkanContext::GetContext();
    
    for (size_t i = 0; i < bufferAndAlloc.size(); i++)
    {
        Utils::CreateBuffer(context.allocator, _size, bufferUsage, VMA_MEMORY_USAGE_CPU_TO_GPU
                            , reinterpret_cast<VkBuffer*>(&bufferAndAlloc[i].buffer), &bufferAndAlloc[i].alloc);
    }
 
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
