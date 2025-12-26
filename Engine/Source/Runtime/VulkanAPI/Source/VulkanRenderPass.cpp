#include "VulkanRenderPass.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "VulkanContext.hpp"
#include "VulkanSwapChain.hpp"

void CountAttachementRef(const std::vector<PC_CORE::SubPass>& _SubPasses, size_t AttachementCount,  
    size_t* colorAttachRefCount, size_t* inputAttachRefCount, size_t* preservedAttachementRefCount)
{
    assert(colorAttachRefCount != nullptr && inputAttachRefCount != nullptr && preservedAttachementRefCount != nullptr);

    for (const auto& subpass : _SubPasses)
    {
        const size_t colorAttachementCount = subpass.colorAttachementDescriptorIndicies.
            size();

        const size_t inputAttachementCount = subpass.inputAttachementIndicies.
            size();

        const size_t preserved = AttachementCount - colorAttachementCount -
            inputAttachementCount;

        *colorAttachRefCount += colorAttachementCount;
        *inputAttachRefCount += inputAttachementCount;
        *preservedAttachementRefCount += preserved;
    }
}


Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi)
    : RhiRenderPass(_Rhi)
{
   
}

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi   , PC_CORE::RhiFormat colorFormat,
    PC_CORE::RhiFormat depthFormat)
    : RhiRenderPass(_Rhi)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = static_cast<VkFormat>(Utils::RhiFormatToVkFormat(colorFormat));
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = static_cast<VkFormat>(Utils::RhiFormatToVkFormat(depthFormat));
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    m_RenderPass = GET_VK_DEVICE.createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat, uint32_t _sampleCount)
    : RhiRenderPass(_Rhi)
{
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = Utils::RhiFormatToVkFormat(colorFormat);
    colorAttachment.samples = Utils::RhSampleCountToVulkan(_sampleCount);
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::eGeneral;

    vk::AttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = Utils::RhiFormatToVkFormat(colorFormat);
    colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentResolve.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 1;
    colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = {};
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, colorAttachmentResolve};

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    m_RenderPass = GET_VK_DEVICE.createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat)
    : RhiRenderPass(_Rhi)
{
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = Utils::RhiFormatToVkFormat(colorFormat);
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = 0;
    dependency.dstSubpass = VK_SUBPASS_EXTERNAL;

    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependency.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    m_RenderPass = GET_VK_DEVICE.createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi, vk::Device device, vk::Format format)
    : RhiRenderPass(_Rhi)
{
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = format;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = {};
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    m_RenderPass = device.createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::~VulkanRenderPass()
{
    
    if (m_RenderPass == VK_NULL_HANDLE)
        return;

    GET_VK_DEVICE.destroyRenderPass(m_RenderPass);
    m_RenderPass = VK_NULL_HANDLE;
}

bool Vulkan::VulkanRenderPass::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    bool hasDepthAttachment = m_DepthStencilAttachement.format != PC_CORE::RhiFormat::Undefined;

    // ---------------- Attachment Descriptions ----------------
    std::vector<vk::AttachmentDescription> vkAttachments = ParseAttahchementDescription(hasDepthAttachment);

    // ---------------- Attachment References ----------------
    std::vector<vk::AttachmentReference> colorAttachmentReferences;
    std::vector<vk::AttachmentReference> inputAttachementReferences;

    std::vector<uint32_t> subpassPreserved;

    size_t colorAttachementRefsCount = 0;
    size_t inputAttachementRefsCount = 0;
    size_t totalPreserved = 0;

    CountAttachementRef(m_SubPasses, m_Attachement.size(),
        &colorAttachementRefsCount, &inputAttachementRefsCount, &totalPreserved);

    colorAttachmentReferences.resize(colorAttachementRefsCount);
    inputAttachementReferences.resize(inputAttachementRefsCount);
    subpassPreserved.resize(totalPreserved);

    size_t colorIndex = 0;
    size_t inputIndex = 0;
    size_t preservedIndex = 0;

    const uint32_t subPassCount = static_cast<uint32_t>(m_SubPasses.size());
    for (uint32_t i = 0; i < subPassCount; ++i)
    {
        const auto& subpass = m_SubPasses[i];

        assert(subpass.useDepth ? (hasDepthAttachment) : true &&
            "Subpass uses depth but no depth attachment provided");

        std::set<uint32_t> usedIndices(subpass.colorAttachementDescriptorIndicies.begin(),
            subpass.colorAttachementDescriptorIndicies.end());
        for (auto& it : subpass.inputAttachementIndicies)
            usedIndices.emplace(it);

        // Fill preserved
        for (uint32_t att = 0; att < static_cast<uint32_t>(m_Attachement.size()); ++att)
            if (!usedIndices.contains(att))
                subpassPreserved[preservedIndex++] = att;

        for (uint32_t j = 0; j < subpass.colorAttachementDescriptorIndicies.size(); ++j)
        {
            const uint32_t attachmentIndex = subpass.colorAttachementDescriptorIndicies[j];

            colorAttachmentReferences[colorIndex].attachment = attachmentIndex;
            colorAttachmentReferences[colorIndex].layout = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(m_Attachement[attachmentIndex].currentImageState);
            colorIndex++;
        }

        for (uint32_t j = 0; j < subpass.inputAttachementIndicies.size(); ++j)
        {
            const uint32_t attachmentIndex = subpass.inputAttachementIndicies[j];
            inputAttachementReferences[inputIndex].attachment = attachmentIndex;
            inputAttachementReferences[inputIndex].layout = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(m_Attachement[attachmentIndex].currentImageState);
            inputIndex++;
        }
    }

    // Depth reference
    vk::AttachmentReference depthAttachmentRef;
    if (hasDepthAttachment)
    {
        depthAttachmentRef.attachment = static_cast<uint32_t>(vkAttachments.size() - 1); // always put depth at the end
        depthAttachmentRef.layout = Vulkan::Utils::RhiResourceStateToVulkanImageLayout(m_DepthStencilAttachement.currentImageState);
    }

    // ---------------- Subpasses ----------------
    std::vector<vk::SubpassDescription> subPasses(m_SubPasses.size());

    uint32_t colorAttachementOffset = 0;
    uint32_t inputAttachementOffset = 0;
    uint32_t preserveAttachementOffset = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(m_SubPasses.size()); ++i)
    {
        const auto& subPass = m_SubPasses[i];
        auto& vkSubpass = subPasses[i];

        const uint32_t colorAttCount = static_cast<uint32_t>(subPass.colorAttachementDescriptorIndicies.size());
        const uint32_t inputAttCount = static_cast<uint32_t>(subPass.inputAttachementIndicies.size());

        vkSubpass.flags = {};
        vkSubpass.pipelineBindPoint = Utils::RhiPipelineBindPointToVulkan(subPass.type);

        // Color
        vkSubpass.colorAttachmentCount = 0;
        if (vkSubpass.colorAttachmentCount < colorAttachmentReferences.size())
        {
            vkSubpass.colorAttachmentCount = colorAttCount;
            vkSubpass.pColorAttachments = &colorAttachmentReferences[colorAttachementOffset];
        }


        // Input
        vkSubpass.inputAttachmentCount = 0;
        if (inputAttachementOffset < inputAttachementReferences.size())
        {
            vkSubpass.inputAttachmentCount = inputAttCount;
            vkSubpass.pInputAttachments = &inputAttachementReferences[inputAttachementOffset];
        }

        // Depth
        vkSubpass.pDepthStencilAttachment = subPass.useDepth ? &depthAttachmentRef : nullptr;

        vkSubpass.preserveAttachmentCount = 0;
        if (preserveAttachementOffset < subpassPreserved.size())
        {
            vkSubpass.preserveAttachmentCount = static_cast<uint32_t>(m_Attachement.size()) -
                colorAttCount - inputAttCount;
            vkSubpass.pPreserveAttachments = &subpassPreserved[preserveAttachementOffset];
        }

        preserveAttachementOffset += vkSubpass.preserveAttachmentCount;
        colorAttachementOffset += subPass.colorAttachementDescriptorIndicies.size();
        preserveAttachementOffset += vkSubpass.preserveAttachmentCount;
    }

    // ---------------- Dependencies ----------------
    std::vector<vk::SubpassDependency> dependencies(subPasses.size());

    for (int i = 0; i < dependencies.size(); i++)
    {
        const int srcIndex = (i - 1) <= 0 ? 0 : (i - 1);
        const int destIndex = (i + 1) == dependencies.size() ? VK_SUBPASS_EXTERNAL : (i + 1);

        auto& dep = dependencies[i];
        dep.srcSubpass = static_cast<uint32_t>(srcIndex);
        dep.dstSubpass = static_cast<uint32_t>(destIndex);

        ParseSubPassTransition(m_SubPasses[i], &dep);
    }

    // ---------------- Create Render Pass ----------------
    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(vkAttachments.size());
    renderPassInfo.pAttachments = vkAttachments.data();
    renderPassInfo.subpassCount = static_cast<uint32_t>(subPasses.size());
    renderPassInfo.pSubpasses = subPasses.data();
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    m_RenderPass = GET_VK_DEVICE.createRenderPass(renderPassInfo);

    vk::DebugUtilsObjectNameInfoEXT nameInfoImageView;
    nameInfoImageView.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
    nameInfoImageView.pNext = nullptr;
    nameInfoImageView.objectType = vk::ObjectType::eRenderPass;
    nameInfoImageView.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkRenderPass>(m_RenderPass));
    nameInfoImageView.pObjectName = GetName().data();

    SET_VK_DEBUG_NAME(nameInfoImageView);

    return true;
}

