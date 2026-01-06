#include <Memory>

#include "LowRenderer/Rhi.hpp"
#include "VulkanContext.hpp"
#include "VulkanTexture.hpp"
#include "VulkanCommandList.hpp"

#include "Utils/HelperFunctions.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "Utils/TransitionImageLayout.hpp"
#include "Utils/VulkanImageHelper.hpp"


Vulkan::VulkanTexture::VulkanTexture(PC_CORE::Rhi& _Rhi)
    : RhiTexture(_Rhi)
{

}


bool Vulkan::VulkanTexture::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    
    if (GetWidth() == 0 || GetHeight() == 0)
    {
        PC_LOGERROR("VulkanTexture::Create() m_Width = 0, m_Height = 0");
        return false;
    }
   
    const size_t nbrOfObjectHandle = GetNbrOfHandle(m_MemoryUsage);
    if (nbrOfObjectHandle == 0)
    {
        PC_LOGERROR("VulkanTexture::Create() nbrOfObjectHandle = 0");
        return false;
    }
    m_Handles.resize(nbrOfObjectHandle);

    
    auto& context = GET_VK_CONTEXT;
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->GetDevice();
    std::shared_ptr<VulkanInstance> instance = context.GetInstance();

    VkImageAspectFlags = Utils::RhiTextureFormatToImageAspectFlagFlags(GetRhiFormat());
    VkFormat = Utils::RhiFormatToVkFormat(GetRhiFormat());
    
    vk::ImageCreateInfo imageInfo{};
    imageInfo.sType = vk::StructureType::eImageCreateInfo;
    imageInfo.imageType = Utils::RhiImageToVkImageType(GetTextureType());
    imageInfo.extent.width = GetWidth();
    imageInfo.extent.height = GetHeight();
    imageInfo.extent.depth = GetDepth();
    imageInfo.mipLevels = GetLevel();
    imageInfo.arrayLayers = GetLayer();
    imageInfo.format = Utils::RhiFormatToVkFormat(GetRhiFormat());
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = Utils::GetImageUsageFlags(GetTextureUsage());
    imageInfo.samples = Utils::RhSampleCountToVulkan(GetSamples());
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.flags = Utils::ImageCreateFlagFromTextureType(GetTextureType());
 
    for (size_t i = 0; i < nbrOfObjectHandle; i++)
    {
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = m_MemoryUsage == RhiResource::MemoryUsage::Dynamic ? VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE : VMA_MEMORY_USAGE_GPU_ONLY;
        
        VmaAllocationInfo VmaAllocationInfo;
        VmaAllocationInfo.pName = GetName().data();
        VK_CALL(static_cast<vk::Result>(vmaCreateImage(context.allocator, reinterpret_cast<VkImageCreateInfo*>(&imageInfo), &allocInfo, 
            reinterpret_cast<VkImage*>(&m_Handles[i].Image), &m_Handles[i].Allocation, &VmaAllocationInfo)));
        
        vk::DebugUtilsObjectNameInfoEXT nameInfoImage;
        nameInfoImage.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
        nameInfoImage .pNext = nullptr;
        nameInfoImage.objectType = vk::ObjectType::eImage;
        nameInfoImage.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImage>(m_Handles[i].Image));
        nameInfoImage.pObjectName = GetName().data();
        
        
        SET_VK_DEBUG_NAME(nameInfoImage);
        
        vk::ImageViewCreateInfo imageviewInfo{};
        imageviewInfo.sType = vk::StructureType::eImageViewCreateInfo;
        imageviewInfo.image = m_Handles[i].Image;
        imageviewInfo.viewType = Utils::RhiImageToVkImageViewType(GetTextureType());
        imageviewInfo.format = imageInfo.format;
        imageviewInfo.subresourceRange.aspectMask = VkImageAspectFlags;
        imageviewInfo.subresourceRange.baseMipLevel = 0;
        imageviewInfo.subresourceRange.baseArrayLayer = 0;
        imageviewInfo.subresourceRange.levelCount = GetLevel();
        imageviewInfo.subresourceRange.layerCount = GetLayer();
        
        VK_CALL(device.createImageView(&imageviewInfo, nullptr, &m_Handles[i].ImageView));
        
        vk::DebugUtilsObjectNameInfoEXT nameInfoImageView;
        nameInfoImageView.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
        nameInfoImageView.pNext = nullptr;
        nameInfoImageView.objectType = vk::ObjectType::eImageView;
        nameInfoImageView.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImageView>(m_Handles[i].ImageView));
        nameInfoImageView.pObjectName = GetName().data();
        
        SET_VK_DEBUG_NAME(nameInfoImageView); 
    }


    if (m_TextureUsage & TextureUsageFlagBits::DepthStencil || m_TextureUsage & TextureUsageFlagBits::RenderTarget) // hardcoded
    {
        const Utils::SingleCommandBeginInfo singleCommandBeginInfo =
        {
            .device = device,
            .commandPool = context.transferCommandPool,
            .queue = context.mainQueue
        };

        vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

        for (size_t i = 0; i < m_Handles.size(); i++)
        {
            TextureAndAlloc& handle = m_Handles[i];
            TransitionImageLayout(commandBuffer,
                handle.Image,
                VkFormat,
                vk::ImageLayout::eUndefined,
                m_TextureUsage & TextureUsageFlagBits::DepthStencil ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eColorAttachmentOptimal,
                VkImageAspectFlags,
                m_Layer,
                m_Level);

        }
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);
    }
   

    return true;
}

