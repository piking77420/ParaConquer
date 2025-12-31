#include "Resources/VulkanDescriptorSets.hpp"

#include "LowRenderer/Rhi.hpp"
#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"
#include "VulkanDescritptorManager.hpp"
#include "Resources/VulkanSampler.hpp"
#include "Utils/RhiToVulkan.hpp"

static inline void CountBufferAndImageDescriptor(const std::vector<PC_CORE::DescriptorWrite>& Bindings,
    size_t& ImageCount, size_t& BufferCount)
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

Vulkan::VulkanDescriptorSets::VulkanDescriptorSets(PC_CORE::Rhi& _Rhi, const CacheDescriptor& _Cache)
    : ShaderProgramDescriptorSets(_Rhi)
    , m_Cache(_Cache)
{
}


bool Vulkan::VulkanDescriptorSets::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    if (GetBinding().empty())
        return false;

    CreateDescriptors();
    UpdateDesciptors();

    return true;
}


Vulkan::VulkanDescriptorSets::~VulkanDescriptorSets()
{
    vk::Device d = GET_VK_DEVICE;
    d.freeDescriptorSets(m_Cache.descriptorPool, MaxFramesInFlight, descriptorSets.data());
}

void Vulkan::VulkanDescriptorSets::CreateDescriptors()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::Device d = GET_VK_DEVICE;

    std::vector<vk::DescriptorSetLayout> layouts(MaxFramesInFlight, m_Cache.descriptorSetLayout[GetSet()]);
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(GET_VK_CONTEXT.rhiDevice)->GetDevice();

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
    descriptorSetAllocateInfo.descriptorPool = m_Cache.descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(MaxFramesInFlight);
    descriptorSetAllocateInfo.pSetLayouts = layouts.data();

    PC_LOG_VERBOSE("CreateDescritptor Set [{}]", GetName().data());
    d.allocateDescriptorSets(&descriptorSetAllocateInfo, descriptorSets.data());

    for (const auto& descriptorSet : descriptorSets)
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

void Vulkan::VulkanDescriptorSets::UpdateDesciptors()
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

    std::vector<vk::WriteDescriptorSet> descriptorWrites(Bindings.size() * descriptorSets.size());
    std::span<vk::WriteDescriptorSet> descriptorWritesSpan(descriptorWrites);

    // Reset counters before descriptor writes
    bufferDescriptorCount = 0;
    imageDescriptorCount = 0;
    FillDescritptorWrite(descriptorWritesSpan, imageSpan, bufferSpan, imageDescriptorCount, bufferDescriptorCount);
   
    size_t descritproWriteCount = Bindings.size();
    size_t descritionWriteOffset = 0;

    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        for (size_t j = 0; j < Bindings.size(); j++)
        {
            descriptorWrites[descritionWriteOffset].dstSet = descriptorSets[i];
            descritionWriteOffset++;
        }
        d.updateDescriptorSets(static_cast<uint32_t>(descritproWriteCount),
            descriptorWrites.data() + descritproWriteCount * i, 0, nullptr);
    }
}

