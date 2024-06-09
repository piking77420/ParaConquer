#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkankPipelineLayout
{
public:
    void Init(const std::vector<VkDescriptorSetLayout>& _vkDescriptorSetLayouts,
    const std::vector<VkPushConstantRange> _vkPushConstantRange = {});

    void Destroy();

    VkPipelineLayout& Get();

    const VkPipelineLayout& Get() const;

private:
    VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;
};

END_PCCORE