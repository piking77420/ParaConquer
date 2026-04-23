#include "VulkanBuffer.hpp"

#include "LowRenderer/Rhi.hpp"
#include "VulkanCommandList.hpp"

#include "Utils/HelperFunctions.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "Utils/VmaHelper.hpp"


VmaAllocationCreateFlags VmaAllocationCreateFlagsFromBuffer(PC_CORE::RhiResource::MemoryUsage _memoryUsage)
{
    VmaAllocationCreateFlags flag = 0;
    
    if (_memoryUsage == PC_CORE::RhiResource::MemoryUsage::CPUVisible)
        flag |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; 
    
    // VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT for raytring
    
    return flag;
}


VmaAllocationCreateInfo VmaAllocationCreateInfoFromBuffer(PC_CORE::RhiResource::MemoryUsage _memoryUsage)
{
    using Musage = PC_CORE::RhiResource::MemoryUsage;
    
    
    VmaAllocationCreateInfo allocCI = 
    {
        .flags = VmaAllocationCreateFlagsFromBuffer(_memoryUsage),
        .usage = (_memoryUsage == PC_CORE::RhiResource::MemoryUsage::CPUVisible) ? VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
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

    for (size_t i = 0; i < m_StagingBuffers.size(); i++)
        context.DefferdDestroy(m_StagingBuffers[i], static_cast<uint32_t>(i));
    
    for (size_t i = 0; i < m_Handle.size(); i++)
        context.DefferdDestroy(m_Handle[i], static_cast<uint32_t>(i));
}

bool Vulkan::VulkanBuffer::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    RhiBuffer::Build();
  
    const uint32_t nbrOfHandle = m_NbrOfBackendObject;
    
    if (nbrOfHandle == 0)
    {
        PC_LOGERROR("VulkanBuffer::GetNbrOfHandle() m_Handles.empty()");
        return false;
    }
    m_Handle.resize(nbrOfHandle);
    if (m_MemoryUsage != MemoryUsage::CPUVisible)
    {
        m_StagingBuffers.resize(m_Handle.size());
        m_StaginBuffersSizes.resize(m_Handle.size());
    }
    m_CurrentFrameMappedData.resize(m_Handle.size());

    vk::BufferCreateInfo bufferCreate{};
    bufferCreate.sType = vk::StructureType::eBufferCreateInfo;
    bufferCreate.usage = Utils::RhiBufferUsageToVulkan(m_Usage);
    bufferCreate.sharingMode = vk::SharingMode::eExclusive;
    
    switch (m_BufferBackingStrategy)
    {
    case PC_CORE::RhiBuffer::SingleBuffer:
        bufferCreate.usage |= vk::BufferUsageFlagBits::eTransferDst;
        bufferCreate.size = m_SizeInByte;

        break;
    case PC_CORE::RhiBuffer::CpuVisibleRing:
        bufferCreate.size = m_SizeInByte * MaxFramesInFlight; // use stride to index correct one 
        break;
    case PC_CORE::RhiBuffer::PerFrameBuffers:
        if (m_MemoryUsage == MemoryUsage::StaticGPU)
            bufferCreate.usage |= vk::BufferUsageFlagBits::eTransferDst;
        bufferCreate.size = m_SizeInByte;
        break;
    case PC_CORE::RhiBuffer::StagedUpload:
        bufferCreate.usage |= vk::BufferUsageFlagBits::eTransferDst;
        bufferCreate.size = m_SizeInByte;
        break;
    case PC_CORE::RhiBuffer::Readback:
        bufferCreate.usage |= vk::BufferUsageFlagBits::eTransferDst;
        bufferCreate.size = m_SizeInByte;
        break;
    default:
        break;
    }

    VmaAllocationCreateInfo aCreateInfo = VmaAllocationCreateInfoFromBuffer(m_MemoryUsage);
    VmaAllocationInfo VmaAllocationInfo;
    VmaAllocationInfo.pName = GetName().data();
    

    auto& context = GET_VK_CONTEXT;
    std::scoped_lock _(context.lock);


    auto instance = context.GetInstance();
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->GetDevice();
    for (size_t i = 0; i < nbrOfHandle; i++)
    {

         VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(context.allocator, reinterpret_cast<VkBufferCreateInfo*>(&bufferCreate), 
                        &aCreateInfo, reinterpret_cast<VkBuffer*>(&m_Handle[i].buffer), &m_Handle[i].alloc, &VmaAllocationInfo)));
        
        vk::DebugUtilsObjectNameInfoEXT nameInfo;
        nameInfo.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
        nameInfo.pNext = nullptr;
        nameInfo.objectType = vk::ObjectType::eBuffer;
        nameInfo.objectHandle = reinterpret_cast<uint64_t>(
            static_cast<VkBuffer>(m_Handle[i].buffer)
            );       
        nameInfo.pObjectName = GetName().data();
        
        SET_VK_DEBUG_NAME(nameInfo);
    }

    /*
    if (m_BufferBackingStrategy == PC_CORE::RhiBuffer::CpuVisibleRing)
    {
        for (size_t i = 0; i < m_CurrentFrameMappedData.size(); i++)
        {
            VK_CALL(static_cast<vk::Result>(vmaMapMemory(context.allocator,
                m_Handle[i].alloc, &m_CurrentFrameMappedData[i])));
        }
    }*/
    
    
    return true;
    
}

