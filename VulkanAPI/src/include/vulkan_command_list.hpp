#pragma once

#include "vulkan_header.h"
#include "low_renderer/command_list.hpp"

namespace Vulkan
{
   
class VulkanCommandList : public PC_CORE::CommandList
{
public:

    VULKAN_API VulkanCommandList(const PC_CORE::CommandListCreateInfo& _commandListCreateInfo);

    VULKAN_API VulkanCommandList() = default;

    VULKAN_API ~VulkanCommandList() override;
    
    VULKAN_API void Reset() override;

    VULKAN_API void BeginRecordCommands() override;

    VULKAN_API void EndRecordCommands() override;

    VULKAN_API void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) override;

    VULKAN_API void EndRenderPass() override;

    VULKAN_API void BindDescriptorSet(const PC_CORE::ShaderProgram* _shaderProgram,
                                      const PC_CORE::ShaderProgramDescriptorSets* _shaderProgramDescriptorSets,
                                      size_t _firstSet,
                                      size_t _descriptorSetCount) override;

    VULKAN_API void BindProgram(const PC_CORE::ShaderProgram* _shaderProgramm) override;

    VULKAN_API void SetViewPort(const PC_CORE::ViewportInfo& _viewPort) override;

    VULKAN_API void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance) override;

    VULKAN_API void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex, int32_t _vertexOffset,
        size_t _firstInstance) override;

    VULKAN_API void BindVertexBuffer(const PC_CORE::VertexBuffer& _vertexBuffer, uint32_t _firstBinding, uint32_t _bindingCount);

    VULKAN_API void BindIndexBuffer(const PC_CORE::IndexBuffer& _indexBuffer, size_t _offset) override;

    
    VULKAN_API vk::CommandBuffer GetHandle() const;

    VULKAN_API const vk::Queue* GetQueue() const;

private:
    std::array<vk::CommandBuffer, MAX_FRAMES_IN_FLIGHT> m_CommandBuffer;

    const vk::Queue* m_Queue = VK_NULL_HANDLE;
};
    
}
