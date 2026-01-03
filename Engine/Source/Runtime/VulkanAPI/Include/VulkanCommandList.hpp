#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/CommandList.hpp"


namespace Vulkan
{   

    class VulkanCommandList final : public PC_CORE::CommandList
    {
    public:
#ifdef PROFILING
        tracy::VkCtx* tracyContext = nullptr;
#endif
        
        VULKAN_API explicit VulkanCommandList(PC_CORE::Rhi& _Rhi);

        VULKAN_API ~VulkanCommandList() override;

        VULKAN_API const void* GetFrameNativeHandle(size_t _frameIndex) const override;

        VULKAN_API void* GetFrameNativeHandle(size_t _frameIndex) override;

        VULKAN_API bool Build() override;

        VULKAN_API void Reset() override;

        VULKAN_API void MergeCommands(CommandList* _secondaries, size_t _count) override;

        VULKAN_API void BeginRecordCommands() override;

        VULKAN_API void EndRecordCommands() override;

        VULKAN_API void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) override;

        VULKAN_API void NextSubPass() override;

        VULKAN_API void EndRenderPass() override;

        VULKAN_API void BindDescriptorSet(const PC_CORE::RhiShaderProgram& _RhiShaderProgram,
                                          const PC_CORE::RhiDescriptorBindings* _shaderProgramDescriptorSets,
                                          size_t _firstSet,
                                          size_t _descriptorSetCount) override;

        VULKAN_API void BindProgram(const PC_CORE::RhiShaderProgram& _RhiShaderProgram) override;

        VULKAN_API void PushConstant(const PC_CORE::RhiShaderProgram& _RhiShaderProgram, const std::string& _pushConstantKey,
                                     const void* _data, size_t _size) override;

        VULKAN_API void SetViewPort(const PC_CORE::ViewportInfo& _viewPort) override;

        VULKAN_API void SetPrimitiveTopology(PC_CORE::RhiShaderProgram::PrimitiveTopology _primitiveTopology) override;

        VULKAN_API void SetBlendEquation(uint32_t _firstAttachement, uint32_t _attachementCount) override;

        VULKAN_API void SetLineWidth(float _widht) override;

        VULKAN_API void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
                             uint32_t _firstInstance) override;

        VULKAN_API void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
                                    int32_t _vertexOffset,
                                    size_t _firstInstance) override;

        VULKAN_API void Dispatch(uint32_t, uint32_t, uint32_t) override;

        VULKAN_API void BindVertexBuffer(const PC_CORE::RhiBuffer& _vertexBuffer, uint32_t _firstBinding,
                                         uint32_t _bindingCount) override;

        VULKAN_API void BindIndexBuffer(const PC_CORE::RhiBuffer& _indexBuffer, PC_CORE::RhiBuffer::IndexFormat _format, size_t _offset) override;

        VULKAN_API void CopyBuffer(const PC_CORE::RhiBuffer& _src, const PC_CORE::RhiBuffer& _dst, size_t _srcOffSet,
                                   size_t _dstoffset, size_t _sizeInBytes) override;

        VULKAN_API void Barrier(RhiResourceState _OldState, RhiResourceState _NewState,
            const std::span<PC_CORE::ImageStateTransition>& _ImageStateTransition,
            const std::span<PC_CORE::BufferStateTransition>& _BufferStateTransition) override;


        VULKAN_API void BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color) override;

        VULKAN_API void EndDebugLabel() override;

        VULKAN_API vk::CommandBuffer GetVkHandle() const;

        VULKAN_API vk::CommandBuffer GetVkHandle(size_t _FrameIndex) const;

        VULKAN_API vk::Semaphore GetVkSemaphore() const;

    private:
        std::array<vk::CommandBuffer, MaxFramesInFlight> m_CommandBuffer;

        std::array<vk::Semaphore, MaxFramesInFlight> m_Semaphore;

        std::vector<vk::ImageMemoryBarrier> m_VkImageBarrier;

        std::vector<vk::BufferMemoryBarrier> m_VkBufferBarrier;

#if DEBUG_GPU_ON
        std::string m_SemaphoreDebugName;
#endif

    };
}


#define GET_VK_COMMAND_BUFFER(CommandList, frameIndex) \
    vk::CommandBuffer cmb = *static_cast<vk::CommandBuffer*>(CommandList->GetFrameNativeHandle(frameIndex));