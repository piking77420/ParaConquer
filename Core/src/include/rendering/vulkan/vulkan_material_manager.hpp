#pragma once

#include <unordered_map>

#include "vulkan_descriptor_pool.hpp"
#include "vulkan_descriptor_set_layout.hpp"
#include "vulkan_header.h"
#include "vulkan_uniform_buffer.h"
#include "resources/material.hpp"

BEGIN_PCCORE
class Material;

class VulkanMaterialManager
{
public:
    
    VulkanDescriptorSetLayout descriptorSetLayout;

    VulkanDescriptorPool vulkanDescriptorPool;
    
    void Init();
    
    void Destroy();

    void CreateMaterial(const Material& material);

    void BindMaterialDescriptorSet(VkCommandBuffer _commandBuffer,uint32_t _firstSet,
        const Material& material , VkPipelineLayout _VkPipelineLayout);

private:
    struct LowLevelMaterial
    {
        std::vector<VulkanUniformBuffer> vulkanUniformBuffer;
        std::vector<VkDescriptorSet> descriptorSets;
    };
    
    std::unordered_map<Guid, LowLevelMaterial> m_MaterialCache;
};

END_PCCORE