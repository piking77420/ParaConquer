#include "vulkan_typedef.h"


#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vma/vk_mem_alloc.h>

using namespace VK_NP;

VulkanContext* VulkanContext::currentContext = nullptr;

VulkanContext::VulkanContext()
{
    currentContext = this;
}
