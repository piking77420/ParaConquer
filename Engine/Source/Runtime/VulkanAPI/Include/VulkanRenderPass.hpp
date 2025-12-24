#pragma once

#include "VulkanHeader.h"
#include "LowRenderer/RhiRenderPass.hpp"

namespace Vulkan
{
    class VulkanRenderPass : public PC_CORE::RhiRenderPass
    {
    public:
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

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, const PC_CORE::RenderPassDescriptor& _attachements);

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat, PC_CORE::RhiFormat depthFormat);

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat, uint32_t _sampleCount);

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat);

        VULKAN_API VulkanRenderPass(PC_CORE::Rhi& _Rhi, vk::Device device, vk::Format format);

        VULKAN_API ~VulkanRenderPass() override;

    protected:
        vk::RenderPass m_RenderPass = VK_NULL_HANDLE;

        void ParseSubPassTransition(const PC_CORE::SubPassTransition& _subPassDependcies,
                             vk::SubpassDependency* _vkdependency);

        vk::ImageLayout GetImageLayoutSubPass(PC_CORE::AttachmentType _attachmentType);

        vk::ImageLayout GetImageLayoutSubPassForInputAttachement(PC_CORE::AttachmentType _attachmentType);


        std::vector<vk::AttachmentDescription> ParseAttahchementDescription(
            const PC_CORE::RenderPassDescriptor& _renderPassDescriptor, bool _hasdepth);
    };
}
