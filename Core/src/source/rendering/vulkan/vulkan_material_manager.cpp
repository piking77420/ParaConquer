#include "rendering\vulkan\vulkan_material_manager.hpp"

#include "rendering/gpu_typedef.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "resources/material.hpp"

using namespace PC_CORE;

void VulkanMaterialManager::Init()
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding = VulkanTextureSampler::GetDescriptorSetLayoutBinding(0,1,VK_SHADER_STAGE_FRAGMENT_BIT);
    VkDescriptorSetLayoutBinding materialUniform = VulkanUniformBuffer::GetLayoutBinding(1,1,VK_SHADER_STAGE_FRAGMENT_BIT , nullptr , true);

    descriptorSetLayout.Init({samplerLayoutBinding ,materialUniform});
    vulkanDescriptorPool.Init(descriptorSetLayout.GetLayoutBinding(), MAX_MATERIAL_COUNT);
}

void VulkanMaterialManager::CreateMaterial(const Material& material)
{
    LowLevelMaterial lowLevelMaterial;
    lowLevelMaterial.descriptorSets.resize(VulkanInterface::GetNbrOfImage());
    
    vulkanDescriptorPool.CreateDescriptorSet(descriptorSetLayout.Get(), lowLevelMaterial.descriptorSets.size(),lowLevelMaterial.descriptorSets.data());
    
    std::vector<VkWriteDescriptorSet> descriptorWrites;
    descriptorWrites.resize(1);
    
    for (const VkDescriptorSet& descriptorSet : lowLevelMaterial.descriptorSets)
    {
        for (VkWriteDescriptorSet& vkWriteDescriptorSet : descriptorWrites)
        {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = material.albedo->vulkanTexture.textureImageView;
            imageInfo.sampler = VulkanInterface::vulkanTextureSampler.Get(material.albedo->vulkanTexture.samplerId);
    
            vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            vkWriteDescriptorSet.dstSet = descriptorSet;
            vkWriteDescriptorSet.dstBinding = 0;
            vkWriteDescriptorSet.dstArrayElement = 0;
            vkWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            vkWriteDescriptorSet.descriptorCount = 1;
            vkWriteDescriptorSet.pImageInfo = &imageInfo;
            
        }
        vkUpdateDescriptorSets(VulkanInterface::GetDevice().device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
    
    m_MaterialCache.insert({material.guid,lowLevelMaterial});
}

void VulkanMaterialManager::BindMaterialDescriptorSet(VkCommandBuffer _commandBuffer,uint32_t _firstSet,
        const Material& material , VkPipelineLayout _VkPipelineLayout)
{
    if (!m_MaterialCache.contains(material.guid))
        return;
    
    const std::vector<VkDescriptorSet>& descriptorSets =  m_MaterialCache.at(material.guid).descriptorSets;

    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _VkPipelineLayout,_firstSet ,
    1, &descriptorSets[VulkanInterface::GetCurrentFrame()], 0, nullptr);

}

void VulkanMaterialManager::Destroy()
{
    m_MaterialCache.clear();
    vulkanDescriptorPool.Destroy();
    descriptorSetLayout.Destroy();
}

