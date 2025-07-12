#pragma once

#include "vulkan_header.h"
#include "low_renderer/rhi_render_pass.hpp"

namespace Vulkan
{
    class VulkanRenderPass : public PC_CORE::RhiRenderPass
    {
    public:

        VULKAN_API const void* GetNativeHandle() const override
        {
            return m_RenderPass;
        }

        VULKAN_API vk::RenderPass GetVulkanRenderPass() const;

        VULKAN_API VulkanRenderPass(const PC_CORE::RenderPassDescriptor& _attachements);
        
        VulkanRenderPass(PC_CORE::RHIFormat colorFormat, PC_CORE::RHIFormat depthFormat);

        VULKAN_API VulkanRenderPass(PC_CORE::RHIFormat colorFormat, uint32_t _sampleCount);

        VULKAN_API VulkanRenderPass(PC_CORE::RHIFormat colorFormat);

        VULKAN_API VulkanRenderPass(vk::Format format);

        VULKAN_API VulkanRenderPass() = default;

        VULKAN_API ~VulkanRenderPass() override;

    protected:
        vk::RenderPass m_RenderPass = VK_NULL_HANDLE;

        void ParseAttachmentLayout(const PC_CORE::RenderPassAttachementDescriptor& _attachment, vk::AttachmentDescription* _attachmentLayouts);

        void ParseDependcies(const PC_CORE::SubPassDependcies& _subPassDependcies, vk::SubpassDependency* _vkdependency);

        vk::ImageLayout GetImageLayoutSubPass(PC_CORE::AttachmentType _attachmentType);
  
        vk::ImageLayout GetImageLayoutSubPassForInputAttachement(PC_CORE::AttachmentType _attachmentType);


        std::vector<vk::AttachmentDescription> ParseAttahchementDescription(const PC_CORE::RenderPassDescriptor& _renderPassDescriptor, bool _hasdepth);

    };
    
}

