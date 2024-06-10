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

    const std::vector<VkDescriptorSetLayoutBinding>& GetLayoutBinding() const;

private:
    VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
    
    std::vector<VkDescriptorSetLayoutBinding> m_DesciptorSetLayoutBinding;
    
    void InitDescriptorLayout();
};

END_PCCORE