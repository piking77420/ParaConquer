#pragma once
#include "vulkan/vulkan_interface.hpp"
#include <GLFW/glfw3.h>

#include "vulkan/vulkan_render_pass.hpp"
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

    std::vector<VkCommandBuffer> m_CommandBuffers;
    
    VkPipeline Vkpipeline = VK_NULL_HANDLE;
    
    VkPipelineLayout VkPipelineLayout = VK_NULL_HANDLE;

    VulkanShaderStage m_VulkanShaderStage;

    VulkanRenderPass m_VulkanRenderPass;

    std::vector<VkSemaphore> m_ImageAvailableSemaphore;
    std::vector<VkSemaphore> m_RenderFinishedSemaphore;
    std::vector<VkFence> m_InFlightFence;

    void BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags);

    void RecordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void CreateBasicGraphiPipeline();

    void CreateAsyncObject();

    void DestroyAsyncObject();
};

END_PCCORE