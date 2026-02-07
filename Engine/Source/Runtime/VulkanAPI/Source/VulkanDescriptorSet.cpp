#include "VulkanDescriptorSet.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanDescritptorManager.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSampler.hpp"
#include "VulkanTexture.hpp"


Vulkan::VulkanDescriptorSet::VulkanDescriptorSet(PC_CORE::Rhi& _Rhi)
    : PC_CORE::RhiDescriptorSet(_Rhi)
{
}


bool Vulkan::VulkanDescriptorSet::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    if (GetBinding().empty())
        return false;

    GetDescriptorSetLayout();
    CreateDescriptors();
    UpdateDesciptors();

    return true;
}

vk::DescriptorSet Vulkan::VulkanDescriptorSet::GetVkDescriptorSet(size_t _FrameIndex) const
{
    return m_DescriptorSets[_FrameIndex];
}

vk::DescriptorSet Vulkan::VulkanDescriptorSet::GetVkDescriptorSet() const
{
    return GetVkDescriptorSet(m_Rhi.GetFrameIndex());
}


Vulkan::VulkanDescriptorSet::~VulkanDescriptorSet()
{
    VulkanContext& VkContext = static_cast<VulkanContext&>(m_Rhi.GetRhiContext());
    vk::Device d = GET_VK_DEVICE;
    d.freeDescriptorSets(VkContext.descritptorManager.GetVkDesciptorPool(), MaxFramesInFlight, m_DescriptorSets.data());
}

void Vulkan::VulkanDescriptorSet::GetDescriptorSetLayout()
{
    VulkanContext& VkContext = static_cast<VulkanContext&>(m_Rhi.GetRhiContext());
    m_Layout = VkContext.descritptorManager.GetDescriptorLayout(GetBinding());
    assert(m_Layout != VK_NULL_HANDLE);

}

void Vulkan::VulkanDescriptorSet::CreateDescriptors()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    VulkanContext& VkContext = static_cast<VulkanContext&>(m_Rhi.GetRhiContext());

    vk::Device d = GET_VK_DEVICE;
    std::vector<vk::DescriptorSetLayout> layouts(MaxFramesInFlight, m_Layout);

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
    descriptorSetAllocateInfo.descriptorPool = VkContext.descritptorManager.GetVkDesciptorPool();
    descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(MaxFramesInFlight);
    descriptorSetAllocateInfo.pSetLayouts = layouts.data();

    PC_LOG_VERBOSE("CreateDescritptor Set [{}]", GetName().data());
    d.allocateDescriptorSets(&descriptorSetAllocateInfo, m_DescriptorSets.data());

    for (const auto& descriptorSet : m_DescriptorSets)
    {
        vk::DebugUtilsObjectNameInfoEXT nameInfoImage;
        nameInfoImage.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
        nameInfoImage.pNext = nullptr;
        nameInfoImage.objectType = vk::ObjectType::eDescriptorSet;
        nameInfoImage.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkDescriptorSet>(descriptorSet));
        nameInfoImage.pObjectName = GetName().data();

        SET_VK_DEBUG_NAME(nameInfoImage);
    }
}

void Vulkan::VulkanDescriptorSet::UpdateDesciptors()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::Device d = GET_VK_DEVICE;

    const std::vector<PC_CORE::DescriptorWrite>& Bindings = GetBinding();

    size_t imageDescriptorCount = 0;
    size_t bufferDescriptorCount = 0;

    CountBufferAndImageDescriptor(Bindings, imageDescriptorCount, bufferDescriptorCount);

    // per frame in flight
    imageDescriptorCount = imageDescriptorCount * MaxFramesInFlight;
    bufferDescriptorCount = bufferDescriptorCount * MaxFramesInFlight;

    std::vector<vk::DescriptorImageInfo> descriptorImageInfos(imageDescriptorCount);
    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos(bufferDescriptorCount);

    // Reset counters
    imageDescriptorCount = 0;
    bufferDescriptorCount = 0;

    std::span<vk::DescriptorImageInfo> imageSpan(descriptorImageInfos);
    std::span<vk::DescriptorBufferInfo> bufferSpan(descriptorBufferInfos);
    FillDescriptorInfo(imageSpan, bufferSpan, imageDescriptorCount, bufferDescriptorCount);

    std::vector<vk::WriteDescriptorSet> descriptorWrites(Bindings.size() * m_DescriptorSets.size());
    std::span<vk::WriteDescriptorSet> descriptorWritesSpan(descriptorWrites);

    // Reset counters before descriptor writes
    bufferDescriptorCount = 0;
    imageDescriptorCount = 0;
    FillDescritptorWrite(descriptorWritesSpan, imageSpan, bufferSpan, imageDescriptorCount, bufferDescriptorCount);
   
    size_t descritproWriteCount = Bindings.size();
    size_t descritionWriteOffset = 0;

    for (size_t i = 0; i < m_DescriptorSets.size(); i++)
    {
        for (size_t j = 0; j < Bindings.size(); j++)
        {
            descriptorWrites[descritionWriteOffset].dstSet = m_DescriptorSets[i];
            descritionWriteOffset++;
        }
        d.updateDescriptorSets(static_cast<uint32_t>(descritproWriteCount),
            descriptorWrites.data() + descritproWriteCount * i, 0, nullptr);
    }
}

