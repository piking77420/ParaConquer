#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanDescriptorSetLayout
{
public:
    void Init(const std::vector<VkDescriptorSetLayoutBinding>& _vkDescriptorSetLayoutBindings);

    void Destroy();

    const VkDescriptorSetLayout& Get() const;

    VkDescriptorSetLayout& Get();
private:
    VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
};

END_PCCORE