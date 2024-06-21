#pragma once
#include <map>

#include "core_header.hpp"
#include "rendering/skydom.hpp"
#include "rendering/vulkan/vulkan_descriptor_pool.hpp"
#include "rendering/vulkan/vulkan_descriptor_set_layout.hpp"
#include "rendering/vulkan/vulkan_pipeline.hpp"
#include "rendering/vulkan/vulkan_pipeline_layout.hpp"
#include "rendering/vulkan/vulkan_shader_stage.hpp"
#include "rendering/vulkan/vulkan_texture.hpp"
#include "resources/mesh.hpp"
#include "world/skybox.hpp"

BEGIN_PCCORE
    class Renderer;

class SkyboxRender
{   
public:
    void CreateSkyboxBackend(const Skybox& skybox);

    void Init(Renderer* _renderer);

    void DrawSkybox(VkCommandBuffer commandBuffer, const Skybox& skybox);

    void Destroy();

private:
    struct SkyBoxCubMapBackend
    {
        VulkanShaderStage vulkanShaderStageSkyBox;
    
        VulkanPipeline skyBoxPipeline;
    
        VulkankPipelineLayout vkPipelineLayout;

        VulkanDescriptorSetLayout descriptorSetLayout;

        VulkanDescriptorPool descriptorPool;
    
        std::vector<VkDescriptorSet> descriptorSetMap;
        
        Mesh* cubeMesh = nullptr;
    };

    struct SkyBoxProceduralBackend
    {
        VulkanShaderStage vulkanShaderStageSkyBox;
    
        VulkanPipeline skyBoxPipeline;
    
        VulkankPipelineLayout vkPipelineLayout;

        VulkanDescriptorSetLayout descriptorSetLayout;

        VulkanDescriptorPool descriptorPool;
    
        std::vector<VkDescriptorSet> descriptorSet;

        Mesh* quad = nullptr;

        Skydom skydom;

    };

    SkyBoxProceduralBackend skyBoxProceduralBackend;
    
    SkyBoxCubMapBackend m_SkyBoxCubMapBackend;

    Renderer* m_Renderer = nullptr;

    // CubeMap //
    
    void InitCubeMapBackend();

    void DestroyCubeMapBackend();

    void DrawSkyboxCubemap(VkCommandBuffer commandBuffer);

    void CreateCubemapDescriptorSetBackend(const Skybox& skybox);

    // Procedural // 
    void InitProceduralBackend();

    void DestroyProceduralSkyboxBackend();

    void DrawSkyboxProcedural(VkCommandBuffer commandBuffer);

    void CreateProcuralDescriptorSetBackend(const Skybox& skybox);

};

END_PCCORE