void Vulkan::VulkanDescriptorSet::FillDescriptorInfo(
    std::span<vk::DescriptorImageInfo>& imageInfo,
    std::span<vk::DescriptorBufferInfo>& bufferInfo,
    size_t& imageIndex,
    size_t& bufferIndex)
{
    for (size_t f = 0; f < m_DescriptorSets.size(); f++)
    {
        for (size_t i = 0; i < GetBinding().size(); i++)
        {
            const PC_CORE::Descriptor& des = GetBinding()[i].descriptor;

            if (std::holds_alternative<PC_CORE::BufferDescriptor>(des))
            {
                const PC_CORE::BufferDescriptor* uniformBufferDescriptor = &std::get<
                    PC_CORE::BufferDescriptor>(des);

                const VulkanBuffer& VkBuffer = *static_cast<const VulkanBuffer*>(uniformBufferDescriptor->buffer);
                const BufferAndAlloc* bufferAndAlloc = static_cast<const BufferAndAlloc*>(VkBuffer.GetBufferAndAlloc(f));

                bufferInfo[bufferIndex].buffer = bufferAndAlloc->buffer;
                bufferInfo[bufferIndex].offset = 0;
                bufferInfo[bufferIndex].range = VkBuffer.GetBufferBackingStrategy() != PC_CORE::RhiBuffer::BufferBackingStrategy::CpuVisibleRing ? VK_WHOLE_SIZE : VkBuffer.GetObjectSize();
                bufferIndex++;
            }

            if (std::holds_alternative<PC_CORE::ImageSamplerDescriptor>(des))
            {
                const PC_CORE::ImageSamplerDescriptor& imageSamplerDescriptor = std::get<PC_CORE::ImageSamplerDescriptor>(des);

                const VulkanTexture& VkTexture = *static_cast<const VulkanTexture*>(imageSamplerDescriptor.Texture);
                const TextureAndAlloc& textureAndAlloc = static_cast<const TextureAndAlloc&>(*VkTexture.GetTextureAndAlloc());

                const VulkanSampler& VkSampler = *static_cast<const VulkanSampler*>(imageSamplerDescriptor.Sampler);
                const vk::Sampler samplerHandle = VkSampler.GetVkSampler();

                imageInfo[imageIndex].imageLayout = Utils::RhiResourceStateToVulkanImageLayout(
                    imageSamplerDescriptor.ResourceState);
                imageInfo[imageIndex].imageView = textureAndAlloc.ImageView;
                imageInfo[imageIndex].sampler = samplerHandle;
                imageIndex++;
            }

            if (std::holds_alternative<PC_CORE::ImageDescriptor>(des))
            {
                const PC_CORE::ImageDescriptor& imageDescriptor = std::get<PC_CORE::ImageDescriptor>(des);

                const VulkanTexture& VkTexture = *static_cast<const VulkanTexture*>(imageDescriptor.Texture);
                const TextureAndAlloc& textureAndAlloc = static_cast<const TextureAndAlloc&>(*VkTexture.GetTextureAndAlloc());

                imageInfo[imageIndex].imageLayout = Utils::RhiResourceStateToVulkanImageLayout(
                    imageDescriptor.ResourceState);
                imageInfo[imageIndex].imageView = textureAndAlloc.ImageView;
                imageIndex++;
            }

            if (std::holds_alternative<PC_CORE::InputAttachementDescriptor>(des))
            {
                const PC_CORE::InputAttachementDescriptor& inputAttachementDescriptor = std::get<PC_CORE::InputAttachementDescriptor>(des);

                const VulkanTexture& VkTexture = *static_cast<const VulkanTexture*>(inputAttachementDescriptor.image);
                const TextureAndAlloc& textureAndAlloc = static_cast<const TextureAndAlloc&>(*VkTexture.GetTextureAndAlloc());

                imageInfo[imageIndex].imageLayout = Utils::RhiResourceStateToVulkanImageLayout(
                    inputAttachementDescriptor.resourceState);
                imageInfo[imageIndex].imageView = textureAndAlloc.ImageView;
                imageIndex++;
            }
        }
    }
}

