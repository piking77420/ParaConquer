#include <memory>

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

    RhiTexture::Build();
   
    const size_t nbrOfObjectHandle = m_NbrOfBackendObject;
    if (nbrOfObjectHandle == 0)
    {
        PC_LOGERROR("VulkanTexture::Create() nbrOfObjectHandle = 0");
        return false;
    }

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
    imageInfo.format = VkFormat;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = Utils::GetImageUsageFlags(GetTextureUsage());
    imageInfo.samples = Utils::RhSampleCountToVulkan(GetSamples());
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.flags = Utils::ImageCreateFlagFromTextureType(GetTextureType());


    {
        auto& context = GET_VK_CONTEXT;
        const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->GetDevice();
        std::shared_ptr<VulkanInstance> instance = context.GetInstance();

        std::scoped_lock _(context.VulkanContextMutex()); // we need to lock it because multiple theread can create device
        assert(nbrOfObjectHandle == 1);
        for (size_t i = 0; i < nbrOfObjectHandle; i++)
        {
            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = m_MemoryUsage == RhiResource::MemoryUsage::CPUVisible ? VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE : VMA_MEMORY_USAGE_GPU_ONLY;

            VmaAllocationInfo VmaAllocationInfo;
            VmaAllocationInfo.pName = GetName().data();
            VK_CALL(static_cast<vk::Result>(vmaCreateImage(context.allocator, reinterpret_cast<VkImageCreateInfo*>(&imageInfo), &allocInfo,
                reinterpret_cast<VkImage*>(&m_Handle.Image), &m_Handle.Allocation, &VmaAllocationInfo)));

            vk::DebugUtilsObjectNameInfoEXT nameInfoImage;
            nameInfoImage.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
            nameInfoImage.pNext = nullptr;
            nameInfoImage.objectType = vk::ObjectType::eImage;
            nameInfoImage.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImage>(m_Handle.Image));
            nameInfoImage.pObjectName = GetName().data();

            SET_VK_DEBUG_NAME(nameInfoImage);

            vk::ImageViewCreateInfo imageviewInfo = GetImageViewCreateInfo(Utils::RhiImageToVkImageViewType(GetTextureType()), 0, GetLayer(), 0, GetLevel());
            VK_CALL(device.createImageView(&imageviewInfo, nullptr, &m_Handle.ImageView));

            vk::DebugUtilsObjectNameInfoEXT nameInfoImageView;
            nameInfoImageView.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
            nameInfoImageView.pNext = nullptr;
            nameInfoImageView.objectType = vk::ObjectType::eImageView;
            nameInfoImageView.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImageView>(m_Handle.ImageView));
            nameInfoImageView.pObjectName = GetName().data();

            SET_VK_DEBUG_NAME(nameInfoImageView);
        }
    }
    


    return true;
}

bool Vulkan::VulkanTexture::UploadData2D(PC_CORE::CommandList* _CommandList, const void* _Data, const std::vector<LevelUploadOperation>& _LevelUpload)
{
    assert(_CommandList != nullptr);

    if (GetTextureType() != Type::Texture2D)
    {
        PC_LOGERROR("UploadData2D should only be used for TextureType::Texture2D");
        return false;
    }
    
    auto& context = GET_VK_CONTEXT;

    {
        std::scoped_lock _(context.VulkanContextMutex());
        const size_t FrameIndex = m_Rhi.GetFrameIndex();
        const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice)->GetDevice();
        auto& StagingBufferFrame = m_StagingBuffer;

        if (StagingBufferFrame.buffer != VK_NULL_HANDLE) // TODO CACHE THOS DEPENDING ON SIZE
        {
            context.DefferdDestroy(StagingBufferFrame, FrameIndex);
        }

        uint32_t TotalSize = 0;
        if (!_LevelUpload.empty())
            TotalSize = _LevelUpload.back().Offset + _LevelUpload.back().Size;

        VulkanBuffer::CreateStagingBufferForCopy(context, &StagingBufferFrame, TotalSize, m_Name.c_str());

        void* mappedData = nullptr;
        vmaMapMemory(context.allocator, StagingBufferFrame.alloc, &mappedData);
        assert(mappedData != nullptr);
        std::memcpy(mappedData, _Data, TotalSize);
        vmaUnmapMemory(context.allocator, StagingBufferFrame.alloc);


        GET_VK_COMMAND_BUFFER(_CommandList, FrameIndex);

        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(context.allocator, m_Handle.Allocation, &allocInfo);

        VkDeviceSize size = allocInfo.size;

        assert(TotalSize <= size);

        uint32_t Offset = 0;
        for (size_t i = 0; i < _LevelUpload.size(); i++)
        {
            vk::BufferImageCopy region{};
            region.bufferOffset = _LevelUpload[i].Offset;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VkImageAspectFlags;
            region.imageSubresource.mipLevel = i;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = VkOffset3D{ 0, 0, 0 };

            region.imageExtent = vk::Extent3D{
                _LevelUpload[i].Width,
                _LevelUpload[i].Height,
                1
            };

            cmb.copyBufferToImage(
                StagingBufferFrame.buffer,
                m_Handle.Image,
                vk::ImageLayout::eTransferDstOptimal,
                region
            );
            Offset += _LevelUpload[i].Size;
        }
    }
    return true;  
}