bool Vulkan::VulkanBuffer::UploadData(PC_CORE::CommandList* _commandList, const void* _data, size_t _sizeInBytes)
{
    if (_sizeInBytes == 0)
        return false;

    assert(
        m_MemoryUsage == RhiResource::MemoryUsage::StaticGPU &&
        "UploadData is only valid for GPU-only buffers (staged upload)"
    );
    assert(
        m_BufferBackingStrategy == BufferBackingStrategy::StagedUpload ||
        m_BufferBackingStrategy == BufferBackingStrategy::SingleBuffer &&
        "UploadData is only valid for staged GPU buffers"
    );    
    assert(_sizeInBytes <= m_SizeInByte);

    if (_sizeInBytes > m_SizeInByte)
        return false;

    auto& context = GET_VK_CONTEXT;
    const size_t FrameIndex = m_Rhi.GetFrameIndex();

    auto& stagingBuffer = *GetVkStagingBuffer(FrameIndex);

    if (stagingBuffer.buffer == VK_NULL_HANDLE || m_StaginBuffersSizes[std::distance(&m_StagingBuffers[0], &stagingBuffer)] != _sizeInBytes)
    {
        if (stagingBuffer.buffer)
        {
            context.DefferdDestroy(stagingBuffer, FrameIndex);
        }
        CreateStagingBufferForCopy(context, &stagingBuffer, _sizeInBytes, m_Name.c_str());
        m_StaginBuffersSizes[std::distance(&m_StagingBuffers[0], &stagingBuffer)] = _sizeInBytes;
    }
    
    // Copy Data to stagingBuffer
    void* mappedData;
    vmaMapMemory(context.allocator, stagingBuffer.alloc, &mappedData);
    std::memcpy(mappedData, _data, _sizeInBytes);
    vmaUnmapMemory(context.allocator, stagingBuffer.alloc);
    
    vk::BufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = _sizeInBytes;


    BufferAndAlloc& buffer = *GetBufferAndAlloc(FrameIndex);
    GET_VK_COMMAND_BUFFER(_commandList, FrameIndex);
    
    
    cmb.copyBuffer(stagingBuffer.buffer, buffer.buffer, copyRegion);

    return true;
}

char* Vulkan::VulkanBuffer::BeginFullDynamicBufferUpdateForCurrentFrame()
{
    PERF_REGION_SCOPED;
    assert(m_MemoryUsage == RhiResource::MemoryUsage::CPUVisible && "You can only dynamic update dynamic buffers");

    const size_t frameIndex = static_cast<uint32_t>(m_Rhi.GetFrameIndex());
    const size_t index = std::min(m_Handle.size() - 1, frameIndex);

    auto& context = GET_VK_CONTEXT;
    assert(m_CurrentFrameMappedData[index] == nullptr && "Data Aldready Map or forgot to call EndFullDynamicBufferUpdateForCurrentFrame");

    {
        std::scoped_lock _(context.lock);
        VK_CALL(static_cast<vk::Result>(vmaMapMemory(context.allocator,
            m_Handle[index].alloc, &m_CurrentFrameMappedData[index])));
    }
    
    return static_cast<char*>(m_CurrentFrameMappedData[index]);
}

char* Vulkan::VulkanBuffer::BeginBufferUpdateForCurrentFrame()
{
    PERF_REGION_SCOPED;
    assert(m_MemoryUsage == RhiResource::MemoryUsage::CPUVisible && "You can only dynamic update dynamic buffers");

    const size_t frameIndex = static_cast<uint32_t>(m_Rhi.GetFrameIndex());
    const size_t index = std::min(m_Handle.size() - 1, frameIndex);

    auto& context = GET_VK_CONTEXT;
    assert(m_CurrentFrameMappedData[index] == nullptr && "Data Aldready Map or forgot to call EndFullDynamicBufferUpdateForCurrentFrame");


    {
        std::scoped_lock _(context.lock);
        VK_CALL(static_cast<vk::Result>(vmaMapMemory(context.allocator,
            m_Handle[index].alloc, &m_CurrentFrameMappedData[index])));
    }

    return static_cast<char*>(m_CurrentFrameMappedData[index]);
}



