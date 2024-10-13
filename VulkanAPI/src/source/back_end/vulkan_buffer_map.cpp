#include "back_end/vulkan_buffer_map.hpp"


VK_NP::VulkanBufferMap::BufferKeyHandle VK_NP::VulkanBufferMap::CreateBuffer(VulkanContext* _context, vk::CommandPool _commandPool,
    uint32_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE usage)
{
    vk::Device device = _context->device;

    vk::Buffer buffer = VK_NULL_HANDLE;
    VmaAllocationInfo allocationInfo = {};
    VmaAllocation vmaAllocation = {};
    CreateBufferAndAlloc(_context, _size, _data, static_cast<VkBufferUsageFlags>(GetVulkanUsage(usage)) | VK_BUFFER_USAGE_TRANSFER_DST_BIT, &buffer, &vmaAllocation, &allocationInfo);


    void* mappedData = nullptr;
    vmaMapMemory(_context->allocator, vmaAllocation, &mappedData);
    memcpy(mappedData, _data, _size);

    BufferInternal BufferInternal =
        {
           .size = _size,
           .allocation = vmaAllocation,
           .allocationInfo = allocationInfo,
           .usage = usage
        };

    BufferKeyHandle BufferKeyHandle = *reinterpret_cast<VK_NP::VulkanBufferMap::BufferKeyHandle*>(&buffer);
    m_BuffersMap.insert({BufferKeyHandle, BufferInternal});
    return BufferKeyHandle;
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
    vmaDestroyBuffer(allocator, CastObjectToVkObject<vk::Buffer>(bufferKeyHandle), nullptr);

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
        DestroyBuffer(_vulkanContext, pair.first);
    }
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

void VK_NP::VulkanBufferMap::CreateBufferAndAlloc(VulkanContext* _context, uint32_t _size,
    const void* data, VkBufferUsageFlags _vkBufferUsageFlags, vk::Buffer* _buffer, VmaAllocation* _allocation, VmaAllocationInfo* _allocationInfo)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = _size;
    bufferCreateInfo.usage = _vkBufferUsageFlags;
    bufferCreateInfo.flags = 0;
    
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // Or VK_SHARING_MODE_CONCURRENT if needed

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                           VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer vkBuffer = VK_NULL_HANDLE; 

    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(_context->allocator, &bufferCreateInfo, &allocationInfo, &vkBuffer, _allocation,
        _allocationInfo)));

    *_buffer = vkBuffer;
}
