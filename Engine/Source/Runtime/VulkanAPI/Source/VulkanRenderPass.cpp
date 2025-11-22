#include "VulkanRenderPass.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "VulkanContext.hpp"
#include "VulkanSwapChain.hpp"

Vulkan::VulkanRenderPass::VulkanRenderPass(const PC_CORE::RenderPassDescriptor& _renderPassDescriptor)
    : RhiRenderPass(_renderPassDescriptor)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::shared_ptr<VulkanDevice> device =
        std::reinterpret_pointer_cast<VulkanDevice>(VulkanContext::GetContext().rhiDevice);

    assert(_renderPassDescriptor.attachement.size() < MaxColorAttachments &&
        "MAX_COLOR_ATTACHMENTS has been reached");

    bool hasDepthAttachment = _renderPassDescriptor.depthAttachment != nullptr;

    // ---------------- Attachment Descriptions ----------------
    std::vector<vk::AttachmentDescription> vkAttachments = ParseAttahchementDescription(
        _renderPassDescriptor, hasDepthAttachment);

    // ---------------- Attachment References ----------------
    std::vector<vk::AttachmentReference> colorAttachmentReferences;
    std::vector<vk::AttachmentReference> inputAttachementReferences;

    std::vector<uint32_t> subpassPreserved;

    size_t colorAttachementRefsCount = 0;
    size_t inputAttachementRefsCount = 0;

    size_t totalPreserved = 0;

    for (const auto& subpass : _renderPassDescriptor.subPasses)
    {
        const size_t colorAttachementCount = subpass.colorAttachementDescriptorIndicies.
                                                     size();

        const size_t inputAttachementCount = subpass.inputAttachementDescriptorIndicies.
                                                     size();

        const size_t preserved = _renderPassDescriptor.attachement.size() - colorAttachementCount -
            inputAttachementCount;

        colorAttachementRefsCount += colorAttachementCount;
        inputAttachementRefsCount += inputAttachementCount;
        totalPreserved += preserved;
    }

    colorAttachmentReferences.resize(colorAttachementRefsCount);
    inputAttachementReferences.resize(inputAttachementRefsCount);
    subpassPreserved.resize(totalPreserved);

    size_t colorIndex = 0;
    size_t inputIndex = 0;
    size_t preservedIndex = 0;

    const uint32_t subPassCount = static_cast<uint32_t>(_renderPassDescriptor.subPasses.size());
    for (uint32_t i = 0; i < subPassCount; ++i)
    {
        const auto& subpass = _renderPassDescriptor.subPasses[i];

        assert(subpass.useDepth ? (hasDepthAttachment) : true &&
            "Subpass uses depth but no depth attachment provided");

        std::set<uint32_t> usedIndices(subpass.colorAttachementDescriptorIndicies.begin(),
                                       subpass.colorAttachementDescriptorIndicies.end());
        for (auto& it : subpass.inputAttachementDescriptorIndicies)
            usedIndices.emplace(it);

        // Fill preserved
        for (uint32_t att = 0; att < static_cast<uint32_t>(_renderPassDescriptor.attachement.size()); ++att)
            if (!usedIndices.contains(att))
                subpassPreserved[preservedIndex++] = att;

        for (uint32_t j = 0; j < subpass.colorAttachementDescriptorIndicies.size(); ++j)
        {
            const uint32_t attachmentIndex = subpass.colorAttachementDescriptorIndicies[j];
            PC_CORE::AttachmentType _attachmentType = _renderPassDescriptor.attachement[attachmentIndex].attachmentType;

            colorAttachmentReferences[colorIndex].attachment = attachmentIndex;
            colorAttachmentReferences[colorIndex].layout = GetImageLayoutSubPass(
                _renderPassDescriptor.attachement[attachmentIndex].attachmentType);
            colorIndex++;
        }

        for (uint32_t j = 0; j < subpass.inputAttachementDescriptorIndicies.size(); ++j)
        {
            const uint32_t attachmentIndex = subpass.inputAttachementDescriptorIndicies[j];
            inputAttachementReferences[inputIndex].attachment = attachmentIndex;
            inputAttachementReferences[inputIndex].layout = GetImageLayoutSubPassForInputAttachement(
                _renderPassDescriptor.attachement[attachmentIndex].attachmentType);
            inputIndex++;
        }
    }

    // Depth reference
    vk::AttachmentReference depthAttachmentRef;
    if (hasDepthAttachment)
    {
        depthAttachmentRef.attachment = static_cast<uint32_t>(vkAttachments.size() - 1);
        depthAttachmentRef.layout = GetImageLayoutSubPass(_renderPassDescriptor.depthAttachment->attachmentType);
    }

    // ---------------- Subpasses ----------------
    std::vector<vk::SubpassDescription> subPasses(_renderPassDescriptor.subPasses.size());

    uint32_t colorAttachementOffset = 0;
    uint32_t inputAttachementOffset = 0;
    uint32_t preserveAttachementOffset = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(_renderPassDescriptor.subPasses.size()); ++i)
    {
        const auto& subPass = _renderPassDescriptor.subPasses[i];
        auto& vkSubpass = subPasses[i];

        const uint32_t colorAttCount = static_cast<uint32_t>(subPass.colorAttachementDescriptorIndicies.size());
        const uint32_t inputAttCount = static_cast<uint32_t>(subPass.inputAttachementDescriptorIndicies.size());

        vkSubpass.flags = {};
        vkSubpass.pipelineBindPoint = Utils::RhiPipelineBindPointToVulkan(subPass.shaderProgramPipelineType);


        vkSubpass.colorAttachmentCount = 0;
        if (vkSubpass.colorAttachmentCount < colorAttachmentReferences.size())
        {
            vkSubpass.colorAttachmentCount = colorAttCount;
            vkSubpass.pColorAttachments = &colorAttachmentReferences[colorAttachementOffset];
        }

        vkSubpass.inputAttachmentCount = 0;
        if (inputAttachementOffset < inputAttachementReferences.size())
        {
            vkSubpass.inputAttachmentCount = inputAttCount;
            vkSubpass.pInputAttachments = &inputAttachementReferences[inputAttachementOffset];
        }

        vkSubpass.pDepthStencilAttachment = subPass.useDepth ? &depthAttachmentRef : nullptr;

        vkSubpass.preserveAttachmentCount = 0;
        if (preserveAttachementOffset < subpassPreserved.size())
        {
            vkSubpass.preserveAttachmentCount = static_cast<uint32_t>(_renderPassDescriptor.attachement.size()) -
                colorAttCount - inputAttCount;
            vkSubpass.pPreserveAttachments = &subpassPreserved[preserveAttachementOffset];
        }

        preserveAttachementOffset += vkSubpass.preserveAttachmentCount;
        colorAttachementOffset += subPass.colorAttachementDescriptorIndicies.size();
        preserveAttachementOffset += vkSubpass.preserveAttachmentCount;
    }

    // ---------------- Dependencies ----------------
    std::vector<vk::SubpassDependency> dependencies(subPasses.size());

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    ParseDependcies(_renderPassDescriptor.subPasses[0].subPassDependcies, &dependencies[0]);

    for (uint32_t i = 1; i < dependencies.size(); ++i)
    {
        auto& dep = dependencies[i];
        dep.srcSubpass = i - 1;
        dep.dstSubpass = i;
        ParseDependcies(_renderPassDescriptor.subPasses[i].subPassDependcies, &dep);
        dep.dependencyFlags = {};
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

    m_RenderPass = device->GetDevice().createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::RhiFormat colorFormat,
                                           PC_CORE::RhiFormat depthFormat) : RhiRenderPass(
    colorFormat, depthFormat)
{
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

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

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    m_RenderPass = device->GetDevice().createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::RhiFormat colorFormat, uint32_t _sampleCount)
{
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = Utils::RhiFormatToVkFormat(colorFormat);
    colorAttachment.samples = Utils::RhiSampleCountToVuklan(_sampleCount);
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

    m_RenderPass = device->GetDevice().createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::RhiFormat colorFormat)
{
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

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

    m_RenderPass = device->GetDevice().createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::VulkanRenderPass(vk::Format format)
{
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

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

    m_RenderPass = device->GetDevice().createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::~VulkanRenderPass()
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);
    if (m_RenderPass == VK_NULL_HANDLE)
        return;

    vulkanDevice->GetDevice().destroyRenderPass(m_RenderPass);
    m_RenderPass = VK_NULL_HANDLE;
}

