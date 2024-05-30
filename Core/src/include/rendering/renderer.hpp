#pragma once
#include "vulkan/vulkan_interface.hpp"
#include <GLFW/glfw3.h>


BEGIN_PCCORE

class Renderer
{
public:
    void Init(GLFWwindow* _window); 

    void Destroy();

private:
    VulkanInterface m_VulkanInterface = {};

    std::vector<VkCommandBuffer> m_CommandBuffers;

    void BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags);

    void RecordCommandBuffers();
};

END_PCCORE