bool Vulkan::VulkanTexture::UploadData2D(PC_CORE::CommandList* _CommandList, const void* _Data, PC_CORE::RhiFormat _Format, uint32_t _ImageWidht, uint32_t _ImageHeight)
{
    assert(_CommandList != nullptr);

    if (GetTextureType() != Type::Texture2D)
    {
        PC_LOGERROR("UploadData2D should only be used for TextureType::Texture2D");
        return false;
    }
    
    auto& context = GET_VK_CONTEXT;
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice)->GetDevice();
    const int multiplayer = PC_CORE::GetBytePerPixel(_Format);
    const size_t imageSize = static_cast<size_t>(_ImageWidht * _ImageHeight * multiplayer);

    if (m_StagingBuffer.buffer != VK_NULL_HANDLE)
    {
        VulkanBuffer::FreeAlloc(context, m_StagingBuffer);
    }
    VulkanBuffer::CreateStagingBufferForCopy(context, &m_StagingBuffer, imageSize);

    void* mappedData;
    vmaMapMemory(context.allocator, m_StagingBuffer.alloc, &mappedData);
    assert(mappedData != nullptr);
    std::memcpy(mappedData, _Data, imageSize);
    vmaUnmapMemory(context.allocator, m_StagingBuffer.alloc);


    const size_t FrameIndex = m_Rhi.GetFrameIndex();
    GET_VK_COMMAND_BUFFER(_CommandList, FrameIndex);

    TextureAndAlloc& handle = *GetTextureAndAlloc(FrameIndex);

    vk::BufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VkImageAspectFlags;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = VkOffset3D{ 0, 0, 0 };

    region.imageExtent = vk::Extent3D{
        _ImageWidht,
        _ImageHeight,
        1
    };
        
    cmb.copyBufferToImage(
            m_StagingBuffer.buffer,
            handle.Image,
            vk::ImageLayout::eTransferDstOptimal,
            region
        );    

    return true;
    
}

void Vulkan::VulkanTexture::UploadDataLayer(PC_CORE::CommandList* commandList, const std::vector<void*>& _imageDatas, uint32_t _imageWidht, uint32_t _imageHeight, uint32_t _layerCount)
{
    /*
    if (m_TextureType != Type::TextureArray2D &&
        m_TextureType != Type::CubeMap &&
        m_TextureType != Type::CubeMapArray)
    {
        PC_LOGERROR("UploadDataLayer should only be used for 2D array/cubemap types");
        return;
    }
    
    if (_imageDatas.size() != _layerCount)
    {
        PC_LOGERROR("_imageDatas.size() != _layerCount");
        return;
    }

    
    auto& context = GET_VK_CONTEXT;
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice)->GetDevice();

    const int bytesPerPixel = PC_CORE::GetBytePerPixel(m_RhiFormat);
    const uint32_t sliceSize = _imageWidht * _imageHeight * bytesPerPixel;
    const uint32_t totalSize = sliceSize * _layerCount;

    
    BufferAndAlloc stagingBuffer;
    VulkanBuffer::CreateStagingBufferForCopy(context, &stagingBuffer, totalSize);

    void* mappedData;
    vmaMapMemory(context.allocator, stagingBuffer.alloc, &mappedData);
    for (uint32_t i = 0; i < _layerCount; i++)
        std::memcpy(static_cast<uint8_t*>(mappedData) + (sliceSize * i), _imageDatas[i], sliceSize);
    vmaUnmapMemory(context.allocator, stagingBuffer.alloc);

    const Utils::SingleCommandBeginInfo singleCommandBeginInfo =
    {
        .device = device,
        .commandPool = context.transferCommandPool,
        .queue = context.mainQueue
    };

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

    for (size_t i = 0; i < m_Handles.size(); i++)
    {
        TextureAndAlloc& handle = m_Handles[i];

        const vk::ImageLayout current = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(handle.resourceState);

        if (handle.resourceState != RhiResourceState::CopyDst)
        {

            TransitionImageLayout(commandBuffer,
                                  handle.Image, 
                                  VkFormat,
                                  current,
                                  vk::ImageLayout::eTransferDstOptimal,
                                  VkImageAspectFlags,
                                  m_Layer, 
                                  m_Level);

            if (m_TextureUsage & TextureUsageFlagBits::RenderTarget)
            {
                handle.resourceState = RhiResourceState::RenderTarget;
            }
            if (m_TextureUsage & TextureUsageFlagBits::DepthStencil)
            {
                handle.resourceState = RhiResourceState::DepthStencilWrite;
            }
            if (m_TextureUsage & TextureUsageFlagBits::Sampled)
            {
                handle.resourceState = RhiResourceState::FragmentShaderResource;
            }
        }

        for (int i = 0; i < _layerCount; i++)
        {
            vk::BufferImageCopy region{};
            region.bufferOffset = sliceSize * i;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VkImageAspectFlags;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = static_cast<uint32_t>(i);
            region.imageSubresource.layerCount = 1;

            region.imageOffset = VkOffset3D{ 0, 0, 0 };

            region.imageExtent = vk::Extent3D{
                _imageWidht,
                _imageHeight,
                1
            };
        
            commandBuffer.copyBufferToImage(
                 stagingBuffer.buffer,
                 handle.Image,
                 vk::ImageLayout::eTransferDstOptimal,
                 region
             );    
        }

        // back to normal
        TransitionImageLayout(commandBuffer,
            handle.Image,
            VkFormat,
            vk::ImageLayout::eTransferDstOptimal,
            Vulkan::Utils::RhiResourceStateToVulkanImageLayout(handle.resourceState),
            VkImageAspectFlags,
            m_Layer,
            m_Level);
    }

    EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);
    VulkanBuffer::FreeAlloc(context, stagingBuffer);*/
}

