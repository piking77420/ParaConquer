#include "back_end/vulkan_buffer_map.hpp"


VK_NP::VulkanBufferMap::BufferKeyHandle VK_NP::VulkanBufferMap::CreateBuffer(VulkanContext* _context, vk::CommandPool _commandPool,
    uint32_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE usage)
{
    vk::Device device = _context->device;

    if (usage & PC_CORE::BUFFER_USAGE_VERTEX)
    {
        return CreateGPUBufferFromCPU(_context, _commandPool, _size, _data, usage);
    }
    else if (usage & PC_CORE::BUFFER_USAGE_INDEX)
    {
        return CreateGPUBufferFromCPU(_context, _commandPool, _size, _data, usage);
    }
    else if (usage & PC_CORE::BUFFER_USAGE_UNIFORM)
    {
        vk::Buffer buffer = VK_NULL_HANDLE;
        BufferInternal bufferInternal;
        VmaAllocation vmaAllocation;
        VmaAllocationInfo vmaAllocationInfo;
        CreateBufferAndAlloc(_context, _size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
            VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, &buffer, &vmaAllocation, &vmaAllocationInfo);

        bufferInternal.allocation = vmaAllocation;
        bufferInternal.allocationInfo = vmaAllocationInfo;
        bufferInternal.size = _size;
        bufferInternal.usage = usage;

        BufferKeyHandle BufferKeyHandle = *reinterpret_cast<VK_NP::VulkanBufferMap::BufferKeyHandle*>(&buffer);
        m_BuffersMap.insert({ BufferKeyHandle,bufferInternal });
        return BufferKeyHandle;

    }
    else if (usage & PC_CORE::BUFFER_USAGE_SHADER_STORAGE)
    {
        
    }
    

    return {};
}

const VK_NP::BufferInternal& VK_NP::VulkanBufferMap::GetBufferUsage(BufferKeyHandle _bufferKeyHandle)
{
    if (!m_BuffersMap.contains(_bufferKeyHandle))
        throw std::runtime_error("Buffer does not exist in vulkan Buffer Map");

    const BufferInternal* bufferInternal = &m_BuffersMap.at(_bufferKeyHandle);

    return *bufferInternal;
}


bool VK_NP::VulkanBufferMap::DestroyBuffer(VulkanContext* _context, BufferKeyHandle bufferKeyHandle)
{
    if (!m_BuffersMap.contains(bufferKeyHandle))
    {
        throw std::runtime_error("Buffer does not exist in vulkan Buffer Map");
        return false;
    }

    VmaAllocator allocator = _context->allocator;

    auto& buffer = m_BuffersMap.at(bufferKeyHandle);
    vmaDestroyBuffer(allocator, CastObjectToVkObject<vk::Buffer>(bufferKeyHandle), buffer.allocation);

    m_BuffersMap.erase(bufferKeyHandle);

    return true;
}




void VK_NP::VulkanBufferMap::Init(VulkanContext* _vulkanContext)
{
    
}

void VK_NP::VulkanBufferMap::Destroy(VulkanContext* _vulkanContext)
{
    for (auto& pair : m_BuffersMap)
    {
        VmaAllocator allocator = _vulkanContext->allocator;
        vmaDestroyBuffer(allocator, CastObjectToVkObject<vk::Buffer>(pair.first), pair.second.allocation);
    }
}

void VK_NP::VulkanBufferMap::MapData(VulkanContext* _vulkanContext, BufferKeyHandle bufferKeyHandle, void** _data)
{
    if (!m_BuffersMap.contains(bufferKeyHandle))
    {
        throw std::runtime_error("Buffer does not exist in vulkan Buffer Map");
    }

    VmaAllocator allocator = _vulkanContext->allocator;
    auto& buffer = m_BuffersMap.at(bufferKeyHandle);
    vmaMapMemory(allocator, buffer.allocation, _data);
}

void VK_NP::VulkanBufferMap::UnMapData(VulkanContext* _vulkanContext, BufferKeyHandle bufferKeyHandle)
{
    if (!m_BuffersMap.contains(bufferKeyHandle))
    {
        throw std::runtime_error("Buffer does not exist in vulkan Buffer Map");
    }

    VmaAllocator allocator = _vulkanContext->allocator;
    auto& buffer = m_BuffersMap.at(bufferKeyHandle);
    vmaUnmapMemory(allocator, buffer.allocation);
}


vk::BufferUsageFlags VK_NP::VulkanBufferMap::GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage)
{
    using namespace PC_CORE;
    vk::BufferUsageFlags vkBufferUsageFlags = {};

    if (usage & BUFFER_USAGE_INDEX)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer; 
    }
    if (usage & BUFFER_USAGE_VERTEX)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer; 
    }

    if (usage & BUFFER_USAGE_UNIFORM)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
    }

    if (usage & BUFFER_USAGE_SHADER_STORAGE)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eStorageBuffer;
    }

    return vkBufferUsageFlags;
}