vk::RenderPass Vulkan::VulkanRenderPass::GetVulkanRenderPass() const
{
    return m_RenderPass;
}


void Vulkan::VulkanRenderPass::ParseDependcies(const PC_CORE::SubPassDependcies& _subPassDependcies,
                                               vk::SubpassDependency* _vkdependency)
{
    _vkdependency->srcStageMask = Utils::RhiPipelineStageToVulkan(_subPassDependcies.srcStageMask);
    _vkdependency->dstStageMask = Utils::RhiPipelineStageToVulkan(_subPassDependcies.dstStageMask);

    _vkdependency->srcAccessMask = Utils::RhiAccessFlagToVulkan(_subPassDependcies.srcAccessMask);
    _vkdependency->dstAccessMask = Utils::RhiAccessFlagToVulkan(_subPassDependcies.dstAccessMask);
}

vk::ImageLayout Vulkan::VulkanRenderPass::GetImageLayoutSubPass(PC_CORE::AttachmentType _attachmentType)
{
    switch (_attachmentType)
    {
    case PC_CORE::AttachmentType::Color:
        return vk::ImageLayout::eColorAttachmentOptimal;
        break;
    case PC_CORE::AttachmentType::Depth:
    case PC_CORE::AttachmentType::Stencil:
    case PC_CORE::AttachmentType::DepthStencil:
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
        break;
    case PC_CORE::AttachmentType::None:
    default:
        assert(false);
    }

    return {};
}

