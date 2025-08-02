#include "resources/vulkan_descriptor_sets.hpp"

#include "utils/rhi_vulkan_parser.hpp"
#include "vulkan_device.hpp"
#include "buffer/vulkan_buffer.hpp"


#include "low_renderer/rhi.hpp"
#include "low_renderer/rhi_uniform_buffer.hpp"
#include "resources/vulkan_sampler.hpp"
#include "texture/vulkan_texture.hpp"

void Vulkan::VulkanDescriptorSets::WriteDescriptorSets(const std::vector<PC_CORE::ShaderProgramDescriptorWrite>& _shaderProgramDescriptorSet)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->
        GetDevice();

    size_t bufferDescriptorCount = 0;
    size_t imageDescriptorCount = 0;

    // Count descriptors
    for (size_t i = 0; i < _shaderProgramDescriptorSet.size(); i++)
    {
        const PC_CORE::Descriptor& des = _shaderProgramDescriptorSet[i].descriptor;

        if (std::holds_alternative<PC_CORE::UniformBufferDescriptor>(des))
            bufferDescriptorCount++;

        if (std::holds_alternative<PC_CORE::ImageSamplerDescriptor>(des))
            imageDescriptorCount++;

        if (std::holds_alternative<PC_CORE::ImageDescriptor>(des))
            imageDescriptorCount++;

        if (std::holds_alternative<PC_CORE::InputAttachementDescriptor>(des))
            imageDescriptorCount++;
    }

    // Double the size per frame in flight
    imageDescriptorCount = imageDescriptorCount * MAX_FRAMES_IN_FLIGHT;
    bufferDescriptorCount = bufferDescriptorCount * MAX_FRAMES_IN_FLIGHT;

    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos(bufferDescriptorCount);
    std::vector<vk::DescriptorImageInfo> descriptorImageInfos(imageDescriptorCount);

    // Reset counters
    bufferDescriptorCount = 0;
    imageDescriptorCount = 0;
    
    for (size_t f = 0; f < m_DescriptorSets.size(); f++)
    {
        for (size_t i = 0; i < _shaderProgramDescriptorSet.size(); i++)
        {
            const PC_CORE::Descriptor& des = _shaderProgramDescriptorSet[i].descriptor;

            if (std::holds_alternative<PC_CORE::UniformBufferDescriptor>(des))
            {
                const PC_CORE::UniformBufferDescriptor* uniformBufferDescriptor = &std::get<PC_CORE::UniformBufferDescriptor>(des);

                const VulkanBuffer* buffer = static_cast<const VulkanBuffer*>(uniformBufferDescriptor->buffer->GetRhiHandle()->GetNativeHandle());
                assert(buffer->bufferAndAlloc.size() == MAX_FRAMES_IN_FLIGHT && "Unsported resource dynamic size in function of thier memory usage");

                descriptorBufferInfos[bufferDescriptorCount].buffer = buffer->bufferAndAlloc.at(f).buffer;
                descriptorBufferInfos[bufferDescriptorCount].offset = 0;
                descriptorBufferInfos[bufferDescriptorCount].range = VK_WHOLE_SIZE;
                bufferDescriptorCount++;
            }

            if (std::holds_alternative<PC_CORE::ImageSamplerDescriptor>(des))
            {
                const PC_CORE::ImageSamplerDescriptor* imageSamplerDescriptor = &std::get<PC_CORE::ImageSamplerDescriptor>(des);

                const std::vector<TextureAndAlloc>* textureAndAlloc = static_cast<const std::vector<TextureAndAlloc>*>(imageSamplerDescriptor->texture->GetRhiHandle()->GetNativeHandle());
                const vk::Sampler* samplerHandle = static_cast<const vk::Sampler*>(imageSamplerDescriptor->sampler->GetRhiHandle()->GetNativeHandle());

                descriptorImageInfos[imageDescriptorCount].imageLayout = Vulkan::Utils::RhiImageStateToVulkanImageLayout(imageSamplerDescriptor->imageState);
                descriptorImageInfos[imageDescriptorCount].imageView = textureAndAlloc->at(f).imageView;
                descriptorImageInfos[imageDescriptorCount].sampler = *samplerHandle;
                imageDescriptorCount++;
            }

            if (std::holds_alternative<PC_CORE::ImageDescriptor>(des))
            {
                const PC_CORE::ImageDescriptor* imageDescriptor = &std::get<PC_CORE::ImageDescriptor>(des);

                const std::vector<TextureAndAlloc>* textureAndAlloc = static_cast<const std::vector<TextureAndAlloc>*>(imageDescriptor->texture->GetRhiHandle()->GetNativeHandle());

                descriptorImageInfos[imageDescriptorCount].imageLayout = Vulkan::Utils::RhiImageStateToVulkanImageLayout(imageDescriptor->imageState);
                descriptorImageInfos[imageDescriptorCount].imageView = textureAndAlloc->at(f).imageView;
                imageDescriptorCount++;
            }

            if (std::holds_alternative<PC_CORE::InputAttachementDescriptor>(des))
            {
                const PC_CORE::InputAttachementDescriptor* inputAttachementDescriptor = &std::get<PC_CORE::InputAttachementDescriptor>(des);
                const std::vector<TextureAndAlloc>* textureAndAlloc = static_cast<const std::vector<TextureAndAlloc>*>(inputAttachementDescriptor->image->GetRhiHandle()->GetNativeHandle());
                const vk::Sampler* samplerHandle = static_cast<const vk::Sampler*>(inputAttachementDescriptor->image->GetRhiHandle()->GetNativeHandle());

                descriptorImageInfos[imageDescriptorCount].imageLayout = Vulkan::Utils::RhiImageStateToVulkanImageLayout(inputAttachementDescriptor->imageState);
                descriptorImageInfos[imageDescriptorCount].imageView = textureAndAlloc->at(f).imageView;
                descriptorImageInfos[imageDescriptorCount].sampler = *samplerHandle;
                imageDescriptorCount++;
            }
        }
    }
    
    // Reset counters before descriptor writes
  

    std::vector<vk::WriteDescriptorSet> descriptorWrites(_shaderProgramDescriptorSet.size() * m_DescriptorSets.size());
    

    size_t descriptorWriteOffset = 0;
    bufferDescriptorCount = 0;
    imageDescriptorCount = 0;

    for (size_t f = 0; f < m_DescriptorSets.size(); f++)
    {
        
        
        for (size_t i = 0; i < _shaderProgramDescriptorSet.size(); i++)
        {
            size_t descriptorWriteIndex = i + descriptorWriteOffset;
            descriptorWrites[descriptorWriteIndex].sType = vk::StructureType::eWriteDescriptorSet;
            descriptorWrites[descriptorWriteIndex].dstBinding = _shaderProgramDescriptorSet.at(i).bindingIndex;
            descriptorWrites[descriptorWriteIndex].dstArrayElement = 0;
            descriptorWrites[descriptorWriteIndex].descriptorType = Utils::RhiToDescriptorType(
                _shaderProgramDescriptorSet.at(i).shaderProgramDescriptorType);
            descriptorWrites[descriptorWriteIndex].descriptorCount = 1;

            switch (_shaderProgramDescriptorSet.at(i).shaderProgramDescriptorType)
            {
            case PC_CORE::ShaderProgramDescriptorType::UniformBuffer:
            case PC_CORE::ShaderProgramDescriptorType::StorageBuffer:;
                descriptorWrites[descriptorWriteIndex].pBufferInfo = &descriptorBufferInfos[bufferDescriptorCount];
                bufferDescriptorCount++;
                break;
            case PC_CORE::ShaderProgramDescriptorType::CombinedImageSampler:
                descriptorWrites[descriptorWriteIndex].pImageInfo = &descriptorImageInfos[imageDescriptorCount];
                imageDescriptorCount++;
                break;
            case PC_CORE::ShaderProgramDescriptorType::StorageImage:
                descriptorWrites[descriptorWriteIndex].pImageInfo = &descriptorImageInfos[imageDescriptorCount];
                imageDescriptorCount++;
                break;
            case PC_CORE::ShaderProgramDescriptorType::InputAttachment:
                descriptorWrites[descriptorWriteIndex].pImageInfo = &descriptorImageInfos[imageDescriptorCount];
                imageDescriptorCount++;
                break;
            default:
                assert(false && "Unsupported shader program descriptor type");
            }
        }
        descriptorWriteOffset += _shaderProgramDescriptorSet.size();    
        
    }
    size_t descritproWriteCount = _shaderProgramDescriptorSet.size();
    size_t descritionWriteOffset = 0;


    for (size_t i = 0; i < m_DescriptorSets.size(); i++)
    {
        for (size_t j = 0; j < _shaderProgramDescriptorSet.size(); j++)
        {
            descriptorWrites[descritionWriteOffset].dstSet = m_DescriptorSets[i];
            descritionWriteOffset++;
        }
        device.updateDescriptorSets(static_cast<uint32_t>(descritproWriteCount), descriptorWrites.data() + descritproWriteCount * i, 0, nullptr);
    }
    
}

Vulkan::VulkanDescriptorSets::VulkanDescriptorSets(vk::DescriptorPool _descitptorPool, vk::DescriptorSetAllocateInfo _vkDescriptorSetAllocateInfo) : m_DescitptorPool(_descitptorPool)
{
    vk::Device d = GET_VK_DEVICE->GetDevice();

    PC_LOG_VERBOSE("CreateDescritptor Set");
    VK_CALL(d.allocateDescriptorSets(&_vkDescriptorSetAllocateInfo, m_DescriptorSets.data()));
}

Vulkan::VulkanDescriptorSets::~VulkanDescriptorSets()
{
    if (m_DescitptorPool == VK_NULL_HANDLE)
        return;

    vk::Device d = GET_VK_DEVICE->GetDevice();

    d.freeDescriptorSets(m_DescitptorPool, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT), m_DescriptorSets.data());
}

const void* Vulkan::VulkanDescriptorSets::GetNativeHandle() const
{
    return &m_DescriptorSets;
}

void* Vulkan::VulkanDescriptorSets::GetNativeHandle()
{
    return &m_DescriptorSets;
}
