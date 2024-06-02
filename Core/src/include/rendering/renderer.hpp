#pragma once
#include "vulkan/vulkan_interface.hpp"
#include <GLFW/glfw3.h>

#include "gpu_typedef.hpp"
#include "resources/mesh.hpp"
#include "vulkan\vulkan_vertex_buffer.hpp"
#include "vulkan/vulkan_command_buffer.hpp"
#include "vulkan/vulkan_descriptor_set_layout.hpp"
#include "vulkan/vulkan_fence.hpp"
#include "vulkan/vulkan_index_buffer.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_pipeline_layout.hpp"
#include "vulkan/vulkan_render_pass.hpp"
#include "vulkan/vulkan_semaphore.h"
#include "vulkan/vulkan_shader_stage.hpp"
#include "vulkan/vulkan_uniform_buffer.h"


BEGIN_PCCORE
    class Renderer
{
public:
    void Init(Window* _window);

    void RecreateSwapChain(Window* _window);

    void Destroy();

    void RenderViewPort();

private:
    VulkanInterface m_VulkanInterface = {};

    VulkanCommandBuffer m_CommandBuffers;
    
    VulkanPipeline m_BasePipeline;
    
    VulkankPipelineLayout m_VkPipelineLayout;

    VulkanDescriptorSetLayout m_DescriptorSetLayout;

    VulkanShaderStage m_VulkanShaderStage;

    UniformBufferObject UniformBufferObject;

    VkDescriptorPool descriptorPool;
    
    std::vector<VulkanUniformBuffer> m_UniformBuffers;

    std::vector<VkDescriptorSet> descriptorSets;
    
    std::vector<VulkanSemaphore> m_ImageAvailableSemaphore;
    
    std::vector<VulkanSemaphore> m_RenderFinishedSemaphore;
    
    std::vector<VulkanFence> m_InFlightFence;

    VulkanIndexBuffer vulkanIndexBuffer;
    
    VulkanVertexBuffer vulkanVertexBuffer;

    void BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags);

    void RecordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void CreateBasicGraphiPipeline();

    void CreateAsyncObject();

    void DestroyAsyncObject();

    void CreateDescriptorSetLayout();

    void UpdateUniformBuffer(uint32_t _currentFrame);

    void CreateDescriptorPool();
    
    void CreateDescriptorSets();
};

END_PCCORE