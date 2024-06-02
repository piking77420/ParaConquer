#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanDescriptorPool
{
public:
    void Init();

    void Destroy();

    const VkDescriptorPool& Get() const;

    VkDescriptorPool& Get();
private:
    VkDescriptorPool m_VkDescriptorPool = VK_NULL_HANDLE;
};

END_PCCORE