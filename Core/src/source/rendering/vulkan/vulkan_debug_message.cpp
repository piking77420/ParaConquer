#include "rendering/vulkan/vulkan_debug_message.hpp"

#include <cstdio>

#include "log.hpp"

using namespace PC_CORE;

VkBool32 PC_CORE::VulkanDebugMessage::VulkanMessageDebug(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if (!PrintInfo && messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
       return VK_FALSE;
    }
    
    printf("Debug callback: %s\n", pCallbackData->pMessage);
    printf("  Severity %s\n", GetSeverity(messageSeverity));
    printf("  Type %s\n", GetDebugType(messageTypes));
    printf("  Objects ");

    for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
        printf("%llu ", pCallbackData->pObjects[i].objectHandle);
    }

    printf("\n");

    return VK_FALSE;
}

void PC_CORE::VulkanDebugMessage::Init(const VkInstance& instance)
{
    const VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &VulkanMessageDebug,
        .pUserData = nullptr
    };

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
    vkCreateDebugUtilsMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    
    if (!vkCreateDebugUtilsMessenger)
    {
        PC_LOGERROR("Cannot find address of vkCreateDebugUtilsMessenger\n");
        exit(1);
    }

    VkResult res = vkCreateDebugUtilsMessenger(instance, &MessengerCreateInfo, nullptr, &m_DebugMessage);
    VK_CHECK_ERROR(res, "debug utils messenger");
    PC_LOG("Vulkan debugUtilsMessenger created");

    Log::Debug("Debug utils messenger created");
}

void VulkanDebugMessage::Destroy(const VkInstance& instance)
{
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = VK_NULL_HANDLE;
    vkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    
    if (!vkDestroyDebugUtilsMessenger)
    {
        PC_LOGERROR("Cannot find address of vkDestroyDebugUtilsMessengerEXT");
        exit(1);
    }
    vkDestroyDebugUtilsMessenger(instance, m_DebugMessage, nullptr);
}

const char* VulkanDebugMessage::GetDebugType(VkDebugUtilsMessageTypeFlagsEXT Type)
{
    switch (Type) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        return "General";

    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        return "Validation";

    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        return "Performance";

#ifdef _WIN64 // doesn't work on my Linux for some reason
    case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
        return "Device address binding";
#endif

    default:
        PC_LOGERROR("Invalid type code + " + std::to_string(Type) + " %d\n");
        exit(1);
    }

    return "NO SUCH TYPE!";
}

const char* VulkanDebugMessage::GetSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
{
    switch (severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        return "VERBOSE";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        return "INFO";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        return "WARNING";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        return "ERROR";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        break;
    default: ;
    }
    PC_LOGERROR("Invalid Severity code + " + std::to_string(severity) + " %d\n");
    exit(1);
    
}
