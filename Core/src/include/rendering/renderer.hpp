#pragma once
#include "vulkan/vulkan_interface.hpp"
#include <GLFW/glfw3.h>

#include "vulkan/vulkan_command_buffer.hpp"
#include "vulkan/vulkan_fence.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_render_pass.hpp"
#include "vulkan/vulkan_semaphore.h"
#include "vulkan/vulkan_shader_stage.hpp"


BEGIN_PCCORE
class Renderer
{
public:
    void Init(GLFWwindow* _window); 

    void Destroy();

    void RenderViewPort();

private:
    VulkanInterface m_VulkanInterface = {};

    VulkanCommandBuffer m_CommandBuffers;
    
    VulkanPipeline m_BasePipeline;
    
    VkPipelineLayout VkPipelineLayout = VK_NULL_HANDLE;

    VulkanShaderStage m_VulkanShaderStage;

    VulkanRenderPass m_VulkanRenderPass;

    VulkanSemaphore m_ImageAvailableSemaphore;
    
    VulkanSemaphore m_RenderFinishedSemaphore;
    
    VulkanFence m_InFlightFence;

    void BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags);

    void RecordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void CreateBasicGraphiPipeline();

    void CreateAsyncObject();

    void DestroyAsyncObject();
};

END_PCCORE