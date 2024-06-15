#pragma once
#include "core_header.hpp"
#include "rendering/vulkan/vulkan_descriptor_pool.hpp"
#include "rendering/vulkan/vulkan_descriptor_set_layout.hpp"
#include "rendering/vulkan/vulkan_pipeline.hpp"
#include "rendering/vulkan/vulkan_pipeline_layout.hpp"
#include "rendering/vulkan/vulkan_shader_stage.hpp"

BEGIN_PCCORE
    class Renderer;

class SkyboxRender
{
public:
    void Init(Renderer* _renderer);

    void UpdateDescritptorSet(uint32_t size, VkDescriptorSet* descriptorSet);

private:
    VulkanShaderStage m_VulkanShaderStageSkyBox;
    
    VulkanPipeline m_SkyBoxPipeline;
    
    VulkankPipelineLayout m_VkPipelineLayout;

    VulkanDescriptorSetLayout m_DescriptorSetLayout;

    VulkanDescriptorPool m_DescriptorPool;
    
    Renderer* m_Renderer = nullptr;

};

END_PCCORE