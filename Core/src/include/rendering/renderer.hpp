﻿#pragma once
#include "vulkan/vulkan_interface.hpp"
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "draw_gizmos.hpp"
#include "gpu_typedef.hpp"
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
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"


BEGIN_PCCORE
class  Renderer
{
public:
    void Init(Window* _window);

    void RecreateSwapChain(Window* _window);

    void Destroy();

    void RenderViewPort(const Camera& _camera,const World& _world);

private:
    std::vector<VkCommandBuffer> m_CommandBuffers;
    
    VulkanPipeline m_BasePipeline;
    
    VulkankPipelineLayout m_VkPipelineLayout;

    VulkanDescriptorSetLayout m_DescriptorSetLayout;

    VulkanDescriptorPool m_DescriptorPool;

    VulkanShaderStage m_VulkanShaderStage;

    CameraBuffer cameraBuffer;
    
    std::vector<VulkanUniformBuffer> m_UniformBuffers;

    std::vector<VulkanShaderStorageBuffer> m_ModelMatriciesShaderStorages;
    
    std::vector<VulkanShaderStorageBuffer> m_ShaderStoragesLight;

    std::vector<MatrixMeshes> m_MatrixMeshs;

    GpuLight* m_GpuLights = nullptr;

    std::vector<VkDescriptorSet> descriptorSets;
    
    std::vector<VulkanSemaphore> m_ImageAvailableSemaphore;
    
    std::vector<VulkanSemaphore> m_RenderFinishedSemaphore;
    
    std::vector<VulkanFence> m_InFlightFence;
    
    const Camera* m_CurrentCamera = nullptr;
    
    const World* m_CurrentWorld = nullptr;

    DrawGizmos drawGizmos;

    void BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags);

    void RecordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void ForwardPass(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void CreateBasicGraphiPipeline();

    void CreateAsyncObject();

    void DestroyAsyncObject();

    void CreateDescriptorSetLayout();

    void UpdateBuffers(uint32_t _currentFrame);

    void UpdateLightBuffer(uint32_t _currentFrame);
    
    void CreateDescriptorSets();

    void ComputeModelAndNormalInvertMatrix(uint32_t _currentFrame);

    void DrawStatisMesh(VkCommandBuffer commandBuffer, uint32_t imageIndex, const StaticMesh& staticMesh,
    const Transform& transform, const Entity& entity);

    void InitBuffers();

    friend DrawGizmos;
};

END_PCCORE