vk::ImageLayout Vulkan::VulkanRenderPass::GetImageLayoutSubPassForInputAttachement(
    PC_CORE::AttachmentType _attachmentType)
{
    switch (_attachmentType)
    {
    case PC_CORE::AttachmentType::Color:
        return vk::ImageLayout::eShaderReadOnlyOptimal;
        break;
    case PC_CORE::AttachmentType::Depth:
    case PC_CORE::AttachmentType::Stencil:
    case PC_CORE::AttachmentType::DepthStencil:
        return vk::ImageLayout::eDepthReadOnlyOptimal;
        break;
    case PC_CORE::AttachmentType::None:
    default:
        assert(false);
    }

    return {};
}

std::vector<vk::AttachmentDescription> Vulkan::VulkanRenderPass::ParseAttahchementDescription(
    const PC_CORE::RenderPassDescriptor& _renderPassDescriptor, bool _hasdepth)
{
    std::vector<vk::AttachmentDescription> vkAttachments(
        _renderPassDescriptor.attachement.size() + static_cast<size_t>(_hasdepth));

    for (uint32_t i = 0; i < _renderPassDescriptor.attachement.size(); i++)
    {
        const auto& attachment = _renderPassDescriptor.attachement[i];
        vk::AttachmentDescription& vkAttachment = vkAttachments[i];

        vkAttachment.flags = {};
        vkAttachment.format = Utils::RhiFormatToVkFormat(attachment.format);
        vkAttachment.samples = Utils::RhiSampleCountToVuklan(attachment.sampleCount);

        vkAttachment.loadOp = Utils::RhiLoadOperationToVulkan(attachment.load);
        vkAttachment.storeOp = Utils::RhiStoreOperationToVulkan(attachment.store);

        vkAttachment.stencilLoadOp = Utils::RhiLoadOperationToVulkan(attachment.stencilLoad);
        vkAttachment.stencilStoreOp = Utils::RhiStoreOperationToVulkan(attachment.stencilStore);

        vkAttachment.initialLayout = Utils::RhiImageStateToVulkanImageLayout(attachment.currentImageState);
        vkAttachment.finalLayout = Utils::RhiImageStateToVulkanImageLayout(attachment.finalImageState);
    }

    if (_hasdepth)
    {
        vk::AttachmentDescription depthDesc{};
        depthDesc.format = Utils::RhiFormatToVkFormat(_renderPassDescriptor.depthAttachment->format);
        depthDesc.samples = Utils::RhiSampleCountToVuklan(_renderPassDescriptor.depthAttachment->sampleCount);
        depthDesc.loadOp = Utils::RhiLoadOperationToVulkan(_renderPassDescriptor.depthAttachment->load);
        depthDesc.storeOp = Utils::RhiStoreOperationToVulkan(_renderPassDescriptor.depthAttachment->store);
        depthDesc.stencilLoadOp = Utils::RhiLoadOperationToVulkan(_renderPassDescriptor.depthAttachment->stencilLoad);
        depthDesc.stencilStoreOp =
            Utils::RhiStoreOperationToVulkan(_renderPassDescriptor.depthAttachment->stencilStore);

        depthDesc.initialLayout = Utils::RhiImageStateToVulkanImageLayout(
            _renderPassDescriptor.depthAttachment->currentImageState);
        depthDesc.finalLayout = Utils::RhiImageStateToVulkanImageLayout(
            _renderPassDescriptor.depthAttachment->finalImageState);

        const size_t depthIndex = vkAttachments.size() - 1;
        vkAttachments[depthIndex] = depthDesc;
    }

    return vkAttachments;
}
