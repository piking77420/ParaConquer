﻿#pragma once
#include "vulkan/vulkan_interface.hpp"
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "draw_gizmos.hpp"
#include "draw_quad.hpp"
#include "gpu_typedef.hpp"
#include "../../source/rendering/skybox_render.h"
#include "resources/mesh.hpp"
#include "resources/texture.hpp"
#include "vulkan/vulkan_descriptor_pool.hpp"
#include "vulkan\vulkan_vertex_buffer.hpp"
#include "vulkan/vulkan_descriptor_set_layout.hpp"
#include "vulkan/vulkan_fence.hpp"
#include "vulkan/vulkan_index_buffer.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_pipeline_layout.hpp"
#include "vulkan/vulkan_render_pass.hpp"
#include "vulkan/vulkan_semaphore.h"
#include "vulkan/vulkan_shader_stage.hpp"
#include "vulkan/vulkan_shader_storage_buffer.hpp"
#include "vulkan/vulkan_uniform_buffer.h"
#include "vulkan/vulkan_viewport.hpp"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"

BEGIN_PCCORE

class Renderer
{
public:
    DrawQuad drawQuad;

    SkyboxRender skyboxRender;

    VulkanViewport vulkanViewport;
    
    std::array<VulkanRenderPass,RenderPass::COUNT> renderPasses;
    
    std::vector<VulkanUniformBuffer> m_CameraBuffers;
    
    void Init(Window* _window);

    void RecreateSwapChain(Window* _window);

    void Destroy();

    void BeginFrame(const World& world);

    void RenderViewPort(const Camera& _camera,
        const uint32_t viewPortId,const World& _world);
        
    void SwapBuffers();

    void WaitGPU() const
    {
        vkDeviceWaitIdle(VulkanInterface::GetDevice().device);
    }
    
    VkCommandBuffer* GetCurrentCommandBuffer()
    {
        return &m_CommandBuffers[VulkanInterface::GetCurrentFrame()];
    }

private:
    struct AsyncObjet
    {
        std::vector<VulkanSemaphore> m_ImageAvailableSemaphore;
        std::vector<VulkanSemaphore> m_RenderFinishedSemaphore;
        std::vector<VulkanFence> m_InFlightFence;
    };
    uint32_t m_ImageIndex = 0;

    AsyncObjet asyncObjet;
    
    std::vector<VkCommandBuffer> m_CommandBuffers;

    std::vector<VkCommandBuffer> m_ForwardCommandBuffers;

    VulkanCommandPool fwdCommandPool;
    
    VulkanPipeline m_BasePipeline;
    
    VulkankPipelineLayout m_VkPipelineLayout;

    VulkanDescriptorSetLayout m_DescriptorSetLayout;

    VulkanDescriptorPool m_DescriptorPool;

    VulkanShaderStage m_VulkanShaderStage;

    CameraBuffer cameraBuffer;
    
    std::vector<VulkanShaderStorageBuffer> m_ModelMatriciesShaderStorages;
    
    std::vector<VulkanShaderStorageBuffer> m_ShaderStoragesLight;
    
    std::vector<VkDescriptorSet> descriptorSets;
    
    GpuLight m_GpuLights;
    
    const Camera* m_CurrentCamera = nullptr;
    
    const World* m_CurrentWorld = nullptr;

    const ViewPort* m_CurrentViewport = nullptr;
    
    DrawGizmos drawGizmos;

    void RenderSwapChain();
    
    void InitRenderPasses();

    void BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags);
    
    void ForwardPass(VkCommandBuffer commandBuffer);
    
    void CreateBasicGraphiPipeline();

    void CreateAsyncObject();

    void DestroyAsyncObject();

    void CreateDescriptorSetLayout();

    void UpdateCameraBuffer(uint32_t _currentFrame);

    void UpdateLightBuffer(uint32_t _currentFrame);
    
    void CreateDescriptorSets();

    void ComputeModelAndNormalInvertMatrix(uint32_t _currentFrame);

    void DrawStatisMesh(VkCommandBuffer commandBuffer, uint32_t imageIndex, const StaticMesh& staticMesh,
    const Transform& transform, const Entity& entity);

    void UpdateWorldBuffers();

    void InitBuffers();
    
    friend DrawGizmos;
};

END_PCCORE