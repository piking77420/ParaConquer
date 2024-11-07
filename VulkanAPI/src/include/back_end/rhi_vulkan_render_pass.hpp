#pragma once
#include "vulkan_header.h"

namespace Vulkan
{
    namespace Backend
    {
        struct RenderPassDescription
        {
            uint32_t nbrOfColorPass = 0;
            uint32_t nbrOfDepthPass = 0;

            std::vector<vk::SubpassDescription> subpassesDescription;
            std::vector<vk::SubpassDependency> dependencies;

            std::vector<vk::AttachmentReference> colorAttachmentRef;
            std::vector<vk::AttachmentReference> depthAttachmentRef;
        };


        PC_CORE::RenderPassHandle CreateRenderPass(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, vk::Device _device);

       void ResolveAttachementDescriptrion(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, std::vector<vk::AttachmentDescription>& _vkAttachmentDescriptions);

       void CountSubPasses(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, RenderPassDescription* _renderPassDescription);

       void ResolveSubPasses(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, RenderPassDescription* _renderPassDescription);

       void ResolveSubPassDependencies(vk::SubpassDependency* _vkSubpassDependency, uint32_t _colorCount, uint32_t _depthCout);

        void ResolveBeginRenderPass(PC_CORE::RenderPassHandle _renderPassHandle, const PC_CORE::BeginRenderPassInfo& _renderPassInfo,
            vk::RenderPassBeginInfo* _vkRenderPassBeginInfo, vk::ClearValue* _vkClearValue);
    }
}
