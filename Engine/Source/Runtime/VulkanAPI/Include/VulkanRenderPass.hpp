#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiRenderPass.hpp"

namespace Vulkan
{
    class VulkanRenderPass : public PC_CORE::RhiRenderPass
    {
    public:
       
        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi);

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat, PC_CORE::RhiFormat depthFormat);

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat, uint32_t _sampleCount);

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat);

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, vk::Device device, vk::Format format);

        VULKAN_API ~VulkanRenderPass() override;

        bool Build() override;

        VULKAN_API const void* GetFrameNativeHandle(size_t _frameIndex) const override
        {
            return &m_RenderPass;
        }

        VULKAN_API void* GetFrameNativeHandle(size_t _frameIndex) override
        {
            return &m_RenderPass;
        }

        VULKAN_API vk::RenderPass GetVulkanRenderPass() const;


    protected:
        struct CreateRenderPassContext
        {
         
    
        };

        struct RenderPassBuildContext
        {

            std::vector<vk::AttachmentDescription> AttachmentDescriptions;
            std::vector<vk::AttachmentReference> ColorAttachmentReferences;
            std::vector<vk::AttachmentReference> InputAttachementReferences;
            std::vector<vk::AttachmentReference> Preserved;
            std::vector<vk::AttachmentReference> DepthsAttachements;

            std::vector<vk::SubpassDescription> vkSubpassDescriptions;
            std::vector<vk::SubpassDependency> vkSubpassDependicies;
        };

        vk::RenderPass m_RenderPass = VK_NULL_HANDLE;

        void PopulateAttachementDescription(RenderPassBuildContext* _RenderPassBuildContext);

        void PopulateSubPassContext(RenderPassBuildContext* _RenderPassBuildContext);

        void PopulateSubPassDependies(std::vector<vk::SubpassDependency>* _RenderPassBuildContext);

        vk::SubpassDependency ComputeMask(
            const std::array<std::array<RhiResourceState, MaxSubPass>, MaxAttachementSlot>& usageTable,
            uint32_t _SubPassIndex,
            PC_CORE::AttachementRef _AttachementRef,
            uint32_t _SrcSubPassIndex,
            uint32_t _DstSubPasIndex);

        static std::pair<vk::AccessFlags, vk::PipelineStageFlags> AttachementDependencies(const PC_CORE::RhiResource::State _State, const PC_CORE::RenderPassAttachementDescriptor& RenderPassAttachementDescriptor);

    };
}
