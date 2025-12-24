#include "VulkanBuffer.hpp"

#include "LowRenderer/Rhi.hpp"
#include "VulkanCommandList.hpp"

#include "Utils/HelperFunctions.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "Utils/VmaHelper.hpp"


VmaAllocationCreateFlags VmaAllocationCreateFlagsFromBuffer(PC_CORE::RhiResource::MemoryUsage _memoryUsage)
{
    VmaAllocationCreateFlags flag = 0;
    
    if (_memoryUsage == PC_CORE::RhiResource::MemoryUsage::Dynamic)
        flag |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; 
    
    // VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT for raytring
    
    return flag;
}


VmaAllocationCreateInfo VmaAllocationCreateInfoFromBuffer(PC_CORE::RhiResource::MemoryUsage _memoryUsage)
{
    using Musage = PC_CORE::RhiResource::MemoryUsage;
    
    assert(_memoryUsage != Musage::None || _memoryUsage != Musage::Count);
    
    VmaAllocationCreateInfo allocCI = 
    {
        .flags = VmaAllocationCreateFlagsFromBuffer(_memoryUsage),
        .usage = (_memoryUsage == PC_CORE::RhiResource::MemoryUsage::Dynamic) ? VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE, // not mandatory see header
        .pUserData = nullptr, // not mandatory see header
        .priority = {} // VmaAllocator is not create with VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT so ignored
    };
    
    
    return allocCI;
}


Vulkan::VulkanBuffer::VulkanBuffer(PC_CORE::Rhi& _Rhi)
    : RhiBuffer(_Rhi)

{
}

Vulkan::VulkanBuffer::~VulkanBuffer()
{
    auto& context = GET_VK_CONTEXT;
    for (auto& alloc : m_Handles)
        FreeAlloc(context, alloc);
}

const void* Vulkan::VulkanBuffer::GetFrameNativeHandle(size_t _frameIndex) const
{
    return GetBufferAndAlloc(_frameIndex);
}

void* Vulkan::VulkanBuffer::GetFrameNativeHandle(size_t _frameIndex)
{
    return GetBufferAndAlloc(_frameIndex);
}

bool Vulkan::VulkanBuffer::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    
    assert(m_MemoryUsage != RhiResource::MemoryUsage::Count && m_MemoryUsage != RhiResource::MemoryUsage::None);

    const uint32_t nbrOfHandle = GetNbrOfHandle(m_MemoryUsage);
    
    if (nbrOfHandle == 0)
    {
        PC_LOGERROR("VulkanBuffer::GetNbrOfHandle() m_Handles.empty()");
        return false;
    }
    m_Handles.resize(nbrOfHandle);

    vk::BufferCreateInfo bufferCreate{};
    bufferCreate.sType = vk::StructureType::eBufferCreateInfo;
    bufferCreate.size = m_SizeInByte;
    bufferCreate.usage = Utils::RhiBufferUsageToVulkan(m_Usage);
    bufferCreate.sharingMode = vk::SharingMode::eExclusive;
    
    if (m_MemoryUsage != RhiResource::MemoryUsage::Dynamic)
    {
        bufferCreate.usage |= vk::BufferUsageFlagBits::eTransferDst;
    }
    
    
    auto& context = GET_VK_CONTEXT;
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->GetDevice();
    auto instance = context.GetInstance();
    
    VmaAllocationCreateInfo aCreateInfo = VmaAllocationCreateInfoFromBuffer(m_MemoryUsage);
    
    VmaAllocationInfo VmaAllocationInfo;
    VmaAllocationInfo.pName = GetName().data();
    
    for (size_t i = 0; i < nbrOfHandle; i++)
    {
         VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(context.allocator, reinterpret_cast<VkBufferCreateInfo*>(&bufferCreate), 
                        &aCreateInfo, reinterpret_cast<VkBuffer*>(&m_Handles[i].buffer), &m_Handles[i].alloc, &VmaAllocationInfo)));
        
        vk::DebugUtilsObjectNameInfoEXT nameInfo;
        nameInfo.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
        nameInfo.pNext = nullptr;
        nameInfo.objectType = vk::ObjectType::eBuffer;
        nameInfo.objectHandle = reinterpret_cast<uint64_t>(
            static_cast<VkBuffer>(m_Handles[i].buffer)
            );       
        nameInfo.pObjectName = GetName().data();
        
        SET_VK_DEBUG_NAME(nameInfo);
    }
    
    
    return true;
    
}

void Vulkan::VulkanBuffer::UploadData(PC_CORE::CommandList* _commandList, const void* _data, size_t _sizeInBytes)
{
    assert(m_MemoryUsage != RhiResource::MemoryUsage::Dynamic && "You can only UploadData with static or streamable buffers");
    assert(_sizeInBytes <= m_SizeInByte);

    BufferAndAlloc stagingBuffer; // may store this in the class for only upload it when beign frame that allows uis to avoid calling waitdeviceIdle
    
    auto& context = GET_VK_CONTEXT;
    CreateStagingBufferForCopy(context, &stagingBuffer, _sizeInBytes);
    
    // Copy Data to stagingBuffer
    void* mappedData;
    vmaMapMemory(context.allocator, stagingBuffer.alloc, &mappedData);
    std::memcpy(mappedData, _data, _sizeInBytes);
    vmaUnmapMemory(context.allocator, stagingBuffer.alloc);

    
    vk::BufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = _sizeInBytes;

    for (size_t i = 0; i < m_Handles.size(); i++)
    {
        auto& buffer = m_Handles[i];
        GET_VK_COMMAND_BUFFER(_commandList, i);

        cmb.copyBuffer(stagingBuffer.buffer, buffer.buffer, copyRegion);
    }

    
    // Destroy the staging buffer no need it anymore
    FreeAlloc(context, stagingBuffer);
}