void VK_NP::VulkanBufferMap::CreateBufferAndAlloc(VulkanContext* _context, uint32_t _size, VkBufferUsageFlags _vkBufferUsageFlags,
VmaMemoryUsage vmaMemoryUsage,
    VmaAllocationCreateFlags vmaAllocationCreateFlagBits,  vk::Buffer* _buffer, VmaAllocation* _allocation, VmaAllocationInfo* _allocationInfo)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = _size;
    bufferCreateInfo.usage = _vkBufferUsageFlags;
    bufferCreateInfo.flags = 0;
    
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // Or VK_SHARING_MODE_CONCURRENT if needed

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = vmaMemoryUsage;
    allocationInfo.flags = vmaAllocationCreateFlagBits;

    VkBuffer vkBuffer = VK_NULL_HANDLE; 

    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(_context->allocator, &bufferCreateInfo, &allocationInfo, &vkBuffer, _allocation,
        _allocationInfo)));

    *_buffer = vkBuffer;
}

VK_NP::VulkanBufferMap::BufferKeyHandle VK_NP::VulkanBufferMap::CreateGPUBufferFromCPU(VulkanContext* _context, vk::CommandPool _commandPool, uint32_t _size,
    const void* _data, PC_CORE::GPU_BUFFER_USAGE usage)
{
    vk::Buffer stagingNuffer = VK_NULL_HANDLE;
    VmaAllocation vmaAllocationStaging = {};
    VmaAllocationInfo stagingBufferAllocationInfo = {};

    CreateBufferAndAlloc(_context, _size, static_cast<VkBufferUsageFlags>(GetVulkanUsage(usage)) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        &stagingNuffer, &vmaAllocationStaging, &stagingBufferAllocationInfo);

    void* mappedData = nullptr;
    vmaMapMemory(_context->allocator, vmaAllocationStaging, &mappedData);
    memcpy(mappedData, _data, _size);  // Copy data to the staging buffer

    vk::Buffer VertexBufferNuffer = VK_NULL_HANDLE;
    VmaAllocation vmaAllocationVertex = {};
    VmaAllocationInfo VertexBufferAllocationInfo = {};

    CreateBufferAndAlloc(_context, _size, static_cast<VkBufferUsageFlags>(GetVulkanUsage(usage)) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY,
        0,  // No flags for GPU-only memory
        &VertexBufferNuffer, &vmaAllocationVertex, &VertexBufferAllocationInfo);

    // Allocate and begin the command buffer
    vk::CommandBuffer commandBuffer = VK_NULL_HANDLE;
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    VK_CALL(_context->device.allocateCommandBuffers(&allocInfo, &commandBuffer));

    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    // Copy from staging to vertex buffer
    vk::BufferCopy copyRegion = {};
    copyRegion.size = _size;
    commandBuffer.copyBuffer(stagingNuffer, VertexBufferNuffer, 1, &copyRegion);

    commandBuffer.end();

    // Submit the command buffer and wait with a fence
    vk::SubmitInfo submitInfo = {};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk::FenceCreateInfo fenceCreateInfo = {};
    vk::Fence fence = _context->device.createFence(fenceCreateInfo);

    VK_CALL(_context->vkQueues.graphicQueue.submit(1, &submitInfo, fence));
    VK_CALL(_context->device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX));

    _context->device.destroyFence(fence);

    // Map the vertex buffer into your internal map
    BufferInternal BufferInternal =
    {
        .size = _size,
        .allocation = vmaAllocationVertex,
        .allocationInfo = VertexBufferAllocationInfo,
        .usage = usage
    };

    BufferKeyHandle BufferKeyHandle = *reinterpret_cast<VK_NP::VulkanBufferMap::BufferKeyHandle*>(&VertexBufferNuffer);
    m_BuffersMap.insert({BufferKeyHandle, BufferInternal});

    // Cleanup staging buffer and unmap
    vmaUnmapMemory(_context->allocator, vmaAllocationStaging);
    vmaDestroyBuffer(_context->allocator, stagingNuffer, vmaAllocationStaging);

    return BufferKeyHandle;
}

VK_NP::VulkanBufferMap::BufferKeyHandle VK_NP::VulkanBufferMap::CreateCpuBuffer(VulkanContext* _context,
    vk::CommandPool _commandPool, uint32_t _size, const void* data, PC_CORE::GPU_BUFFER_USAGE usage)
{

    return {};
}
