#include <Memory>

#include "LowRenderer/Rhi.hpp"
#include "VulkanContext.hpp"
#include "VulkanTexture.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandList.hpp"

#include "Utils/HelperFunctions.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "Utils/TransitionImageLayout.hpp"
#include "Utils/VulkanImageHelper.hpp"

static vk::ImageLayout GetDefaultImageLayout(PC_CORE::RhiTexture::TextureUsageFlag flag, vk::ImageAspectFlags aspect)
{
    using TexF = PC_CORE::RhiTexture::TextureUsageFlag;
    
    if (flag & TexF::Storage) // for compute shader
        return vk::ImageLayout::eGeneral;

    if (flag & TexF::Sampled)
    {
        if (aspect & vk::ImageAspectFlagBits::eDepth)
            return vk::ImageLayout::eDepthStencilReadOnlyOptimal;

        return vk::ImageLayout::eShaderReadOnlyOptimal;
    }

    if (flag & TexF::RenderTarget)
    {
        if (aspect & vk::ImageAspectFlagBits::eColor)
            return vk::ImageLayout::eColorAttachmentOptimal;

        if (aspect & vk::ImageAspectFlagBits::eDepth)
            return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    }
    
    assert(false && "Unsuported usage");
    
    return vk::ImageLayout::eUndefined;
}


static RhiResourceState GetAfterCreationImageLayout(
    PC_CORE::RhiTexture::TextureUsageFlag flag,
    vk::ImageAspectFlags aspect)
{
    using TexF = PC_CORE::RhiTexture::TextureUsageFlag;

    if (flag & TexF::TransferDst)
        return RhiResourceState::CopyDst;

    if (flag & TexF::TransferSrc)
        return RhiResourceState::CopySrc;

    if (flag & TexF::RenderTarget)
    {
        if (aspect & vk::ImageAspectFlagBits::eColor)
            return RhiResourceState::RenderTarget;

        if (aspect & vk::ImageAspectFlagBits::eDepth && aspect & vk::ImageAspectFlagBits::eStencil)
            return RhiResourceState::DepthStencilWrite;
    }

    if (flag & TexF::Sampled)
    {
        if (aspect & vk::ImageAspectFlagBits::eDepth)
            return RhiResourceState::DepthStencilRead;

        return RhiResourceState::ShaderRead;
    }

    if (flag & TexF::Storage) // for compute shader
        return RhiResourceState::ComputeWrite; // eGeneral layout 

    assert(false && "Unsuported usage");

    return RhiResourceState::Undefined;
}

const void* Vulkan::VulkanTexture::GetFrameNativeHandle(size_t _frameIndex) const
{
    return GetTextureAndAlloc(_frameIndex);
}

void* Vulkan::VulkanTexture::GetFrameNativeHandle(size_t _frameIndex)
{
    return GetTextureAndAlloc(_frameIndex);
}

Vulkan::VulkanTexture::VulkanTexture(PC_CORE::Rhi& _Rhi, const std::string& _name, const RhiTextureDesciptor& _rhiTextureDesciptor, MemoryUsage _memoryUsage)
    : RhiTexture(_Rhi, _name, _rhiTextureDesciptor, _memoryUsage)
{

}

Vulkan::VulkanTexture::VulkanTexture(PC_CORE::Rhi& _Rhi, std::string&& _name, const RhiTextureDesciptor& _rhiTextureDesciptor, MemoryUsage _memoryUsage)
    : RhiTexture(_Rhi, _name, _rhiTextureDesciptor, _memoryUsage)
{
    
}