void Vulkan::VulkanDescriptorSet::FillDescritptorWrite(std::span<vk::WriteDescriptorSet>& _WriteDescriptorSetSpan,
    const std::span<vk::DescriptorImageInfo>& imageInfo , const std::span<vk::DescriptorBufferInfo>& bufferInfo,
    size_t& imageIndex,
    size_t& bufferIndex)
{
    const size_t BindingSize = GetBinding().size();
    size_t descriptorWriteOffset = 0;
    for (size_t f = 0; f < m_DescriptorSets.size(); f++)
    {
        for (size_t i = 0; i < BindingSize; i++)
        {
            auto& CurrentBinding = GetBinding()[i];

            size_t descriptorWriteIndex = i + descriptorWriteOffset;
            _WriteDescriptorSetSpan[descriptorWriteIndex].sType = vk::StructureType::eWriteDescriptorSet;
            _WriteDescriptorSetSpan[descriptorWriteIndex].dstBinding = CurrentBinding.bindingIndex;
            _WriteDescriptorSetSpan[descriptorWriteIndex].dstArrayElement = 0;
            _WriteDescriptorSetSpan[descriptorWriteIndex].descriptorCount = 1;
            _WriteDescriptorSetSpan[descriptorWriteIndex].descriptorType = Utils::RhiToDescriptorType(
                CurrentBinding.type);

            switch (CurrentBinding.type)
            {
            case PC_CORE::DescriptorType::UniformBuffer:
            case PC_CORE::DescriptorType::StorageBuffer:
            case PC_CORE::DescriptorType::DynamicUniformBuffer:
            case PC_CORE::DescriptorType::DynamicStorageBuffer:
                _WriteDescriptorSetSpan[descriptorWriteIndex].pBufferInfo = &bufferInfo[bufferIndex++];
                break;
            case PC_CORE::DescriptorType::CombinedImageSampler:
                _WriteDescriptorSetSpan[descriptorWriteIndex].pImageInfo = &imageInfo[imageIndex++];
                break;
            case PC_CORE::DescriptorType::StorageImage:
                _WriteDescriptorSetSpan[descriptorWriteIndex].pImageInfo = &imageInfo[imageIndex++];
                break;
            case PC_CORE::DescriptorType::InputAttachment:
                _WriteDescriptorSetSpan[descriptorWriteIndex].pImageInfo = &imageInfo[imageIndex++];
                break;
            case PC_CORE::DescriptorType::SampledImage:
                _WriteDescriptorSetSpan[descriptorWriteIndex].pImageInfo = &imageInfo[imageIndex++];
                break;
            case PC_CORE::DescriptorType::Sampler:
            case PC_CORE::DescriptorType::InlineUniformBlock:
            case PC_CORE::DescriptorType::AccelerationStructure:
            case PC_CORE::DescriptorType::Count:
            default:
                assert(false && "Unsupported shader program descriptor type");
            }
            
        }
        descriptorWriteOffset += BindingSize;
    }
}

void Vulkan::VulkanDescriptorSet::CountBufferAndImageDescriptor(const std::vector<PC_CORE::DescriptorWrite>& Bindings, size_t& ImageCount, size_t& BufferCount)
{
    for (size_t i = 0; i < Bindings.size(); i++)
    {
        const PC_CORE::Descriptor& des = Bindings[i].descriptor;

        if (std::holds_alternative<PC_CORE::BufferDescriptor>(des))
            BufferCount++;

        if (std::holds_alternative<PC_CORE::ImageSamplerDescriptor>(des))
            ImageCount++;

        if (std::holds_alternative<PC_CORE::ImageDescriptor>(des))
            ImageCount++;

        if (std::holds_alternative<PC_CORE::InputAttachementDescriptor>(des))
            ImageCount++;
    }
}