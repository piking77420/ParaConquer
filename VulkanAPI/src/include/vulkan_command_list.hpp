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

    VULKAN_API void MergeCommands(CommandList* _secondaries, size_t _count) override;

    VULKAN_API void BeginRecordCommands() override;

    VULKAN_API void EndRecordCommands() override;

    VULKAN_API void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) override;

    VULKAN_API void NextSubPass() override;

    VULKAN_API void EndRenderPass() override;

    VULKAN_API void BindDescriptorSet(const PC_CORE::ShaderProgram* _shaderProgram,
                                      const PC_CORE::ShaderProgramDescriptorSets* _shaderProgramDescriptorSets,
                                      size_t _firstSet,
                                      size_t _descriptorSetCount) override;

    VULKAN_API void BindProgram(const PC_CORE::ShaderProgram* _shaderProgramm) override;

    VULKAN_API void PushConstant(const PC_CORE::ShaderProgram* _shaderProgram, const std::string& _pushConstantKey,
        const  void* _data, size_t _size) override;

    VULKAN_API void SetViewPort(const PC_CORE::ViewportInfo& _viewPort) override;

    VULKAN_API void SetPrimitiveTopology(PC_CORE::PrimitiveTopology _primitiveTopology) override;

    VULKAN_API void SetBlendEquation(uint32_t _firstAttachement, uint32_t _attachementCount) override;

    VULKAN_API void SetLineWidth(float _widht) override;

    VULKAN_API void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance) override;

    VULKAN_API void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex, int32_t _vertexOffset,
        size_t _firstInstance) override;

    VULKAN_API void BindVertexBuffer(const PC_CORE::RhiVertexBuffer& _vertexBuffer, uint32_t _firstBinding, uint32_t _bindingCount) override;

    VULKAN_API void BindIndexBuffer(const PC_CORE::RhiIndexBuffer& _indexBuffer, size_t _offset) override;

    VULKAN_API  void CopyBuffer(const PC_CORE::RhiBuffer& _src, const PC_CORE::RhiBuffer& _dst, size_t _srcOffSet, size_t _dstoffset, size_t _sizeInBytes,
        PC_CORE::GpuPipelineStageFlagBits _dstBufferUsage) override;

    VULKAN_API void Flush() override;

    VULKAN_API void BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color) override;

    VULKAN_API void EndDebugLabel() override;

    VULKAN_API vk::CommandBuffer GetHandle() const;

private:
    std::array<vk::CommandBuffer, MAX_FRAMES_IN_FLIGHT> m_CommandBuffer;

#ifdef  defined(PROFILING)
    tracy::VkCtx* m_VkTracyContext = nullptr;
#endif
  
};
    
}