bool Vulkan::VulkanTexture::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    
    if (m_RhiTextureDesciptor.Width == 0 || m_RhiTextureDesciptor.Height == 0)
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

    VkImageAspectFlags = Utils::RhiTextureFormatToImageAspectFlagFlags(m_RhiTextureDesciptor.RhiFormat);
    VkFormat = Utils::RhiFormatToVkFormat(m_RhiTextureDesciptor.RhiFormat);
    
    vk::ImageCreateInfo imageInfo{};
    imageInfo.sType = vk::StructureType::eImageCreateInfo;
    imageInfo.imageType = Utils::RhiImageToVkImageType(m_RhiTextureDesciptor.TextureType);
    imageInfo.extent.width = m_RhiTextureDesciptor.Width;
    imageInfo.extent.height = m_RhiTextureDesciptor.Height;
    imageInfo.extent.depth = m_RhiTextureDesciptor.Depth;
    imageInfo.mipLevels = m_RhiTextureDesciptor.Level;
    imageInfo.arrayLayers = m_RhiTextureDesciptor.LayerCount;
    imageInfo.format = Utils::RhiFormatToVkFormat(m_RhiTextureDesciptor.RhiFormat);
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = Utils::GetImageUsageFlags(m_RhiTextureDesciptor.TextureUsage, VkImageAspectFlags);
    imageInfo.samples = Utils::RhSampleCountToVulkan(m_RhiTextureDesciptor.Samples);
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.flags = Utils::ImageCreateFlagFromTextureType(m_RhiTextureDesciptor.TextureType);
 
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
        imageviewInfo.viewType = Utils::RhiImageToVkImageViewType(m_RhiTextureDesciptor.TextureType);
        imageviewInfo.format = imageInfo.format;
        imageviewInfo.subresourceRange.aspectMask = VkImageAspectFlags;
        imageviewInfo.subresourceRange.baseMipLevel = 0;
        imageviewInfo.subresourceRange.baseArrayLayer = 0;
        imageviewInfo.subresourceRange.levelCount = m_RhiTextureDesciptor.Level;
        imageviewInfo.subresourceRange.layerCount = m_RhiTextureDesciptor.LayerCount;
        
        VK_CALL(device.createImageView(&imageviewInfo, nullptr, &m_Handles[i].ImageView));
        
        vk::DebugUtilsObjectNameInfoEXT nameInfoImageView;
        nameInfoImageView.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
        nameInfoImageView.pNext = nullptr;
        nameInfoImageView.objectType = vk::ObjectType::eImageView;
        nameInfoImageView.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImageView>(m_Handles[i].ImageView));
        nameInfoImageView.pObjectName = GetName().data();
        
        SET_VK_DEBUG_NAME(nameInfoImageView); 
    }
    
    return true;
}

void Vulkan::VulkanTexture::UploadData2D(PC_CORE::CommandList* commandList, const void* _imageData, const uint32_t _imageWidht, const uint32_t _imageHeight, PC_CORE::RhiChannel _channel)
{
    if (m_RhiTextureDesciptor.TextureType != Type::Texture2D)
    {
        PC_LOGERROR("UploadData2D should only be used for TextureType::Texture2D");
        return;
    }
    
    auto& context = GET_VK_CONTEXT;
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice)->GetDevice();
    const int multiplayer = PC_CORE::GetBytePerPixel(m_RhiTextureDesciptor.RhiFormat);
    const size_t imageSize = static_cast<size_t>(_imageWidht * _imageHeight * multiplayer);
    
    
    BufferAndAlloc stagingBuffer;
    VulkanBuffer::CreateStagingBufferForCopy(context, &stagingBuffer, imageSize);

    void* mappedData;
    vmaMapMemory(context.allocator, stagingBuffer.alloc, &mappedData);
    std::memcpy(mappedData, _imageData, imageSize);
    vmaUnmapMemory(context.allocator, stagingBuffer.alloc);

    for (size_t i = 0; i < m_Handles.size(); i++)
    {
        TextureAndAlloc& handle = m_Handles[i];
        GET_VK_COMMAND_BUFFER(commandList, i);

        const vk::ImageLayout current = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(handle.resourceState);

        if (handle.resourceState != RhiResourceState::CopyDst)
        {
            TransitionImageLayout(cmb,
                handle.Image,
                VkFormat,
                current,
                vk::ImageLayout::eTransferDstOptimal,
                VkImageAspectFlags,
                m_RhiTextureDesciptor.LayerCount,
                m_RhiTextureDesciptor.Level);
        }

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
            _imageWidht,
            _imageHeight,
            1
        };
        
        cmb.copyBufferToImage(
             stagingBuffer.buffer,
             handle.Image,
             vk::ImageLayout::eTransferDstOptimal,
             region
         );    

        // back to normal
        TransitionImageLayout(cmb,
            handle.Image,
            VkFormat,
            vk::ImageLayout::eTransferDstOptimal,
            current,
            VkImageAspectFlags,
            m_RhiTextureDesciptor.LayerCount,
            m_RhiTextureDesciptor.Level);
    }
    
}

