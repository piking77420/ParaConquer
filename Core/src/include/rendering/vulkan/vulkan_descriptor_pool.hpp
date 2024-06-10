#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanDescriptorPool
{
public:
    void Init(const std::vector<VkDescriptorSetLayoutBinding>& _vkDescriptorSetLayoutBindings,
        size_t _allocationCout);

    void Destroy();

    const VkDescriptorPool& Get() const;

    VkDescriptorPool& Get();

    void CreateDescriptorSet(VkDescriptorSetLayout _setLayout, uint32_t _nbr, VkDescriptorSet* _data) const;
private:
    VkDescriptorPool m_VkDescriptorPool = VK_NULL_HANDLE;

    uint32_t m_MaxAllocationFromPool = -1;
    
};

END_PCCORE