vk::RenderPass Vulkan::VulkanRenderPass::GetVulkanRenderPass() const
{
    return m_RenderPass;
}



vk::ImageLayout Vulkan::VulkanRenderPass::GetImageLayoutSubPassForInputAttachement(RhiResourceState resourceState)
{
    assert(resourceState == RhiResourceState::RenderTarget && "Only color render target are supported now as input attacheent");

    return vk::ImageLayout::eShaderReadOnlyOptimal;
}

std::vector<vk::AttachmentDescription> Vulkan::VulkanRenderPass::ParseAttahchementDescription(bool _hasdepth)
{
    std::vector<vk::AttachmentDescription> vkAttachments(
        m_Attachement.size() + static_cast<size_t>(_hasdepth));

    for (uint32_t i = 0; i < m_Attachement.size(); i++)
    {
        const auto& attachment = m_Attachement[i];
        vk::AttachmentDescription& vkAttachment = vkAttachments[i];

        vkAttachment.flags = {};
        vkAttachment.format = Utils::RhiFormatToVkFormat(attachment.format);
        vkAttachment.samples = Utils::RhSampleCountToVulkan(attachment.sampleCount);

        vkAttachment.loadOp = Utils::RhiLoadOperationToVulkan(attachment.load);
        vkAttachment.storeOp = Utils::RhiStoreOperationToVulkan(attachment.store);

        vkAttachment.stencilLoadOp = Utils::RhiLoadOperationToVulkan(attachment.stencilLoad);
        vkAttachment.stencilStoreOp = Utils::RhiStoreOperationToVulkan(attachment.stencilStore);

        vkAttachment.initialLayout = Utils::RhiResourceStateToVulkanImageLayout(attachment.currentImageState);
        vkAttachment.finalLayout = Utils::RhiResourceStateToVulkanImageLayout(attachment.finalImageState);
    }

    if (_hasdepth)
    {
        vk::AttachmentDescription depthDesc{};
        depthDesc.format = Utils::RhiFormatToVkFormat(m_DepthStencilAttachement.format);
        depthDesc.samples = Utils::RhSampleCountToVulkan(m_DepthStencilAttachement.sampleCount);
        depthDesc.loadOp = Utils::RhiLoadOperationToVulkan(m_DepthStencilAttachement.load);
        depthDesc.storeOp = Utils::RhiStoreOperationToVulkan(m_DepthStencilAttachement.store);
        depthDesc.stencilLoadOp = Utils::RhiLoadOperationToVulkan(m_DepthStencilAttachement.stencilLoad);
        depthDesc.stencilStoreOp = Utils::RhiStoreOperationToVulkan(m_DepthStencilAttachement.stencilStore);

        depthDesc.initialLayout = Utils::RhiResourceStateToVulkanImageLayout(
            m_DepthStencilAttachement.currentImageState);
        depthDesc.finalLayout = Utils::RhiResourceStateToVulkanImageLayout(
            m_DepthStencilAttachement.finalImageState);

        const size_t depthIndex = vkAttachments.size() - 1;
        vkAttachments[depthIndex] = depthDesc;
    }

    return vkAttachments;
}


void Vulkan::VulkanRenderPass::ParseSubPassTransition(const PC_CORE::SubPass& _subPass,
    vk::SubpassDependency* _vkdependency)
{
    // TODO HANDLE LOAD AND STORE OP

    // Handle Access
    {
        _vkdependency->srcAccessMask = Utils::RhiResourceStateToAccesFlag(_subPass.subPassTransition.ImageStateTransition.OldState);
        _vkdependency->dstAccessMask = Utils::RhiResourceStateToAccesFlag(_subPass.subPassTransition.ImageStateTransition.NewState);
    }



    // Handle Stage
    {
        _vkdependency->srcStageMask = Utils::RhiPipelineStageToVulkan(_subPass.subPassTransition.SrcStageFlag);
        _vkdependency->dstStageMask = Utils::RhiPipelineStageToVulkan(_subPass.subPassTransition.DstStageFlag);
    }

}
