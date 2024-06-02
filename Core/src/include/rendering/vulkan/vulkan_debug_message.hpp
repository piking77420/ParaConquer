#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanDebugMessage
{
public:
    static VkBool32 VulkanMessageDebug(
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData);

    void Init(const VkInstance& instance);
    
    void Destroy(const VkInstance& instance);

private:
    VkDebugUtilsMessengerEXT m_DebugMessage = VK_NULL_HANDLE;

    static const char* GetDebugType(VkDebugUtilsMessageTypeFlagsEXT Type);
    static const char* GetSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT Type);

};

END_PCCORE