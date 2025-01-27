#pragma once

#include "vulkan_header.h"
#include "low_renderer/command_list.hpp"

namespace Vulkan
{
   
class VulkanCommandList : public PC_CORE::CommandList
{
public:
    VULKAN_API VulkanCommandList(PC_CORE::CommandPoolFamily _commandPoolFamily);

    VULKAN_API VulkanCommandList() = default;

    VULKAN_API ~VulkanCommandList() override;
    
    VULKAN_API void Reset() override;

    VULKAN_API void BeginRecordCommands() override;

    VULKAN_API void EndRecordCommands() override;

    VULKAN_API void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) override;

    VULKAN_API void EndRenderPass() override;

    VULKAN_API void BindProgram(const PC_CORE::ShaderProgram* _shaderProgramm) override;

    VULKAN_API void SetViewPort(const PC_CORE::ViewportInfo& _viewPort) override;

    VULKAN_API void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance) override;
    
    VULKAN_API vk::CommandBuffer GetHandle() const;

    VULKAN_API const vk::Queue* GetQueue() const;

private:
    vk::CommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

    const vk::Queue* m_Queue = VK_NULL_HANDLE;
};
    
}
