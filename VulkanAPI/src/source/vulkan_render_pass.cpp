#include "vulkan_render_pass.hpp"

#include "utils/rhi_vulkan_parser.hpp"
#include "vulkan_context.hpp"
#include "vulkan_swap_chain.hpp"

Vulkan::VulkanRenderPass::VulkanRenderPass(const PC_CORE::RenderPassDescriptor& _attachments)
    : RhiRenderPass(_attachments)
{
    std::shared_ptr<Vulkan::VulkanDevice> device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

    assert(_attachments.attachments.size() < MAX_COLOR_ATTACHMENTS && "MAX_COLOR_ATTACHMENTS has been reached");

    std::vector<vk::AttachmentDescription> attachments(_attachments.attachments.size());
    uint32_t depthAttachmentIndex = std::numeric_limits<uint32_t>::max();

    for (uint32_t i = 0; i < _attachments.attachments.size(); i++)
    {
        const auto& attachment = _attachments.attachments[i];
        vk::AttachmentDescription& vkAttachment = attachments[i];

        vkAttachment.flags = {};
        vkAttachment.format = Utils::RHIFormatToVkFormat(attachment.format);
        vkAttachment.samples = Utils::RhiSampleCountToVuklan(attachment.sampleCount);
        vkAttachment.loadOp = Utils::RhiLoadOperationToVulkan(attachment.load);
        vkAttachment.storeOp = Utils::RhiStoreOperationToVulkan(attachment.store);
        vkAttachment.stencilLoadOp = Utils::RhiLoadOperationToVulkan(attachment.stencilLoad);
        vkAttachment.stencilStoreOp = Utils::RhiStoreOperationToVulkan(attachment.stencilStore);
        vkAttachment.initialLayout = vk::ImageLayout::eUndefined; // TODO

        if (attachment.attachmentType == PC_CORE::AttachmentType::Depth ||
            attachment.attachmentType == PC_CORE::AttachmentType::Stencil)
        {
            assert(i == _attachments.attachments.size() - 1 && "Depth or Stencil attachment must be at the end");
            assert(depthAttachmentIndex == std::numeric_limits<uint32_t>::max() && "Only one depth attachment allowed");
            depthAttachmentIndex = i;
        }

        ParseAttachmentLayout(attachment, &vkAttachment);
    }

    // Prepare subpasses
    std::vector<vk::SubpassDescription> subPasses(_attachments.subPasses.size());
    std::vector<vk::AttachmentReference> attachmentReferences;
    std::vector<uint32_t> subpassPreserved;

    std::vector<bool> subpassUsesDepth(_attachments.subPasses.size(), false);
    size_t totalAttachmentRefs = 0;
    size_t totalPreservedAttachments = 0;

    // Count for preallocation
    for (const auto& subpass : _attachments.subPasses)
    {
        totalAttachmentRefs += subpass.attachmentDescriptorIndices.size();
        totalPreservedAttachments += _attachments.attachments.size() - subpass.attachmentDescriptorIndices.size();
    }

    attachmentReferences.resize(totalAttachmentRefs);
    subpassPreserved.resize(totalPreservedAttachments);

    size_t refIndex = 0;
    size_t preservedIndex = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(_attachments.subPasses.size()); i++)
    {
        const auto& subpass = _attachments.subPasses[i];
        std::set<uint32_t> usedAttachments(subpass.attachmentDescriptorIndices.begin(),
                                           subpass.attachmentDescriptorIndices.end());

        // Preserved attachments
        for (uint32_t att = 0; att < static_cast<uint32_t>(_attachments.attachments.size()); att++)
        {
            if (usedAttachments.find(att) == usedAttachments.end())
            {
                subpassPreserved[preservedIndex++] = att;
            }
        }

        // Attachment references
        for (uint32_t j = 0; j < subpass.attachmentDescriptorIndices.size(); j++)
        {
            uint32_t attachmentIndex = subpass.attachmentDescriptorIndices[j];

            if (attachmentIndex == depthAttachmentIndex)
            {
                subpassUsesDepth[i] = true;
            }

            attachmentReferences[refIndex].attachment = attachmentIndex;
            attachmentReferences[refIndex].layout = attachments[attachmentIndex].finalLayout;
            ++refIndex;
        }
    }

    // Assign to vk::SubpassDescription
    uint32_t refOffset = 0;
    uint32_t preserveOffset = 0;
    std::vector<vk::AttachmentReference> depthReferences(_attachments.subPasses.size());

    for (uint32_t i = 0; i < static_cast<uint32_t>(_attachments.subPasses.size()); i++)
    {
        vk::SubpassDescription& vkSubpass = subPasses[i];
        const auto& subpass = _attachments.subPasses[i];

        vkSubpass.flags = {};
        vkSubpass.pipelineBindPoint = Utils::RhiPipelineBindPointToVulkan(subpass.shaderProgramPipelineType);

        vkSubpass.inputAttachmentCount = subpass.attachmentDescriptorIndices.size();
        vkSubpass.pInputAttachments = attachmentReferences.data() + refOffset;

        if (subpassUsesDepth[i])
        {
            depthReferences[i].attachment = depthAttachmentIndex;
            depthReferences[i].layout = attachments[depthAttachmentIndex].finalLayout;
            vkSubpass.pDepthStencilAttachment = &depthReferences[i];
        }
        else
        {
            vkSubpass.pDepthStencilAttachment = nullptr;
        }

        vkSubpass.preserveAttachmentCount = _attachments.attachments.size() - subpass.attachmentDescriptorIndices.
            size();
        vkSubpass.pPreserveAttachments = subpassPreserved.data() + preserveOffset;

        refOffset += subpass.attachmentDescriptorIndices.size();
        preserveOffset += vkSubpass.preserveAttachmentCount;
    }

    std::vector<vk::SubpassDependency> dependencies;
    dependencies.resize(subPasses.size());

    dependencies[0] = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0u,
        .srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits::eMemoryRead,
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    };

    for (uint32_t i = 1; i < dependencies.size(); i++) // start from 1
    {
        vk::SubpassDependency& dep = dependencies[i];

        dep.srcSubpass = i - 1;
        dep.dstSubpass = i;

        dep.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dep.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;

        dep.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        dep.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        dep.dependencyFlags = vk::DependencyFlagBits::eByRegion;
    }

    vk::RenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = static_cast<uint32_t>(subPasses.size());
    renderPassInfo.pSubpasses = subPasses.data();
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();


    m_RenderPass = device->GetDevice().createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::RHIFormat colorFormat,
                                           PC_CORE::RHIFormat depthFormat) : PC_CORE::RhiRenderPass(
    colorFormat, depthFormat)
{
    std::shared_ptr<Vulkan::VulkanDevice> device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = static_cast<VkFormat>(Utils::RHIFormatToVkFormat(colorFormat));
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = static_cast<VkFormat>(Utils::RHIFormatToVkFormat(depthFormat));
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

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::RHIFormat colorFormat, uint32_t _sampleCount)
{
    std::shared_ptr<Vulkan::VulkanDevice> device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = Utils::RHIFormatToVkFormat(colorFormat);
    colorAttachment.samples = Utils::RhiSampleCountToVuklan(_sampleCount);
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

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

Vulkan::VulkanRenderPass::VulkanRenderPass(PC_CORE::RHIFormat colorFormat)
{
    std::shared_ptr<Vulkan::VulkanDevice> device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = Utils::RHIFormatToVkFormat(colorFormat);
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
    std::shared_ptr<Vulkan::VulkanDevice> device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(
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

void Vulkan::VulkanRenderPass::ParseAttachmentLayout(const PC_CORE::AttachementDescriptor& _attachment,
                                                     vk::AttachmentDescription* _attachmentLayouts)
{
    bool doesReadValue = _attachment.load == PC_CORE::LoadOperation::Load;
    bool doesWriteValue = _attachment.store == PC_CORE::StoreOperation::Store;

    switch (_attachment.attachmentType)
    {
    case PC_CORE::AttachmentType::Color:

        if (doesWriteValue)
            _attachmentLayouts->finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
        else if (doesReadValue)
            _attachmentLayouts->finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        else
            _attachmentLayouts->finalLayout = vk::ImageLayout::eUndefined;

        break;
    case PC_CORE::AttachmentType::Depth:

        if (doesWriteValue)
            _attachmentLayouts->finalLayout = vk::ImageLayout::eDepthAttachmentOptimal;
        else if (doesReadValue)
            _attachmentLayouts->finalLayout = vk::ImageLayout::eDepthReadOnlyOptimal;
        else
            _attachmentLayouts->finalLayout = vk::ImageLayout::eUndefined;

        break;
    case PC_CORE::AttachmentType::Stencil:
        if (doesWriteValue)
            _attachmentLayouts->finalLayout = vk::ImageLayout::eStencilAttachmentOptimal;
        else if (doesReadValue)
            _attachmentLayouts->finalLayout = vk::ImageLayout::eStencilReadOnlyOptimal;
        else
            _attachmentLayouts->finalLayout = vk::ImageLayout::eUndefined;

        break;
    case PC_CORE::AttachmentType::DepthStencil:
        if (doesWriteValue)
            _attachmentLayouts->finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        else if (doesReadValue)
            _attachmentLayouts->finalLayout = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
        else
            _attachmentLayouts->finalLayout = vk::ImageLayout::eUndefined;
        break;
    default: ;
    }
}
