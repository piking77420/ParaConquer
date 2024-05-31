#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanCommandBuffer
{
public:
    void SetNbrofAllocation(size_t size);
    
    VkCommandBuffer& operator[](size_t index);

    const VkCommandBuffer& operator[](size_t index) const;

    VkCommandBuffer* GetPtr();

private:
    std::vector<VkCommandBuffer> m_CommandsBuffer;
};

END_PCCORE