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

        VULKAN_API bool Build() override;

        VULKAN_API void Reset() override;

        VULKAN_API void MergeCommands(CommandList* _secondaries, size_t _count) override;

        VULKAN_API void BeginRecordCommands() override;

        VULKAN_API void EndRecordCommands() override;

        VULKAN_API void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) override;

        VULKAN_API void BeginComputePasss() override;

        VULKAN_API void NextSubPass() override;

        VULKAN_API void EndRenderPass() override;

        VULKAN_API void BindDescriptorSet(const PC_CORE::RhiDescriptorSet* _shaderProgramDescriptorSets,
                                          size_t _FirstSet,
                                          std::optional<size_t> _DynamicOffset) override;

        VULKAN_API void BindDescriptorSets(
            const std::span<const PC_CORE::RhiDescriptorSet*>& descriptorSets,
            size_t firstSet,
            const std::span<const size_t>& dynamicOffset) override;

        VULKAN_API bool BindRhiPipeline(const PC_CORE::RhiPipeline& _RhiPipeline) override ;

        VULKAN_API void PushConstant(RhiShaderStageTypeFlag _RhiShaderStageTypeFlag,
                                     const void* _Data,
                                     uint32_t _Offset,
                                     uint32_t _Size) override;

        VULKAN_API void SetViewPort(const PC_CORE::ViewportInfo& _viewPort) override;

        VULKAN_API void SetPrimitiveTopology(PC_CORE::RhiGraphicPipeline::PrimitiveTopology _primitiveTopology) override;

        VULKAN_API void SetBlendEquation(uint32_t _firstAttachement, uint32_t _attachementCount) override;

        VULKAN_API void SetLineWidth(float _widht) override;

        VULKAN_API void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
                             uint32_t _firstInstance) override;

        VULKAN_API void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
                                    int32_t _vertexOffset,
                                    size_t _firstInstance) override;

        VULKAN_API void Dispatch(uint32_t, uint32_t, uint32_t) override;

        VULKAN_API void DrawMeshTask(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ) override;

        VULKAN_API void BindDrawBuffers(const DrawBuffers& _DrawBuffers) override;


        VULKAN_API void CopyBuffer(const PC_CORE::RhiBuffer& _src, const PC_CORE::RhiBuffer& _dst, size_t _srcOffSet,
                                   size_t _dstoffset, size_t _sizeInBytes) override;

        VULKAN_API void Barrier(RhiResourceState _OldState, RhiResourceState _NewState,
            const std::span<const PC_CORE::ImageStateTransition>& _ImageStateTransition,
            const std::span<const PC_CORE::BufferStateTransition>& _BufferStateTransition) override;


        VULKAN_API void BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color) override;

        VULKAN_API void EndDebugLabel() override;

        VULKAN_API vk::CommandBuffer GetVulkanCommandBufferHandle() const;

        VULKAN_API vk::CommandBuffer GetVulkanCommandBufferHandle(size_t _FrameIndex) const;

        VULKAN_API vk::Semaphore GetVkSemaphore() const;

    private:
        std::array<vk::CommandBuffer, MaxFramesInFlight> m_CommandBuffer;

        std::array<vk::Semaphore, MaxFramesInFlight> m_Semaphore;

        std::vector<vk::ImageMemoryBarrier> m_VkImageBarrier;

        std::vector<vk::BufferMemoryBarrier> m_VkBufferBarrier;

        std::vector<vk::CommandBuffer> m_MergedCommandBufferBuffer;

        std::vector<vk::DescriptorSet> m_VkDescriptorSetBuffer;

        std::vector<uint32_t> m_DynamicOffsetBuffer;

        vk::PipelineBindPoint GetCurrentRecordPipelineBindPoint() const;

        vk::PipelineLayout GetCurrentPipelineLayout() const;

        vk::Pipeline GetCurrentPipeline() const;


        bool IsInRecordState() const;
#if DEBUG_GPU_ON
        std::string m_SemaphoreDebugName;
#endif

    };
}


#define GET_VK_COMMAND_BUFFER(CommandList, frameIndex) \
    vk::CommandBuffer cmb = reinterpret_cast<VulkanCommandList*>(CommandList)->GetVulkanCommandBufferHandle(FrameIndex);