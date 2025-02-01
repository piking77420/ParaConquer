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

    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        vk::DescriptorBufferInfo bufferInfo{};

        // Safe pointer retrieval
        PC_CORE::UniformBufferDescriptor* uniformBufferDescriptor = _shaderProgramDescriptorSet.at(0).uniformBufferDescriptor;
        PC_CORE::ImageSamperDescriptor* imageSamplerDescriptor = _shaderProgramDescriptorSet.at(1).imageSamperDescriptor;
        
        PC_CORE::GpuBuffer* bufferHandle = uniformBufferDescriptor->buffer;
        PC_CORE::GpuHandle* imageHandle = imageSamplerDescriptor->imageHandle;
        PC_CORE::Sampler* samplerHandle = imageSamplerDescriptor->sampler;
        
        // Safe cast
        VulkanBufferHandle* vulkanBufferHandle = reinterpret_cast<VulkanBufferHandle*>(bufferHandle->bufferHandles.at(i).
            get());
        VulkanImageHandle* vulkanImageHandle = reinterpret_cast<VulkanImageHandle*>(imageHandle);
        VulkanSampler* vulkanSampler = reinterpret_cast<VulkanSampler*>(samplerHandle);

        if (!vulkanBufferHandle || !vulkanImageHandle || !vulkanSampler) continue;

        bufferInfo.buffer = vulkanBufferHandle->buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        vk::DescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageInfo.imageView = vulkanImageHandle->view;
        imageInfo.sampler = vulkanSampler->GetSampler();

        std::vector<vk::WriteDescriptorSet> descriptorWrites(2);

        descriptorWrites[0].sType = vk::StructureType::eWriteDescriptorSet;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = vk::StructureType::eWriteDescriptorSet;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0,
                                    nullptr);
    }
}
