#include "resources/vulkan_descriptor_sets.hpp"

#include "rhi_vulkan_parser.hpp"
#include "vulkan_device.hpp"
#include "handles/vulkan_buffer_handle.hpp"
#include "low_renderer/gpu_buffer.hpp"
#include "low_renderer/rhi.hpp"
#include "low_renderer/uniform_buffer.hpp"

void Vulkan::VulkanDescriptorSets::WriteDescriptorSets(const std::vector<PC_CORE::ShaderProgramDescriptorWrite>& _shaderProgramDescriptorSet)
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->GetDevice();

    for (uint32_t i = 0; i < descriptorSets.size(); i++)
    {
        std::vector<vk::WriteDescriptorSet> descriptorWrites(_shaderProgramDescriptorSet.size());

        uint32_t bufferCount = 0;
        uint32_t imageCount = 0;

        for (size_t j = 0; j < descriptorWrites.size(); j++)
        {
            vk::DescriptorType descriptor = RhiToDescriptorType(_shaderProgramDescriptorSet[j].shaderProgramDescriptorType);

            if (descriptor == vk::DescriptorType::eUniformBuffer || descriptor == vk::DescriptorType::eStorageBuffer)
            {
                bufferCount++;
            }
            else if (descriptor == vk::DescriptorType::eSampler || descriptor == vk::DescriptorType::eSampledImage)
            {
                imageCount++;
            }
        }

        std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos(bufferCount);
        std::vector<vk::DescriptorImageInfo> descriptorImageInfos(imageCount);
    
        for (size_t j = 0; j < descriptorWrites.size(); j++)
        {
            ManagerDescriptor(_shaderProgramDescriptorSet.at(j), &descriptorBufferInfos, &descriptorImageInfos, j);
        }
        const uint32_t descriptorCount = bufferCount + imageCount;
        
        for (size_t j = 0; j < descriptorWrites.size(); j++)
        {
            vk::WriteDescriptorSet& vkWriteDescriptorSet = descriptorWrites[j];
            vkWriteDescriptorSet.sType = vk::StructureType::eWriteDescriptorSet;
            vkWriteDescriptorSet.dstSet = descriptorSets[i];
            vkWriteDescriptorSet.dstBinding = _shaderProgramDescriptorSet[j].bindingIndex;
            vkWriteDescriptorSet.dstArrayElement = 0;
            vkWriteDescriptorSet.descriptorCount = descriptorCount;
            if (!descriptorImageInfos.empty())
                vkWriteDescriptorSet.pBufferInfo = descriptorBufferInfos.data();
            if (!descriptorImageInfos.empty())
                vkWriteDescriptorSet.pImageInfo = descriptorImageInfos.data();
        }
       
        
        device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),0 , nullptr );
    }

    
}

void Vulkan::VulkanDescriptorSets::ManagerDescriptor(
    const PC_CORE::ShaderProgramDescriptorWrite& _ShaderProgramDescriptorWrite,
    std::vector<vk::DescriptorBufferInfo>* descriptorBufferInfos,
    std::vector<vk::DescriptorImageInfo>* descriptorImageInfo, size_t _descirptorIndex)
{

    vk::DescriptorType descriptorType = RhiToDescriptorType(_ShaderProgramDescriptorWrite.shaderProgramDescriptorType);
    
    if (descriptorType == vk::DescriptorType::eUniformBuffer || descriptorType == vk::DescriptorType::eStorageBuffer)
    {
        if (descriptorType == vk::DescriptorType::eUniformBuffer)
        {
            PC_CORE::UniformBuffer* uniformBuffer = static_cast<PC_CORE::UniformBuffer*>(_ShaderProgramDescriptorWrite.object);
            vk::DescriptorBufferInfo& bufferInfo = descriptorBufferInfos->at(_descirptorIndex);
            bufferInfo.buffer = reinterpret_cast<VulkanBufferHandle*>(uniformBuffer->GetHandle()->get())->buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;
        }
        
 
    }
    else if (descriptorType == vk::DescriptorType::eSampler || descriptorType == vk::DescriptorType::eSampledImage)
    {
        vk::DescriptorImageInfo& vkDescriptorImageInfo = descriptorImageInfo->at(_descirptorIndex);
        vkDescriptorImageInfo.sampler = VK_NULL_HANDLE;
        vkDescriptorImageInfo.imageView = VK_NULL_HANDLE;
        vkDescriptorImageInfo.sampler = VK_NULL_HANDLE;
    }
    
    
}
