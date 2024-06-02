#pragma once
#include "vulkan/vulkan_interface.hpp"
#include <GLFW/glfw3.h>

#include "vulkan\vulkan_vertex_buffer.hpp"
#include "vulkan/vulkan_command_buffer.hpp"
#include "vulkan/vulkan_fence.hpp"
#include "vulkan/vulkan_index_buffer.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_render_pass.hpp"
#include "vulkan/vulkan_semaphore.h"
#include "vulkan/vulkan_shader_stage.hpp"


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
    
    VkPipelineLayout VkPipelineLayout = VK_NULL_HANDLE;

    VulkanShaderStage m_VulkanShaderStage;
    
    std::vector<VulkanSemaphore> m_ImageAvailableSemaphore;
    
    std::vector<VulkanSemaphore> m_RenderFinishedSemaphore;
    
    std::vector<VulkanFence> m_InFlightFence;

    VulkanVertexBuffer m_VertexBuffer;

    VulkanIndexBuffer m_IndexBuffer;

    void BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags);

    void RecordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void CreateBasicGraphiPipeline();

    void CreateAsyncObject();

    void DestroyAsyncObject();
};

END_PCCORE