#include "resources/vulkan_descriptor_sets.hpp"

#include "vulkan_device.hpp"
#include "buffer/vulkan_buffer_handle.hpp"
#include "low_renderer/gpu_buffer.hpp"
#include "low_renderer/rhi.hpp"

void Vulkan::VulkanDescriptorSets::WriteDescriptorSets(std::vector<PC_CORE::ShaderProgramDescriptorWrite> _shaderProgramDescriptorSet)
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->GetDevice();

    for (uint32_t i = 0; i < descriptorSets.size(); i++)
    {
        std::vector<vk::WriteDescriptorSet> descriptorWrites(_shaderProgramDescriptorSet.size());

        for (size_t j = 0; j < descriptorWrites.size(); j++)
        {
            PC_CORE::GpuBuffer* gpuBuffer = static_cast<PC_CORE::GpuBuffer*>(_shaderProgramDescriptorSet.at(j).ptr);
            vk::Buffer buffer = std::reinterpret_pointer_cast<VulkanBufferHandle>(gpuBuffer->GetHandle())->buffer;
            
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;


            vk::WriteDescriptorSet& vkWriteDescriptorSet = descriptorWrites[j];
            vkWriteDescriptorSet.sType = vk::StructureType::eWriteDescriptorSet;
            vkWriteDescriptorSet.dstSet = descriptorSets[i];
            vkWriteDescriptorSet.dstBinding = _shaderProgramDescriptorSet[j].bindingIndex;
            vkWriteDescriptorSet.dstArrayElement = 0;
            vkWriteDescriptorSet.descriptorType = vk::DescriptorType::eUniformBuffer;
            vkWriteDescriptorSet.descriptorCount = 1;
            vkWriteDescriptorSet.pBufferInfo = &bufferInfo;
            
        }
        device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),0 , nullptr );
    }

    
}