void Vulkan::VulkanBuffer::EndBufferUpdate()
{
    PERF_REGION_SCOPED
    assert(m_MemoryUsage == RhiResource::MemoryUsage::CPUVisible && "You can only dynamic update dynamic buffers");

    if (m_BufferBackingStrategy != PC_CORE::RhiBuffer::CpuVisibleRing &&
        m_BufferBackingStrategy != PC_CORE::RhiBuffer::PerFrameBuffers)
    {
        return;
    }

    const size_t frameIndex = static_cast<uint32_t>(m_Rhi.GetFrameIndex());
    const size_t index = std::min(m_Handle.size() - 1, frameIndex);

    assert(m_CurrentFrameMappedData[index] != nullptr && "Data not Map or forgot to call BeginFullDynamicBufferUpdateForCurrentFrame");

    auto& context = GET_VK_CONTEXT;
    
    {
        std::scoped_lock _(context.lock);
        vmaUnmapMemory(context.allocator,
            m_Handle[index].alloc);
    }
    m_CurrentFrameMappedData[index] = nullptr;
}

void Vulkan::VulkanBuffer::CreateStagingBufferForCopy(VulkanContext& _VkContext, BufferAndAlloc* bufferAndAlloc, size_t _sizeInBytes, const char* BufferName) // TODO MAKE AN HELPER CLASS 
{
    
    vk::BufferCreateInfo bufferCreate{};
    bufferCreate.sType = vk::StructureType::eBufferCreateInfo;
    bufferCreate.size = vk::DeviceSize{_sizeInBytes};
    bufferCreate.usage = vk::BufferUsageFlagBits::eTransferSrc;
    bufferCreate.sharingMode = vk::SharingMode::eExclusive;

    
    VmaAllocationCreateInfo aCreateInfo{};
    aCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    aCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    
    const vk::Device device = _VkContext.GetDevice()->GetDevice();

    VK_CALL(static_cast<vk::Result>(vmaCreateBuffer(_VkContext.allocator, reinterpret_cast<VkBufferCreateInfo*>(&bufferCreate),
                   &aCreateInfo, reinterpret_cast<VkBuffer*>(bufferAndAlloc), &bufferAndAlloc->alloc, nullptr)));

    const std::string StagingBufferName = (std::string(BufferName) + " Staging");
    
    vk::DebugUtilsObjectNameInfoEXT nameInfo;
    nameInfo.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
    nameInfo.pNext = nullptr;
    nameInfo.objectType = vk::ObjectType::eBuffer;
    nameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkBuffer>(bufferAndAlloc->buffer));
    nameInfo.pObjectName = StagingBufferName.c_str();
    vmaSetAllocationName(_VkContext.allocator, bufferAndAlloc->alloc, StagingBufferName.c_str());
#ifdef  DEBUG_GPU_ON
    _VkContext.GetInstance()->SetDebugName(device, &nameInfo);
#endif
}

const Vulkan::BufferAndAlloc* Vulkan::VulkanBuffer::GetBufferAndAlloc(size_t _frameIndex) const
{
    return GetVkAlloc(_frameIndex);
}

Vulkan::BufferAndAlloc* Vulkan::VulkanBuffer::GetBufferAndAlloc(size_t _frameIndex)
{
    return GetVkAlloc(_frameIndex);
}

Vulkan::BufferAndAlloc* Vulkan::VulkanBuffer::GetVkAlloc(size_t _frameIndex)
{
    if (m_Handle.empty())
    {
        PC_LOGERROR("VulkanBuffer::GetFrameNativeHandle() m_Handles.empty()");
        return nullptr;
    }

    const size_t handleIndex = std::min(m_Handle.size() - 1, _frameIndex);
    return &m_Handle[handleIndex];
}

const Vulkan::BufferAndAlloc* Vulkan::VulkanBuffer::GetVkStagingBuffer(size_t _frameIndex) const
{
    if (m_StagingBuffers.empty())
    {
        PC_LOGERROR("VulkanBuffer::GetFrameNativeHandle() m_StagingBuffer.empty()");
        return nullptr;
    }

    const size_t handleIndex = std::min(m_StagingBuffers.size() - 1, _frameIndex);
    return &m_StagingBuffers[handleIndex];
}

Vulkan::BufferAndAlloc* Vulkan::VulkanBuffer::GetVkStagingBuffer(size_t _frameIndex)
{
    if (m_StagingBuffers.empty())
    {
        PC_LOGERROR("VulkanBuffer::GetFrameNativeHandle() m_StagingBuffer.empty()");
        return nullptr;
    }

    const size_t handleIndex = std::min(m_StagingBuffers.size() - 1, _frameIndex);
    return &m_StagingBuffers[handleIndex];
}

const Vulkan::BufferAndAlloc* Vulkan::VulkanBuffer::GetVkAlloc(size_t _frameIndex) const
{
    if (m_Handle.empty())
    {
        PC_LOGERROR("VulkanBuffer::GetFrameNativeHandle() m_Handles.empty()");
        return nullptr;
    }

    const size_t handleIndex = std::min(m_Handle.size() - 1, _frameIndex);
    return &m_Handle[handleIndex];
}