char* Vulkan::VulkanBuffer::BeginFullDynamicBufferUpdateForCurrentFrame()
{
    PERF_REGION_SCOPED;
    assert(m_MemoryUsage == RhiResource::MemoryUsage::Dynamic && "You can only dynamic update dynamic buffers");
    assert(m_CurrentFrameMappedData == nullptr && "Data Aldready Map or forgot to call EndFullDynamicBufferUpdateForCurrentFrame");
    
    const auto frameIndex = m_Rhi.GetFrameIndex();
    auto& context = GET_VK_CONTEXT;
    
    
    VK_CALL(static_cast<vk::Result>(vmaMapMemory(context.allocator,
        m_Handles[frameIndex].alloc, &m_CurrentFrameMappedData)));
    return static_cast<char*>(m_CurrentFrameMappedData);
}


void Vulkan::VulkanBuffer::EndFullDynamicBufferUpdateForCurrentFrame()
{
    PERF_REGION_SCOPED
    assert(m_MemoryUsage == RhiResource::MemoryUsage::Dynamic && "You can only dynamic update dynamic buffers");
    assert(m_CurrentFrameMappedData != nullptr && "Data not Map or forgot to call BeginFullDynamicBufferUpdateForCurrentFrame");

    const auto frameIndex = m_Rhi.GetFrameIndex();
    auto& context = GET_VK_CONTEXT;
    
    vmaUnmapMemory(context.allocator,
        m_Handles[frameIndex].alloc);
    m_CurrentFrameMappedData = nullptr;
}

void Vulkan::VulkanBuffer::CreateStagingBufferForCopy(VulkanContext& _VkContext, BufferAndAlloc* bufferAndAlloc, size_t _sizeInBytes)
{
    
    vk::BufferCreateInfo bufferCreate{};
    bufferCreate.sType = vk::StructureType::eBufferCreateInfo;
    bufferCreate.size = vk::DeviceSize{_sizeInBytes};
    bufferCreate.usage = vk::BufferUsageFlagBits::eTransferSrc;
    bufferCreate.sharingMode = vk::SharingMode::eExclusive;
    
    VmaAllocationInfo VmaAllocationInfo;
    VmaAllocationInfo.pName = "StagingBuffer";
    
    VmaAllocationCreateInfo aCreateInfo{};
    aCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    aCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    
    const vk::Device device = _VkContext.GetDevice()->GetDevice();

    vmaCreateBuffer(_VkContext.allocator, reinterpret_cast<VkBufferCreateInfo*>(&bufferCreate),
                   &aCreateInfo, reinterpret_cast<VkBuffer*>(bufferAndAlloc), &bufferAndAlloc->alloc, &VmaAllocationInfo);
    
    vk::DebugUtilsObjectNameInfoEXT nameInfo;
    nameInfo.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
    nameInfo.pNext = nullptr;
    nameInfo.objectType = vk::ObjectType::eBuffer;
    nameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkBuffer>(bufferAndAlloc->buffer));
    nameInfo.pObjectName = "StagingBuffer";
    
    _VkContext.GetInstance()->SetDebugName(device, &nameInfo);
}

void Vulkan::VulkanBuffer::FreeAlloc(VulkanContext& _VkContext, BufferAndAlloc& _handle)
{
    if (_handle.buffer == VK_NULL_HANDLE || _handle.alloc == VK_NULL_HANDLE)
        return;
    
    vmaDestroyBuffer(_VkContext.allocator, _handle.buffer, _handle.alloc);
    _handle.buffer = VK_NULL_HANDLE;
    _handle.alloc = VK_NULL_HANDLE;
}

const Vulkan::BufferAndAlloc* Vulkan::VulkanBuffer::GetBufferAndAlloc(size_t _frameIndex) const
{
    if (m_Handles.empty())
    {
        PC_LOGERROR("VulkanBuffer::GetFrameNativeHandle() m_Handles.empty()");
        return nullptr;
    }
    
    const size_t handleIndex = std::min(m_Handles.size() - 1, _frameIndex);
    return &m_Handles[handleIndex];
}

Vulkan::BufferAndAlloc* Vulkan::VulkanBuffer::GetBufferAndAlloc(size_t _frameIndex)
{
    if (m_Handles.empty())
    {
        PC_LOGERROR("VulkanBuffer::GetFrameNativeHandle() m_Handles.empty()");
        return nullptr;
    }
    
    const size_t handleIndex = std::min(m_Handles.size() - 1, _frameIndex);
    return &m_Handles[handleIndex];
}
