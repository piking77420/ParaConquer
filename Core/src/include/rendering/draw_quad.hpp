#pragma once

#include "core_header.hpp"
#include "resources/mesh.hpp"
#include "vulkan/vulkan_descriptor_pool.hpp"
#include "vulkan/vulkan_descriptor_set_layout.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_pipeline_layout.hpp"
#include "vulkan/vulkan_shader_stage.hpp"
#include "vulkan/vulkan_texture.hpp"

BEGIN_PCCORE
    class Renderer;

class DrawQuad
{
public:
    static constexpr uint32_t MaxDrawQuadAlloc = 100;
    
    const Renderer* m_Renderer = nullptr;

    void Init(Renderer* renderer);

    void Draw(VkCommandBuffer _commandBuffer, VkDescriptorSet _descriptorSet);

    void Destroy();

    VkDescriptorSet DrawQuadAddTexture(const VulkanTexture& _vulkanTexture) const;

    VkDescriptorSet DrawQuadAddTexture(const VkImageView& imageView) const;

    void FreeDescriptorSet(VkDescriptorSet _vkDescriptorSet) const; 

private:
    VulkanShaderStage m_VulkanShaderStage;
    
    VulkanPipeline m_Pipeline;
    
    VulkankPipelineLayout m_VkPipelineLayout;

    VulkanDescriptorSetLayout m_DescriptorSetLayout;

    VulkanDescriptorPool m_DescriptorPool;

    const Mesh* quad = nullptr;

    void InitShader();
    
    void CreateGraphiPipeline();
    
    void InitDescriptor();
};

END_PCCORE