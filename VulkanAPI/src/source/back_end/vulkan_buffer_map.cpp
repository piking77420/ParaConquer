#include "back_end/vulkan_buffer_map.hpp"


VK_NP::VulkanBufferMap::BufferKeyHandle VK_NP::VulkanBufferMap::CreateBuffer(
    uint32_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE usage)
{
    VulkanContext* context = VulkanContext::currentContext;
    VmaAllocator allocator = context->allocator;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = _size;
    bufferInfo.usage = static_cast<VkBufferUsageFlags>(GetVulkanUsage(usage)) | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    // Add usage as needed
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Adjust sharing mode if needed

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    VmaAllocation allocation;
    VkBuffer bufferC = VK_NULL_HANDLE;

    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &bufferC, &allocation,
        nullptr)));

    VkMemoryPropertyFlags memPropFlags;
    vmaGetAllocationMemoryProperties(allocator, allocation, &memPropFlags);

    if (memPropFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        // COPY USER DATA TO MAPPED DATA
        void* data;
        vmaMapMemory(allocator, allocation, &data);
        memcpy(data, _data, _size);
        vmaUnmapMemory(allocator, allocation);
        BufferInternal bufferInternal =
        {
            .size = _size,
            .allocation = allocation,
            .usage = usage
        };
        vk::Buffer buffer = bufferC;
        m_BuffersMap.insert({buffer, bufferInternal});

        return buffer;
    }
    else
    {
        vk::CommandBuffer commandBuffer;
        
        
    }
}

const VK_NP::BufferInternal& VK_NP::VulkanBufferMap::GetBufferUsage(BufferKeyHandle _bufferKeyHandle)
{
    if (!m_BuffersMap.contains(_bufferKeyHandle))
        throw std::runtime_error("Buffer does not exist in vulkan Buffer Map");

    const BufferInternal* bufferInternal = &m_BuffersMap.at(_bufferKeyHandle);

    return *bufferInternal;
}


bool VK_NP::VulkanBufferMap::DestroyBuffer(BufferKeyHandle bufferKeyHandle)
{
    if (!m_BuffersMap.contains(bufferKeyHandle))
    {
        throw std::runtime_error("Buffer does not exist in vulkan Buffer Map");
        return false;
    }

    VulkanContext* context = VulkanContext::currentContext;
    VmaAllocator allocator = context->allocator;

    auto& buffer = m_BuffersMap.at(bufferKeyHandle);
    vmaDestroyBuffer(allocator, CastObjectToVkObject<vk::Buffer>(bufferKeyHandle), nullptr);

    m_BuffersMap.erase(bufferKeyHandle);

    return true;
}


VK_NP::VulkanBufferMap::~VulkanBufferMap()
{
    for (auto& pair : m_BuffersMap)
    {
        DestroyBuffer(pair.first);
    }
}


void VK_NP::VulkanBufferMap::CopyDataToBuffer(BufferInternal* _bufferInternal, const void* _data)
{
    /*
    VulkanContext* context = VulkanContext::currentContext;
    VmaAllocator allocator = context->allocator;
     
    VkMemoryPropertyFlags memPropFlags;
    vmaGetAllocationMemoryProperties(allocator, _bufferInternal->allocation, &memPropFlags);

    if (memPropFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        // Map the memory write into hit
        void* data;
        vmaMapMemory(allocator, _bufferInternal->allocation, &data);

        memcpy(data, _data, _bufferInternal->size);

        vmaUnmapMemory(allocator, _bufferInternal->allocation);
        
    }
    else
    {
        VkBufferCreateInfo stagingBufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        stagingBufCreateInfo.size = _bufferInternal->size;
        stagingBufCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        VmaAllocationCreateInfo stagingAllocCreateInfo = {};
        stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBuffer stagingBuf;
        VmaAllocation stagingAlloc;
        VmaAllocationInfo stagingAllocInfo;
        VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(allocator, &stagingBufCreateInfo, &stagingAllocCreateInfo, &stagingBuf,
                &stagingAlloc, &stagingAllocInfo)));

        memcpy(stagingAllocInfo.pMappedData, _data, _bufferInternal->size);
        VK_CALL(static_cast<vk::Result>(vmaFlushAllocation(allocator, stagingAlloc, 0, _bufferInternal->size)));

        VkBufferMemoryBarrier bufMemBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
        bufMemBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        bufMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        bufMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier.buffer = stagingBuf;
        bufMemBarrier.offset = 0;
        bufMemBarrier.size = VK_WHOLE_SIZE;

        vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 1, &bufMemBarrier, 0, nullptr);
 
        VkBufferCopy bufCopy =
        {
            0, // srcOffset
            0, // dstOffset,
            myDataSize, // size
        };

        vkCmdCopyBuffer(cmdBuf, stagingBuf, buf, 1, &bufCopy);
 
        vk::BufferMemoryBarrier bufMemBarrier2;
        bufMemBarrier2.sType = vk::StructureType::eBufferMemoryBarrier;
        bufMemBarrier2.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        bufMemBarrier2.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT; // We created a uniform buffer
        bufMemBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier2.buffer = buf;
        bufMemBarrier2.offset = 0;
        bufMemBarrier2.size = VK_WHOLE_SIZE;
 
        vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            0, 0, nullptr, 1, &bufMemBarrier2, 0, nullptr);
    }
    */
}

vk::BufferUsageFlags VK_NP::VulkanBufferMap::GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage)
{
    using namespace PC_CORE;
    vk::BufferUsageFlags vkBufferUsageFlags = {};

    if (usage & GPU_BUFFER_USAGE::INDEX)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer; // Now you can use |=
    }
    if (usage & GPU_BUFFER_USAGE::VERTEX)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer; // Add more usage flags if needed
    }

    if (usage & GPU_BUFFER_USAGE::UNIFORM)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
    }

    if (usage & GPU_BUFFER_USAGE::SHADER_STORAGE)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eStorageBuffer;
    }

    return vkBufferUsageFlags;
}
