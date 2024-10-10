#include "back_end/vulkan_buffer_map.hpp"


VK_NP::VulkanBufferMap::BufferKeyHandle VK_NP::VulkanBufferMap::CreateBuffer(VulkanContext* _context, vk::CommandPool _commandPool,
    uint32_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE usage)
{
    VmaAllocator allocator = _context->allocator;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = _size;
    bufferInfo.usage = static_cast<VkBufferUsageFlags>(GetVulkanUsage(usage)) | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    // Add usage as needed
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Adjust sharing mode if needed

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    VmaAllocation allocation;
    VkBuffer bufferC = VK_NULL_HANDLE;

    
    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &bufferC, &allocation,
        nullptr)));

    VkMemoryPropertyFlags memPropFlags;
    vmaGetAllocationMemoryProperties(allocator, allocation, &memPropFlags);

    vk::CommandBuffer commandBuffer;
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = _commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = 1;
    VK_CALL(_context->device.allocateCommandBuffers(&commandBufferAllocateInfo, &commandBuffer));


    vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    
    commandBuffer.begin(commandBufferBeginInfo);

    if (memPropFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        // COPY USER DATA TO MAPPED DATA
        void* data;
        vmaMapMemory(allocator, allocation, &data);
        memcpy(data, _data, _size);
       //VK_CALL(static_cast<vk::Result>(vmaFlushAllocation(allocator, allocation, 0, VK_WHOLE_SIZE)));

        vk::BufferMemoryBarrier bufMemBarrier = {};
        bufMemBarrier.sType = vk::StructureType::eBufferMemoryBarrier;
        bufMemBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        bufMemBarrier.dstAccessMask = vk::AccessFlagBits::eUniformRead;
        bufMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier.buffer = bufferC;
        bufMemBarrier.offset = 0;
        bufMemBarrier.size = VK_WHOLE_SIZE;
        // TO DO CHANGE BARRIER FROM BUFFER TYPE
        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eVertexShader,
            static_cast<vk::DependencyFlagBits>(0),0,nullptr,1,&bufMemBarrier,0,nullptr);
        
    }
    else
    {
        VkBufferCreateInfo stagingBufCreateInfo  = {};
        stagingBufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        stagingBufCreateInfo.pNext = nullptr;
        stagingBufCreateInfo.size = _size;
        stagingBufCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Adjust sharing mode if needed

        // Add usage as needed
        stagingBufCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Adjust sharing mode if needed

        VmaAllocationCreateInfo stagingAllocCreateInfo = {};
        stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT;
   
        VkBuffer stagingBuf;
        VmaAllocation stagingAlloc;
        VmaAllocationInfo stagingAllocInfo;
        VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(allocator, &stagingBufCreateInfo, &stagingAllocCreateInfo,
            &stagingBuf, &stagingAlloc, &stagingAllocInfo)));

        void* data;
        vmaMapMemory(allocator, stagingAlloc, &data);
        memcpy(data, _data, _size);
        
        vk::BufferMemoryBarrier bufMemBarrier = {};
        bufMemBarrier.sType = vk::StructureType::eBufferMemoryBarrier;
        bufMemBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite; 
        bufMemBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead; 
        bufMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier.buffer = stagingBuf;
        bufMemBarrier.offset = 0;
        bufMemBarrier.size = VK_WHOLE_SIZE;

        // Use VK_PIPELINE_STAGE_TRANSFER_BIT for both src and dst stages
        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eHost, // srcStageMask
            vk::PipelineStageFlagBits::eTransfer, // dstStageMask
            vk::DependencyFlags(0),
            0, nullptr,
            1, &bufMemBarrier,
            0, nullptr
        );

        vk::BufferCopy buffCopy = {
            0, // srcOffset
            0, // dstOffset,
            _size, // size
        };

        commandBuffer.copyBuffer(stagingBuf, bufferC, 1, &buffCopy);

        vk::BufferMemoryBarrier bufMemBarrier2  = {};
        bufMemBarrier2.sType = vk::StructureType::eBufferMemoryBarrier;
        bufMemBarrier2.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        bufMemBarrier2.dstAccessMask = vk::AccessFlagBits::eUniformRead;
        bufMemBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufMemBarrier2.buffer = bufferC;
        bufMemBarrier2.offset = 0;
        bufMemBarrier2.size = VK_WHOLE_SIZE;

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eVertexInput,
        static_cast<vk::DependencyFlagBits>(0),0,nullptr,1,&bufMemBarrier2,0,nullptr);
    }
         
    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VK_CALL(_context->vkQueues.transferQueu.submit( 1, &submitInfo, VK_NULL_HANDLE));
    _context->device.waitIdle();
    _context->device.freeCommandBuffers(_commandPool, commandBuffer);

    
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

    if (usage & GPU_BUFFER_USAGE::INDEX)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer; 
    }
    if (usage & GPU_BUFFER_USAGE::VERTEX)
    {
        vkBufferUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer; 
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
