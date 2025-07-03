#include "vulkan_render_pass.hpp"

#include "utils/rhi_vulkan_parser.hpp"
#include "vulkan_context.hpp"
#include "vulkan_swap_chain.hpp"

Vulkan::VulkanRenderPass::VulkanRenderPass(const PC_CORE::RenderPassDescriptor& _renderPassDescriptor)
    : RhiRenderPass(_renderPassDescriptor)
{
    PERF_REGION_SCOPED;
    
    std::shared_ptr<Vulkan::VulkanDevice> device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

    assert(_renderPassDescriptor.colorAttachement.size() < MAX_COLOR_ATTACHMENTS && "MAX_COLOR_ATTACHMENTS has been reached");
    
    bool hasDephtAttachments = _renderPassDescriptor.depthAttachment != nullptr;

    // ColorAttachement
    std::vector<vk::AttachmentDescription> attachments(_renderPassDescriptor.colorAttachement.size() + static_cast<size_t>(hasDephtAttachments));
    for (uint32_t i = 0; i < _renderPassDescriptor.colorAttachement.size(); i++)
    {
        const auto& attachment = _renderPassDescriptor.colorAttachement[i];
        vk::AttachmentDescription& vkAttachment = attachments[i];

        vkAttachment.flags = {};
        vkAttachment.format = Utils::RHIFormatToVkFormat(attachment.format);
        vkAttachment.samples = Utils::RhiSampleCountToVuklan(attachment.sampleCount);
        
        vkAttachment.loadOp = Utils::RhiLoadOperationToVulkan(attachment.load);
        vkAttachment.storeOp = Utils::RhiStoreOperationToVulkan(attachment.store);
        
        vkAttachment.stencilLoadOp = Utils::RhiLoadOperationToVulkan(attachment.stencilLoad);
        vkAttachment.stencilStoreOp = Utils::RhiStoreOperationToVulkan(attachment.stencilStore);
        vkAttachment.initialLayout = vk::ImageLayout::eUndefined;
        
        ParseAttachmentLayout(_renderPassDescriptor.colorAttachement[i], &attachments[i]);
    }

    if (hasDephtAttachments)
    {
        // Depth
        vk::AttachmentDescription dephtAttachments;
        dephtAttachments.flags = {};
        dephtAttachments.format = Utils::RHIFormatToVkFormat(_renderPassDescriptor.depthAttachment->format);
        dephtAttachments.samples = Utils::RhiSampleCountToVuklan(_renderPassDescriptor.depthAttachment->sampleCount);
        
        dephtAttachments.loadOp = Utils::RhiLoadOperationToVulkan(_renderPassDescriptor.depthAttachment->load);
        dephtAttachments.storeOp = Utils::RhiStoreOperationToVulkan(_renderPassDescriptor.depthAttachment->store);
        
        dephtAttachments.stencilLoadOp = Utils::RhiLoadOperationToVulkan(_renderPassDescriptor.depthAttachment->stencilLoad);
        dephtAttachments.stencilStoreOp = Utils::RhiStoreOperationToVulkan(_renderPassDescriptor.depthAttachment->stencilStore);
        dephtAttachments.initialLayout = vk::ImageLayout::eUndefined;

        // the last attachement is alway the depth is the render use it
        attachments[attachments.size() - 1] = dephtAttachments;
        ParseAttachmentLayout(*_renderPassDescriptor.depthAttachment, &attachments[attachments.size() - 1]);
    }


    // Prepare Attachement Reference
    std::vector<vk::AttachmentReference> attachmentReferences;

    vk::AttachmentReference dephtAttachmentReferences;
    if (hasDephtAttachments)
    {
        assert(_renderPassDescriptor.depthAttachment->attachmentType == PC_CORE::AttachmentType::DepthStencil ||
            _renderPassDescriptor.depthAttachment->attachmentType == PC_CORE::AttachmentType::Depth && "invalid attachementType");
        
        dephtAttachmentReferences.layout = GetImageLayoutSubPass(_renderPassDescriptor.depthAttachment->attachmentType);
        dephtAttachmentReferences.attachment = static_cast<uint32_t>(attachments.size() - 1);
    }
    
    // Prepare subpasses
    std::vector<vk::SubpassDescription> subPasses(_renderPassDescriptor.subPasses.size());
    
    std::vector<uint32_t> subpassPreserved;
    size_t totalAttachmentRefs = 0;
    size_t totalPreservedAttachments = 0;

    // Count for preallocation
    for (const auto& subpass : _renderPassDescriptor.subPasses)
    {
        totalAttachmentRefs += subpass.colorAttachementDescriptorIndicies.size();
        totalPreservedAttachments += _renderPassDescriptor.colorAttachement.size() - subpass.colorAttachementDescriptorIndicies.size();
    }

    attachmentReferences.resize(totalAttachmentRefs);
    subpassPreserved.resize(totalPreservedAttachments);

    size_t refIndex = 0;
    size_t preservedIndex = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(_renderPassDescriptor.subPasses.size()); i++)
    {
        const auto& subpass = _renderPassDescriptor.subPasses[i];

        assert(subpass.useDepth == hasDephtAttachments && "Subpass use dpeth but no depth attachement provided");

        //Copy indicies into set
        std::set<uint32_t> usedAttachments(subpass.colorAttachementDescriptorIndicies.begin(),
                                           subpass.colorAttachementDescriptorIndicies.end());

        // Preserved attachments
        for (uint32_t att = 0; att < static_cast<uint32_t>(_renderPassDescriptor.colorAttachement.size()); att++)
        {
            // push unsed indicies into memory
            if (!usedAttachments.contains(att))
            {
                subpassPreserved[preservedIndex++] = att;
            }
        }

        // Attachment references
        for (uint32_t j = 0; j < subpass.colorAttachementDescriptorIndicies.size(); j++)
        {
            uint32_t attachmentIndex = subpass.colorAttachementDescriptorIndicies[j];
            
            attachmentReferences[refIndex].attachment = attachmentIndex;
            attachmentReferences[refIndex].layout = GetImageLayoutSubPass(_renderPassDescriptor.colorAttachement[attachmentIndex].attachmentType);
            ++refIndex;
        }
    }

    // Assign to vk::SubpassDescription
    uint32_t refOffset = 0;
    uint32_t preserveOffset = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(_renderPassDescriptor.subPasses.size()); i++)
    {
        const PC_CORE::SubPassDescription& subPass = _renderPassDescriptor.subPasses[i];
        vk::SubpassDescription& vkSubpass = subPasses[i];

        vkSubpass.flags = {};
        vkSubpass.pipelineBindPoint = Utils::RhiPipelineBindPointToVulkan(subPass.shaderProgramPipelineType);

        vkSubpass.colorAttachmentCount = subPass.colorAttachementDescriptorIndicies.size();
        vkSubpass.pColorAttachments = &attachmentReferences[preservedIndex];
        vkSubpass.pDepthStencilAttachment = subPass.useDepth ?  &dephtAttachmentReferences : nullptr;
    
        vkSubpass.preserveAttachmentCount = static_cast<uint32_t>(_renderPassDescriptor.colorAttachement.size() - subPass.colorAttachementDescriptorIndicies.size());
        vkSubpass.pPreserveAttachments = subpassPreserved.data() + preserveOffset;
        
        refOffset += subPass.colorAttachementDescriptorIndicies.size();
        preserveOffset += vkSubpass.preserveAttachmentCount;
    }

    std::vector<vk::SubpassDependency> dependencies;
    dependencies.resize(subPasses.size());
    
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0u;
    dependencies[0].srcStageMask = {};
    dependencies[0].dstStageMask = {};
    dependencies[0].srcAccessMask = {};
    dependencies[0].dstAccessMask = {};
    dependencies[0].dependencyFlags = {};
    ParseDependcies(_renderPassDescriptor.subPasses[0].subPassDependcies, &dependencies[0]);
    for (uint32_t i = 1; i < dependencies.size(); i++) // start from 1
    {
        vk::SubpassDependency& dep = dependencies[i];

        dep.srcSubpass = i - 1;
        dep.dstSubpass = i;
        ParseDependcies(_renderPassDescriptor.subPasses[i].subPassDependcies, &dep);
        dep.dependencyFlags = {};
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
    
    vk::AttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = Utils::RHIFormatToVkFormat(colorFormat);
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

void Vulkan::VulkanRenderPass::ParseAttachmentLayout(const PC_CORE::RenderPassAttachementDescriptor& _attachment,
                                                     vk::AttachmentDescription* _attachmentLayouts)
{
    bool doesReadValue  = _attachment.load == PC_CORE::LoadOperation::Load;
    bool doesWriteValue = _attachment.store == PC_CORE::StoreOperation::Store;
    bool isInputOnly    = (doesReadValue && !doesWriteValue);

    switch (_attachment.attachmentType)
    {
    case PC_CORE::AttachmentType::Color:
        if (isInputOnly)
        {
            _attachmentLayouts->finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else if (doesWriteValue)
        {
            _attachmentLayouts->finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else if (doesReadValue)
        {
            _attachmentLayouts->finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else
        {
            _attachmentLayouts->finalLayout = vk::ImageLayout::eUndefined;
        }
        break;

    case PC_CORE::AttachmentType::Depth:
    case PC_CORE::AttachmentType::Stencil:
    case PC_CORE::AttachmentType::DepthStencil:
        if (isInputOnly)
        {
            // Si tu as activé la feature `separateDepthStencilLayouts`, tu peux utiliser des layouts plus spécifiques ici
            _attachmentLayouts->finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else if (doesWriteValue)
        {
            _attachmentLayouts->finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        }
        else if (doesReadValue)
        {
            _attachmentLayouts->finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else
        {
            _attachmentLayouts->finalLayout = vk::ImageLayout::eUndefined;
        }
        break;

    default:
        _attachmentLayouts->finalLayout = vk::ImageLayout::eUndefined;
        break;
    }
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
}