void Vulkan::VulkanTexture::UploadDataLayer(PC_CORE::CommandList* commandList, const std::vector<void*>& _imageDatas, uint32_t _imageWidht, uint32_t _imageHeight,
                                            uint32_t _layerCount, PC_CORE::RhiChannel _channel)
{
    if (m_RhiTextureDesciptor.TextureType != Type::TextureArray2D &&
       m_RhiTextureDesciptor.TextureType != Type::CubeMap &&
       m_RhiTextureDesciptor.TextureType != Type::CubeMapArray)
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

    const int bytesPerPixel = PC_CORE::GetBytePerPixel(m_RhiTextureDesciptor.RhiFormat);
    const uint32_t sliceSize = _imageWidht * _imageHeight * bytesPerPixel;
    const uint32_t totalSize = sliceSize * _layerCount;

    
    BufferAndAlloc stagingBuffer;
    VulkanBuffer::CreateStagingBufferForCopy(context, &stagingBuffer, totalSize);

    void* mappedData;
    vmaMapMemory(context.allocator, stagingBuffer.alloc, &mappedData);
    for (uint32_t i = 0; i < _layerCount; i++)
        std::memcpy(static_cast<uint8_t*>(mappedData) + (sliceSize * i), _imageDatas[i], sliceSize);
    vmaUnmapMemory(context.allocator, stagingBuffer.alloc);
    
    
    for (size_t i = 0; i < m_Handles.size(); i++)
    {
        TextureAndAlloc& handle = m_Handles[i];
        GET_VK_COMMAND_BUFFER(commandList, i);

        const vk::ImageLayout current = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(handle.resourceState);

        if (handle.resourceState != RhiResourceState::CopyDst)
        {

            TransitionImageLayout(cmb, 
                                  handle.Image, 
                                  VkFormat,
                                  current,
                                  vk::ImageLayout::eTransferDstOptimal,
                                  VkImageAspectFlags,
                                  m_RhiTextureDesciptor.LayerCount, 
                                  m_RhiTextureDesciptor.Level);
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
        
            cmb.copyBufferToImage(
                 stagingBuffer.buffer,
                 handle.Image,
                 vk::ImageLayout::eTransferDstOptimal,
                 region
             );    
        }

        // back to normal
        TransitionImageLayout(cmb,
            handle.Image,
            VkFormat,
            vk::ImageLayout::eTransferDstOptimal,
            current,
            VkImageAspectFlags,
            m_RhiTextureDesciptor.LayerCount,
            m_RhiTextureDesciptor.Level);
    }
}

void Vulkan::VulkanTexture::GenerateMipMap(PC_CORE::CommandList* commandList)
{   
    if (!IsNeededToGenerateMip())
        return;
    
    auto& context = GET_VK_CONTEXT;
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice)->GetDevice();


    for (size_t i = 0; i < m_Handles.size(); i++)
    {
        TextureAndAlloc& handle = m_Handles[i];
        GET_VK_COMMAND_BUFFER(commandList, i);
        const vk::ImageLayout current = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(handle.resourceState);

        Utils::GenerateMipMapFunc(cmb, 
                                  handle.Image, 
                                  vk::ImageLayout::eShaderReadOnlyOptimal,  // todo not harcoded
                                  m_RhiTextureDesciptor.Width,
                                  m_RhiTextureDesciptor.Height, 
                                  Utils::RhiFormatToVkFormat(m_RhiTextureDesciptor.RhiFormat), 
                                  m_RhiTextureDesciptor.Level, 
                                  VkImageAspectFlags);
    
        // image transition to eShaderReadOnlyOptimal in GenerateMipMapFunc
            TransitionImageLayout(cmb,
                handle.Image,
                VkFormat,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                current,
                VkImageAspectFlags,
                m_RhiTextureDesciptor.LayerCount,
                m_RhiTextureDesciptor.Level);
        
    }
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
