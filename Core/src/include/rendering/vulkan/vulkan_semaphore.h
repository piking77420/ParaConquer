#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "core_header.hpp"

BEGIN_PCCORE

class VulkanSemaphore
{
public:
    void Init(const VkSemaphoreCreateInfo& semaphoreCreateInfo, size_t nbr);

    VkSemaphore& operator[](size_t index);

    const VkSemaphore& operator[](size_t index) const;

    VkSemaphore* GetPtr();

    void Destroy();

private:
    std::vector<VkSemaphore> m_Semaphores;
};

END_PCCORE