void Vulkan::VulkanTexture::UploadDataLayer(PC_CORE::CommandList* commandList, const std::vector<void*>& _imageDatas, const std::vector<std::vector<PC_CORE::RhiTexture::LevelUploadOperation>>& _LayerUploads)
{
    
    if (m_TextureType != Type::TextureArray2D &&
        m_TextureType != Type::CubeMap &&
        m_TextureType != Type::CubeMapArray)
    {
        PC_LOGERROR("UploadDataLayer should only be used for 2D array/cubemap types");
        return;
    }
    
    
    if (_imageDatas.size() != _LayerUploads.size())
    {
        PC_LOGERROR("_imageDatas.size() != _layerCount");
        return;
    }
    {
        auto& context = GET_VK_CONTEXT;
        std::scoped_lock _(context.VulkanContextMutex());

    }

    /*
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

    for (size_t i = 0; i < m_Handle.size(); i++)
    {
        TextureAndAlloc& handle = m_Handle[i];

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

bool Vulkan::VulkanTexture::GenerateMipMap(PC_CORE::CommandList* _CommandList, PC_CORE::Filter _Filter, RhiResourceState _StateAfterOperation)
{   
    if (!IsNeededToGenerateMip())
        return false;
    
    auto& context = GET_VK_CONTEXT;
    const vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice)->GetDevice();
    const size_t FrameIndex = m_Rhi.GetFrameIndex();
    GET_VK_COMMAND_BUFFER(_CommandList, FrameIndex);


    const vk::ImageLayout OldImageLayout = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(m_RhiResourceState);
    const vk::AccessFlags Oldflag = Vulkan::Utils::RhiResourceStateToAccesFlag(m_RhiResourceState);
    const vk::PipelineStageFlags OldDstStageFlags = Utils::PipelineStageFlagsFromRhiResourceState(m_RhiResourceState);

    const vk::ImageLayout NewImageLayout = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(_StateAfterOperation);
    const vk::AccessFlags Newflag = Vulkan::Utils::RhiResourceStateToAccesFlag(_StateAfterOperation);
    const vk::PipelineStageFlags NewDstStageFlags = Utils::PipelineStageFlagsFromRhiResourceState(_StateAfterOperation);

    Utils::GenerateMipMapFunc(cmb,
                                m_Handle.Image,
                                Utils::RhiToVulkanFilter(_Filter),
                                // CURRENT
                                Oldflag,
                                OldImageLayout,
                                OldDstStageFlags,
                                // NEW
                                Newflag,
                                NewImageLayout,
                                NewDstStageFlags,

                                m_Width,
                                m_Height, 
                                Utils::RhiFormatToVkFormat(m_RhiFormat), 
                                m_Level,
                                GetLayer(),
                                VkImageAspectFlags);

    m_RhiResourceState = _StateAfterOperation;

    return true;
}

const Vulkan::TextureAndAlloc* Vulkan::VulkanTexture::GetTextureAndAlloc() const
{
    return &m_Handle;
}

Vulkan::TextureAndAlloc* Vulkan::VulkanTexture::GetTextureAndAlloc()
{
    return &m_Handle;
}

vk::ImageViewCreateInfo Vulkan::VulkanTexture::GetImageViewCreateInfo(vk::ImageViewType _ViewType, uint32_t _BaseArrayLayer, uint32_t _LayerCount,  uint32_t _BaseMipLevel, uint32_t _LevelCount) const
{
    vk::ImageViewCreateInfo imageviewInfo{};
    imageviewInfo.sType = vk::StructureType::eImageViewCreateInfo;
    imageviewInfo.image = m_Handle.Image;
    imageviewInfo.viewType = _ViewType;
    imageviewInfo.format = Utils::RhiFormatToVkFormat(GetRhiFormat());

    imageviewInfo.subresourceRange.aspectMask = VkImageAspectFlags;
    imageviewInfo.subresourceRange.baseMipLevel = _BaseMipLevel;
    imageviewInfo.subresourceRange.levelCount = _LevelCount;
    imageviewInfo.subresourceRange.baseArrayLayer = _BaseArrayLayer;
    imageviewInfo.subresourceRange.layerCount = _LayerCount;

    return imageviewInfo;
}

Vulkan::VulkanTexture::~VulkanTexture()
{
    auto& context = GET_VK_CONTEXT;

    context.DefferdDestroy(m_StagingBuffer, m_Rhi.GetFrameIndex());
    context.DefferdDestroy(m_Handle, m_Rhi.GetFrameIndex());
}

