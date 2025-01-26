#pragma once

#include "vulkan_header.h"
#include "low_renderer/command_list.hpp"

namespace Vulkan
{
   
class VulkanCommandList : public PC_CORE::CommandList
{
public:
    VulkanCommandList(PC_CORE::CommandPoolFamily _commandPoolFamily);

    VulkanCommandList() = default;

    ~VulkanCommandList() override;
    
    void BeginRecordCommands() override;

    void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) override;
    
    void EndRenderPass() override;

private:
    vk::CommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
};
    
}
