#pragma once

#include <unordered_map>

#include "vulkan_descriptor_set_layout.hpp"
#include "vulkan_header.h"
#include "resources/material.hpp"

BEGIN_PCCORE
class Material;

class VulkanMaterialManager
{
public:
    
    VulkanDescriptorSetLayout descriptorSetLayout;
    
    void Init();
    
    void Destroy();

    void CreateMaterial(const Material& material);

    void BindMaterialDescriptorSet(VkCommandBuffer _commandBuffer,
        const Material& material , VkPipelineLayout _VkPipelineLayout);

private:
    struct LowLevelMaterial
    {
        std::vector<VkDescriptorSet> descriptorSets;
        VkDescriptorPool descriptorPool;
    };
    
    std::unordered_map<Guid, LowLevelMaterial> m_MaterialCache;
};

END_PCCORE