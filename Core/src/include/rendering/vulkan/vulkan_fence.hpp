#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanFence
{
public:

    void Init(const VkFenceCreateInfo& _semaphoreCreateInfo, size_t _nbr);

    VkFence& operator[](size_t _index);

    const VkFence& operator[](size_t _index) const;

    VkFence* GetPtr();

    void Destroy();
    
private:
    std::vector<VkFence> m_Fences;
};

END_PCCORE