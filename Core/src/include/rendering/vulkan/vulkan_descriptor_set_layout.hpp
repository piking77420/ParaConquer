#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanDescriptorSetLayout
{
public:
    void Init(const std::vector<VkDescriptorSetLayoutBinding>& _vkDescriptorSetLayoutBindings);

    void CreateDesciptorPool(VkDescriptorPool* descriptorPool);

    void Destroy();

    const VkDescriptorSetLayout& Get() const;

    VkDescriptorSetLayout& Get();

    void CreateDescriptorSet(VkDescriptorPool vkDescriptorPool,uint32_t _nbr, VkDescriptorSet* _data) const;
    
private:
    
    VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;

    std::vector<VkDescriptorSetLayoutBinding> m_desciptorSetLayoutBinding;
    
    void InitDescriptorLayout();
};

END_PCCORE