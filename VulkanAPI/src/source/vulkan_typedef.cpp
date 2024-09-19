#include "vulkan_typedef.h"

using namespace VK_NP;

VulkanContext* VulkanContext::currentContext = nullptr;

VulkanContext::VulkanContext()
{
    currentContext = this;
}