void Vulkan::VulkanDescriptorSets::FillDescriptorInfo(
    std::span<vk::DescriptorImageInfo>& imageInfo,
    std::span<vk::DescriptorBufferInfo>& bufferInfo,
    size_t& imageIndex,
    size_t& bufferIndex)
{
    for (size_t f = 0; f < descriptorSets.size(); f++)
    {
        for (size_t i = 0; i < GetBinding().size(); i++)
        {
            const PC_CORE::Descriptor& des = GetBinding()[i].descriptor;

            if (std::holds_alternative<PC_CORE::BufferDescriptor>(des))
            {
                const PC_CORE::BufferDescriptor* uniformBufferDescriptor = &std::get<
                    PC_CORE::BufferDescriptor>(des);

                const VulkanBuffer& VkBuffer = *reinterpret_cast<VulkanBuffer*>(uniformBufferDescriptor->buffer);
                const BufferAndAlloc* bufferAndAlloc = static_cast<const BufferAndAlloc*>(VkBuffer.GetBufferAndAlloc(f));

                bufferInfo[bufferIndex].buffer = bufferAndAlloc->buffer;
                bufferInfo[bufferIndex].offset = 0;
                bufferInfo[bufferIndex].range = VK_WHOLE_SIZE;
                bufferIndex++;
            }

            if (std::holds_alternative<PC_CORE::ImageSamplerDescriptor>(des))
            {
                const PC_CORE::ImageSamplerDescriptor& imageSamplerDescriptor = std::get<PC_CORE::ImageSamplerDescriptor>(des);

                const VulkanTexture& VkTexture = reinterpret_cast<const VulkanTexture&>(*imageSamplerDescriptor.texture);
                const TextureAndAlloc& textureAndAlloc = static_cast<const TextureAndAlloc&>(*VkTexture.GetTextureAndAlloc(f));

                const VulkanSampler& VkSampler = reinterpret_cast<const VulkanSampler&>(*imageSamplerDescriptor.sampler);
                const vk::Sampler samplerHandle = VkSampler.GetVkSampler();

                imageInfo[imageIndex].imageLayout = Utils::RhiResourceStateToVulkanImageLayout(
                    imageSamplerDescriptor.resourceState);
                imageInfo[imageIndex].imageView = textureAndAlloc.ImageView;
                imageInfo[imageIndex].sampler = samplerHandle;
                imageIndex++;
            }

            if (std::holds_alternative<PC_CORE::ImageDescriptor>(des))
            {
                const PC_CORE::ImageDescriptor& imageDescriptor = std::get<PC_CORE::ImageDescriptor>(des);

                const VulkanTexture& VkTexture = reinterpret_cast<const VulkanTexture&>(*imageDescriptor.texture);
                const TextureAndAlloc& textureAndAlloc = static_cast<const TextureAndAlloc&>(*VkTexture.GetTextureAndAlloc(f));

                imageInfo[imageIndex].imageLayout = Utils::RhiResourceStateToVulkanImageLayout(
                    imageDescriptor.resourceState);
                imageInfo[imageIndex].imageView = textureAndAlloc.ImageView;
                imageIndex++;
            }

            if (std::holds_alternative<PC_CORE::InputAttachementDescriptor>(des))
            {
                const PC_CORE::InputAttachementDescriptor& inputAttachementDescriptor = std::get<
                    PC_CORE::InputAttachementDescriptor>(des);

                const VulkanTexture& VkTexture = reinterpret_cast<const VulkanTexture&>(*inputAttachementDescriptor.image);
                const TextureAndAlloc& textureAndAlloc = static_cast<const TextureAndAlloc&>(*VkTexture.GetTextureAndAlloc(f));

                imageInfo[imageIndex].imageLayout = Utils::RhiResourceStateToVulkanImageLayout(
                    inputAttachementDescriptor.resourceState);
                imageInfo[imageIndex].imageView = textureAndAlloc.ImageView;
                imageIndex++;
            }
        }
    }
}

void Vulkan::VulkanDescriptorSets::FillDescritptorWrite(std::span<vk::WriteDescriptorSet>& _WriteDescriptorSetSpan,
    const std::span<vk::DescriptorImageInfo>& imageInfo , const std::span<vk::DescriptorBufferInfo>& bufferInfo,
    size_t& imageIndex,
    size_t& bufferIndex)
{
    const size_t BindingSize = GetBinding().size();
    size_t descriptorWriteOffset = 0;
    for (size_t f = 0; f < descriptorSets.size(); f++)
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
            case PC_CORE::DescriptorType::StorageBuffer:;
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
            case PC_CORE::DescriptorType::Sampler:
            case PC_CORE::DescriptorType::SampledImage:
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


const void* Vulkan::VulkanDescriptorSets::GetFrameNativeHandle(size_t _frameIndex) const
{
    return &descriptorSets[_frameIndex];
}

void* Vulkan::VulkanDescriptorSets::GetFrameNativeHandle(size_t _frameIndex)
{
    return &descriptorSets[_frameIndex];
}

