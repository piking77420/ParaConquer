#include "resources/vulkan_descriptor_sets.hpp"

#include "rhi_vulkan_parser.hpp"
#include "vulkan_device.hpp"
#include "handles/vulkan_buffer_handle.hpp"
#include "handles/vulkan_image_handle.hpp"
#include "low_renderer/gpu_buffer.hpp"
#include "low_renderer/rhi.hpp"
#include "low_renderer/uniform_buffer.hpp"
#include "resources/vulkan_sampler.hpp"

void Vulkan::VulkanDescriptorSets::WriteDescriptorSets(
    const std::vector<PC_CORE::ShaderProgramDescriptorWrite>& _shaderProgramDescriptorSet)
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->
        GetDevice();
    // Ensure descriptorSets and bufferHandles are correctly sized
    assert(
        descriptorSets.size() == _shaderProgramDescriptorSet.at(0).uniformBufferDescriptor->buffer->bufferHandles.size(
        ));

    size_t bufferDescriptorCount = 0;
    size_t imageDescriptorCount = 0;

    // Count descriptors
    for (size_t i = 0; i < _shaderProgramDescriptorSet.size(); i++)
    {
        if (_shaderProgramDescriptorSet[i].uniformBufferDescriptor != nullptr)
            bufferDescriptorCount++;
        if (_shaderProgramDescriptorSet[i].imageSamperDescriptor != nullptr)
            imageDescriptorCount++;
    }

    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos(bufferDescriptorCount);
    std::vector<vk::DescriptorImageInfo> descriptorImageInfos(imageDescriptorCount);

    // Reset counters
    bufferDescriptorCount = 0;
    imageDescriptorCount = 0;

    for (size_t i = 0; i < _shaderProgramDescriptorSet.size(); i++)
    {
        if (_shaderProgramDescriptorSet[i].uniformBufferDescriptor != nullptr)
        {
            PC_CORE::UniformBufferDescriptor* uniformBufferDescriptor = _shaderProgramDescriptorSet.at(i).
                uniformBufferDescriptor;
            PC_CORE::GpuBuffer* bufferHandle = uniformBufferDescriptor->buffer;
            VulkanBufferHandle* vulkanBufferHandle = reinterpret_cast<VulkanBufferHandle*>(bufferHandle->bufferHandles.
                at(i).get());

            descriptorBufferInfos[bufferDescriptorCount].buffer = vulkanBufferHandle->buffer;
            descriptorBufferInfos[bufferDescriptorCount].offset = 0;
            descriptorBufferInfos[bufferDescriptorCount].range = VK_WHOLE_SIZE;
            bufferDescriptorCount++;
        }

        if (_shaderProgramDescriptorSet[i].imageSamperDescriptor != nullptr)
        {
            PC_CORE::ImageSamperDescriptor* imageSamplerDescriptor = _shaderProgramDescriptorSet.at(i).
                imageSamperDescriptor;

            PC_CORE::GpuHandle* imageHandle = imageSamplerDescriptor->imageHandle;
            PC_CORE::Sampler* samplerHandle = imageSamplerDescriptor->sampler;

            VulkanImageHandle* vulkanImageHandle = reinterpret_cast<VulkanImageHandle*>(imageHandle);
            VulkanSampler* vulkanSampler = reinterpret_cast<VulkanSampler*>(samplerHandle);

            descriptorImageInfos[imageDescriptorCount].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            descriptorImageInfos[imageDescriptorCount].imageView = vulkanImageHandle->view;
            descriptorImageInfos[imageDescriptorCount].sampler = vulkanSampler->GetSampler();
            imageDescriptorCount++;
        }
    }

    // Reset counters before descriptor writes
    bufferDescriptorCount = 0;
    imageDescriptorCount = 0;

    std::vector<vk::WriteDescriptorSet> descriptorWrites(_shaderProgramDescriptorSet.size());
        for (size_t i = 0; i < descriptorWrites.size(); i++)
        {
            descriptorWrites[i].sType = vk::StructureType::eWriteDescriptorSet;
            descriptorWrites[i].dstBinding = _shaderProgramDescriptorSet.at(i).bindingIndex;
            descriptorWrites[i].dstArrayElement = 0;
            descriptorWrites[i].descriptorType = RhiToDescriptorType(
                _shaderProgramDescriptorSet.at(i).shaderProgramDescriptorType);
            descriptorWrites[i].descriptorCount = 1;

            switch (_shaderProgramDescriptorSet.at(i).shaderProgramDescriptorType)
            {
            case PC_CORE::ShaderProgramDescriptorType::UniformBuffer:
                descriptorWrites[i].pBufferInfo = &descriptorBufferInfos[bufferDescriptorCount];
                bufferDescriptorCount++;
                break;
            case PC_CORE::ShaderProgramDescriptorType::CombineImageSampler:
                descriptorWrites[i].pImageInfo = &descriptorImageInfos[imageDescriptorCount];
                imageDescriptorCount++;
                break;
            }
        }

    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        for (size_t j = 0; j < descriptorWrites.size(); j++)
        {
            descriptorWrites[j].dstSet = descriptorSets[i];
        }
        device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
    
     
    
    
}
