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
        vk::RenderPass m_RenderPass = VK_NULL_HANDLE;

        void ParseSubPassTransition(const PC_CORE::SubPass& _SubPass, vk::SubpassDependency* _vkdependency);


        vk::ImageLayout GetImageLayoutSubPassForInputAttachement(RhiResourceState resourceState);

        std::vector<vk::AttachmentDescription> ParseAttahchementDescription(bool _hasdepth);
    };
}
