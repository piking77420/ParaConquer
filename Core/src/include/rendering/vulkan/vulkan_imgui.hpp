#pragma once
#include "vulkan_header.h"
#include "vulkan_descriptor_pool.hpp"
#include "window.hpp"

BEGIN_PCCORE

class VulkanImgui
{
public:
    void Init(const Window& _window);

    void Destroy();

    void NewFrame() const;

    void EndFrame() const;

    static void Render(VkCommandBuffer* _commandBuffer);
private:
    void Theme();

    void DockSpace() const;

    void EndDockSpace() const;

    static void CheckErrorImgui(VkResult _err);
    
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
};

END_PCCORE