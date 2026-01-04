#include "VulkanDescritptorManager.hpp"

#include <map>

#include "VulkanContext.hpp"
#include "VulkanDescriptorSet.hpp"

Vulkan::VulkanDescritptorManager::VulkanDescritptorManager(VulkanContext& _Context)
    : m_Context(_Context)
{

}