bool Vulkan::VulkanTexture::GenerateMipMap(PC_CORE::CommandList* _CommandList)
{   
    if (!IsNeededToGenerateMip())
        return false;
    
    auto& context = GET_VK_CONTEXT;
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice)->GetDevice();
    const size_t FrameIndex = m_Rhi.GetFrameIndex();
    GET_VK_COMMAND_BUFFER(_CommandList, FrameIndex);
    TextureAndAlloc& handle = *GetTextureAndAlloc(FrameIndex);
    const vk::ImageLayout current = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(handle.resourceState);

    TransitionImageLayout(cmb,
        handle.Image,
        VkFormat,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        current,
        VkImageAspectFlags,
        m_Layer,
        m_Level);

    Utils::GenerateMipMapFunc(cmb,
                                handle.Image, 
                                vk::ImageLayout::eShaderReadOnlyOptimal,  // todo not harcoded
                                m_Width,
                                m_Height, 
                                Utils::RhiFormatToVkFormat(m_RhiFormat), 
                                m_Level, 
                                VkImageAspectFlags);
    
    // image transition to eShaderReadOnlyOptimal in GenerateMipMapFunc
    TransitionImageLayout(cmb,
        handle.Image,
        VkFormat,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        current,
        VkImageAspectFlags,
        m_Layer,
        m_Level);

    return true;
}

RhiResourceState Vulkan::VulkanTexture::GetResourceState() const
{
    const TextureAndAlloc* ptr = GetTextureAndAlloc(m_Rhi.GetFrameIndex());

    return ptr->resourceState;
}

const Vulkan::TextureAndAlloc* Vulkan::VulkanTexture::GetTextureAndAlloc(size_t _frameIndex) const
{
    if (m_Handles.empty())
    {
        PC_LOGERROR("VulkanTexture::GetFrameNativeHandle() m_Handles.empty()");
        return nullptr;
    }
    
    const size_t handleIndex = std::min(m_Handles.size() - 1, _frameIndex);
    return &m_Handles[handleIndex];
}

Vulkan::TextureAndAlloc* Vulkan::VulkanTexture::GetTextureAndAlloc(size_t _frameIndex)
{
    if (m_Handles.empty())
    {
        PC_LOGERROR("VulkanTexture::GetFrameNativeHandle() m_Handles.empty()");
        return nullptr;
    }
    
    const size_t handleIndex = std::min(m_Handles.size() - 1, _frameIndex);

    return &m_Handles[handleIndex];
}


Vulkan::VulkanTexture::~VulkanTexture()
{
    auto& context = GET_VK_CONTEXT;
    if (m_StagingBuffer.buffer != VK_NULL_HANDLE)
    {
        VulkanBuffer::FreeAlloc(context, m_StagingBuffer);
    }

    for (auto& handle : m_Handles)
        FreeAlloc(handle);
}

void Vulkan::VulkanTexture::FreeAlloc(TextureAndAlloc& _handle)
{
    auto& context = GET_VK_CONTEXT;
    vk::Device device = context.GetDevice()->GetDevice();

    if (_handle.Allocation != VK_NULL_HANDLE)
    {
        if (_handle.Image != VK_NULL_HANDLE && _handle.Allocation != VK_NULL_HANDLE)
        {
            device.destroyImageView(_handle.ImageView);
            vmaDestroyImage(context.allocator, _handle.Image, _handle.Allocation);
        }
    }
    else if (_handle.Image != VK_NULL_HANDLE && _handle.ImageView != VK_NULL_HANDLE)
    {
        device.destroyImageView(_handle.ImageView);
        device.destroyImage(_handle.Image);
    }
    _handle.Allocation = VK_NULL_HANDLE;
    _handle.ImageView = VK_NULL_HANDLE;
    _handle.Image = VK_NULL_HANDLE;
}
