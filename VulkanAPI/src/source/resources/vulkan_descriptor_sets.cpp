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
   
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->
        GetDevice();

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

    // Double the size per frame in flight
    imageDescriptorCount = imageDescriptorCount * MAX_FRAMES_IN_FLIGHT;
    bufferDescriptorCount = bufferDescriptorCount * MAX_FRAMES_IN_FLIGHT;

    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos(bufferDescriptorCount);
    std::vector<vk::DescriptorImageInfo> descriptorImageInfos(imageDescriptorCount);

    // Reset counters
    bufferDescriptorCount = 0;
    imageDescriptorCount = 0;
    

    for (size_t f = 0; f < descriptorSets.size(); f++)
    {
        for (size_t i = 0; i < _shaderProgramDescriptorSet.size(); i++)
        {
            assert(!(_shaderProgramDescriptorSet[i].uniformBufferDescriptor != nullptr && _shaderProgramDescriptorSet[i].imageSamperDescriptor != nullptr)); 

            
            if (_shaderProgramDescriptorSet[i].uniformBufferDescriptor != nullptr)
            {
                
                const PC_CORE::UniformBufferDescriptor* uniformBufferDescriptor = _shaderProgramDescriptorSet.at(i).uniformBufferDescriptor;
                
                const std::vector<BufferAndAlloc>* textureAndAlloc = static_cast<const std::vector<BufferAndAlloc>*>(uniformBufferDescriptor->buffer->GetRhiHandle()->GetNativeHandle());

                descriptorBufferInfos[bufferDescriptorCount].buffer = textureAndAlloc->at(f).buffer;
                descriptorBufferInfos[bufferDescriptorCount].offset = 0;
                descriptorBufferInfos[bufferDescriptorCount].range = VK_WHOLE_SIZE;
                bufferDescriptorCount++;
            }
            if (_shaderProgramDescriptorSet[i].imageSamperDescriptor != nullptr)    
            {
                PC_CORE::ImageSamperDescriptor* imageSamplerDescriptor = _shaderProgramDescriptorSet.at(i).
                    imageSamperDescriptor;

                const std::vector<TextureAndAlloc>* textureAndAlloc = static_cast<const std::vector<TextureAndAlloc>*>(imageSamplerDescriptor->texture->GetRhiHandle()->GetNativeHandle());
                const vk::Sampler* samplerHandle = static_cast<const vk::Sampler*>(imageSamplerDescriptor->sampler->GetRhiHandle()->GetNativeHandle());
                
                descriptorImageInfos[imageDescriptorCount].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                descriptorImageInfos[imageDescriptorCount].imageView = textureAndAlloc->at(f).imageView;
                descriptorImageInfos[imageDescriptorCount].sampler = *samplerHandle;
                imageDescriptorCount++;
            }

            
        }
    }
    
    // Reset counters before descriptor writes
  

    std::vector<vk::WriteDescriptorSet> descriptorWrites(_shaderProgramDescriptorSet.size() * descriptorSets.size());
    

    size_t descriptorWriteOffset = 0;
    bufferDescriptorCount = 0;
    imageDescriptorCount = 0;

    for (size_t f = 0; f < descriptorSets.size(); f++)
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
                descriptorWrites[descriptorWriteIndex].pBufferInfo = &descriptorBufferInfos[bufferDescriptorCount];
                bufferDescriptorCount++;
                break;
            case PC_CORE::ShaderProgramDescriptorType::CombineImageSampler:
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


    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        for (size_t j = 0; j < _shaderProgramDescriptorSet.size(); j++)
        {
            descriptorWrites[descritionWriteOffset].dstSet = descriptorSets[i];
            descritionWriteOffset++;
        }
        device.updateDescriptorSets(static_cast<uint32_t>(descritproWriteCount), descriptorWrites.data() + descritproWriteCount * i, 0, nullptr);
    }
    
}
