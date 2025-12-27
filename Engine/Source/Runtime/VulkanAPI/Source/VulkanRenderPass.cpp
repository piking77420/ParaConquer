#include "VulkanRenderPass.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "VulkanContext.hpp"
#include "VulkanSwapChain.hpp"

namespace Vulkan 
{

static inline vk::AttachmentDescription ParseRenderPassAttachementDescriptor(const PC_CORE::RenderPassAttachementDescriptor& RenderPassAttachementDescriptor)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::AttachmentDescription vkAttachment{};
    vkAttachment.flags = {};
    vkAttachment.format = Utils::RhiFormatToVkFormat(RenderPassAttachementDescriptor.Format);
    vkAttachment.samples = Utils::RhSampleCountToVulkan(RenderPassAttachementDescriptor.SampleCount);

    vkAttachment.loadOp = Utils::RhiLoadOperationToVulkan(RenderPassAttachementDescriptor.Load);
    vkAttachment.storeOp = Utils::RhiStoreOperationToVulkan(RenderPassAttachementDescriptor.Store);

    vkAttachment.stencilLoadOp = Utils::RhiLoadOperationToVulkan(RenderPassAttachementDescriptor.StencilLoad);
    vkAttachment.stencilStoreOp = Utils::RhiStoreOperationToVulkan(RenderPassAttachementDescriptor.StencilStore);

    vkAttachment.initialLayout = Utils::RhiResourceStateToVulkanImageLayout(RenderPassAttachementDescriptor.InitialImageState);
    vkAttachment.finalLayout = Utils::RhiResourceStateToVulkanImageLayout(RenderPassAttachementDescriptor.FinalImageState);

    return vkAttachment;
}

static inline vk::AttachmentReference ParseSubPassAttachementRef(const PC_CORE::AttachementRef& attachementRef)
{
    return vk::AttachmentReference(static_cast<uint32_t>(attachementRef.Slot), Utils::RhiResourceStateToVulkanImageLayout(attachementRef.ResourceState));
}


VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi)
    : RhiRenderPass(_Rhi)
{
   
}

VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi   , PC_CORE::RhiFormat colorFormat,
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

VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat, uint32_t _sampleCount)
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

VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi, PC_CORE::RhiFormat colorFormat)
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

VulkanRenderPass::VulkanRenderPass(PC_CORE::Rhi& _Rhi, vk::Device device, vk::Format format)
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

VulkanRenderPass::~VulkanRenderPass()
{
    
    if (m_RenderPass == VK_NULL_HANDLE)
        return;

    GET_VK_DEVICE.destroyRenderPass(m_RenderPass);
    m_RenderPass = VK_NULL_HANDLE;
}


bool VulkanRenderPass::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    RenderPassBuildContext RenderPassBuildContext;
    PopulateAttachementDescription(&RenderPassBuildContext);
    PopulateSubPassContext(&RenderPassBuildContext);
    PopulateSubPassDependies(&RenderPassBuildContext.vkSubpassDependicies);

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;

    // Attachement
    renderPassInfo.attachmentCount = static_cast<uint32_t>(RenderPassBuildContext.AttachmentDescriptions.size());
    renderPassInfo.pAttachments = RenderPassBuildContext.AttachmentDescriptions.data();

    // SubPass
    renderPassInfo.subpassCount = static_cast<uint32_t>(RenderPassBuildContext.vkSubpassDescriptions.size());
    renderPassInfo.pSubpasses = RenderPassBuildContext.vkSubpassDescriptions.data();

    // Dependencies
    renderPassInfo.dependencyCount = static_cast<uint32_t>(RenderPassBuildContext.vkSubpassDependicies.size());
    renderPassInfo.pDependencies = RenderPassBuildContext.vkSubpassDependicies.data();

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

vk::RenderPass VulkanRenderPass::GetVulkanRenderPass() const
{
    return m_RenderPass;
}

void VulkanRenderPass::PopulateAttachementDescription(RenderPassBuildContext* _RenderPassBuildContext)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::vector<vk::AttachmentDescription>& attachementDescriptions = _RenderPassBuildContext->AttachmentDescriptions;
    attachementDescriptions.resize(m_AttachementCount);

    for (size_t i = 0; i < attachementDescriptions.size(); i++)
        attachementDescriptions[i] = ParseRenderPassAttachementDescriptor(m_Attachements[i]);
}

void VulkanRenderPass::PopulateSubPassContext(RenderPassBuildContext* _RenderPassBuildContext)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    std::vector<vk::SubpassDescription>& SubpassDescriptions = _RenderPassBuildContext->vkSubpassDescriptions;
    SubpassDescriptions.resize(m_SubPassCount);

    // Count Total Color Input And Preserved Attachement
    uint32_t ColorAttachementCount = 0;
    uint32_t InputAttachementCount = 0;
    uint32_t PreservedAttachementCount = 0;
    uint32_t DepthAttachementCount = 0;
    {
        for (uint32_t i = 0; i < m_SubPassCount; i++)
        {
            const auto& SubPass = m_SubPasses[i];

            const uint32_t SubPassColorAttachementCount = static_cast<uint32_t>(SubPass.ColorAttachements.size());
            const uint32_t SubPassInputAttachementCount = static_cast<uint32_t>(SubPass.InputAttachements.size());
            const uint32_t SubPasDepthAttachement = static_cast<uint32_t>(SubPass.HasDepthAttachement());

            const uint32_t SubPassPreservedAttachementCount = m_AttachementCount -
                (SubPassColorAttachementCount + SubPassInputAttachementCount + SubPasDepthAttachement);

            ColorAttachementCount += SubPassColorAttachementCount;
            InputAttachementCount += SubPassInputAttachementCount;
            PreservedAttachementCount += SubPassPreservedAttachementCount;
            DepthAttachementCount += SubPasDepthAttachement;
        }
        
        _RenderPassBuildContext->ColorAttachmentReferences.resize(ColorAttachementCount);
        _RenderPassBuildContext->InputAttachementReferences.resize(InputAttachementCount);
        _RenderPassBuildContext->Preserved.resize(PreservedAttachementCount);
        _RenderPassBuildContext->DepthsAttachements.resize(DepthAttachementCount);
    }
    // Reset
    ColorAttachementCount = 0;
    InputAttachementCount = 0;
    PreservedAttachementCount = 0;
    DepthAttachementCount = 0;

    {
        SubpassDescriptions.resize(m_SubPassCount);
        for (uint32_t i = 0; i < m_SubPassCount; i++)
        {
            const auto& SubPass = m_SubPasses[i];
            vk::SubpassDescription& VkSubpassDescription = SubpassDescriptions[i];

            // Compute Preserved
            {
                std::set<PC_CORE::AttachementSlot> PreservedSlotsSet;
                for (size_t i = 0; i < m_AttachementCount; i++)
                    PreservedSlotsSet.emplace(static_cast<PC_CORE::AttachementSlot>(i));

                for (const PC_CORE::AttachementRef& Catt : SubPass.ColorAttachements)
                    PreservedSlotsSet.erase(Catt.Slot);
                for (const PC_CORE::AttachementRef& Iatt : SubPass.InputAttachements)
                    PreservedSlotsSet.erase(Iatt.Slot);

                if (SubPass.HasDepthAttachement())
                    PreservedSlotsSet.erase(SubPass.DepthAttachement.Slot);

                
                uint32_t PreservedLocalIndex = 0;
                for (const auto& PSlot : PreservedSlotsSet)
                {
                    _RenderPassBuildContext->InputAttachementReferences[PreservedAttachementCount + (PreservedLocalIndex++)]
                        = vk::AttachmentReference(static_cast<uint32_t>(PSlot), vk::ImageLayout::eUndefined);
                }
                PreservedAttachementCount += PreservedLocalIndex;
                    
            }
            
           
            auto PopulateDescription = [](
                const std::vector<PC_CORE::AttachementRef>& SrcAttachementRef,
                std::vector<vk::AttachmentReference>* VKAttachmentReferences,
                uint32_t* _GlobalAttachementCount,
                uint32_t* vkAttachementCountPtr,
                 const vk::AttachmentReference** ppVkAttahchmentPtr)
                {
                    uint32_t localAttCount = 0;
                    for (const auto& Catt : SrcAttachementRef)
                    {
                        (*VKAttachmentReferences).at(*_GlobalAttachementCount + localAttCount) = ParseSubPassAttachementRef(Catt);
                        localAttCount++;
                    }
                    *ppVkAttahchmentPtr = VKAttachmentReferences->data() + *_GlobalAttachementCount;
                    *vkAttachementCountPtr = localAttCount;

                    *_GlobalAttachementCount = *_GlobalAttachementCount + localAttCount;
                };

            PopulateDescription(
                m_SubPasses[i].ColorAttachements,
                &_RenderPassBuildContext->ColorAttachmentReferences,
                &ColorAttachementCount,
                &VkSubpassDescription.colorAttachmentCount,
                &VkSubpassDescription.pColorAttachments);

            PopulateDescription(
                m_SubPasses[i].InputAttachements,
                &_RenderPassBuildContext->InputAttachementReferences,
                &InputAttachementCount,
                &VkSubpassDescription.inputAttachmentCount,
                &VkSubpassDescription.pInputAttachments);

            if (m_SubPasses[i].HasDepthAttachement())
            {
                _RenderPassBuildContext->DepthsAttachements[DepthAttachementCount] = ParseSubPassAttachementRef(m_SubPasses[i].DepthAttachement);
                VkSubpassDescription.pDepthStencilAttachment = &_RenderPassBuildContext->DepthsAttachements[DepthAttachementCount++];
            }
        }

    }
    
    
}

void VulkanRenderPass::PopulateSubPassDependies(std::vector<vk::SubpassDependency>* _RenderPassBuildContext)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::vector<vk::SubpassDependency>& RenderPassBuildContext = *_RenderPassBuildContext;

    std::array<std::array<RhiResourceState, MaxSubPass>, MaxAttachementSlot>
        attachementSlotLifeCycle{};
    for (std::array<RhiResourceState, MaxSubPass>& slot : attachementSlotLifeCycle)
        slot.fill(RhiResourceState::Undefined);
    

    // FIll map
    for (size_t i = 0; i < m_SubPassCount; i++)
    {
        // Color
        for (size_t j = 0; j < m_SubPasses[i].ColorAttachements.size(); j++)
        {
            const PC_CORE::AttachementRef ref = m_SubPasses[i].ColorAttachements[j];
            attachementSlotLifeCycle[static_cast<uint32_t>(ref.Slot)][i] = ref.ResourceState;
        }

        // Inout
        for (size_t j = 0; j < m_SubPasses[i].InputAttachements.size(); j++)
        {
            const PC_CORE::AttachementRef ref = m_SubPasses[i].InputAttachements[j];
            attachementSlotLifeCycle[static_cast<uint32_t>(ref.Slot)][i] = ref.ResourceState;
        }

        // Depth
        if (m_SubPasses[i].HasDepthAttachement())
        {
            const PC_CORE::AttachementRef ref = m_SubPasses[i].DepthAttachement;
            attachementSlotLifeCycle[static_cast<uint32_t>(ref.Slot)][i] = ref.ResourceState;
        }
    }

    const size_t nbrOfDependencies = m_SubPassCount + 1;
    for (size_t i = 0; i < nbrOfDependencies; i++)
    {
        const uint32_t srcPass = (i == 0) ? VK_SUBPASS_EXTERNAL : (i - 1);

        const uint32_t dstPass = (i == m_SubPassCount)  ? VK_SUBPASS_EXTERNAL : i;

        auto ComputeMaskDepencies = [&](PC_CORE::AttachementRef AttachementRef)
            {
                RenderPassBuildContext.push_back(ComputeMask
                (
                    attachementSlotLifeCycle,
                    i,
                    AttachementRef,
                    srcPass,
                    dstPass
                ));
            };

        for (const auto& Catt : m_SubPasses[i].ColorAttachements)
            ComputeMaskDepencies(Catt);

        for (const auto& Iatt : m_SubPasses[i].InputAttachements)
            ComputeMaskDepencies(Iatt);

        if (m_SubPasses[i].HasDepthAttachement())
            ComputeMaskDepencies(m_SubPasses[i].DepthAttachement);

    }
}


vk::SubpassDependency VulkanRenderPass::ComputeMask
(
    const std::array<std::array<RhiResourceState, MaxSubPass>, MaxAttachementSlot>& usageTable,
    uint32_t _SubPassIndex,
    PC_CORE::AttachementRef _AttachementRef,
    uint32_t _SrcSubPassIndex,
    uint32_t _DstSubPasIndex
)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::SubpassDependency dependicies;
    dependicies.srcSubpass = _SrcSubPassIndex;
    dependicies.dstSubpass = _DstSubPasIndex;
    dependicies.dependencyFlags = {};


    const PC_CORE::AttachementSlot slot = _AttachementRef.Slot;
    const size_t slotIndex = static_cast<size_t>(_AttachementRef.Slot);
    const PC_CORE::RenderPassAttachementDescriptor& RenderPassAttachementDescriptor = m_Attachements[slotIndex];    

    if (_SrcSubPassIndex != VK_SUBPASS_EXTERNAL &&
        _DstSubPasIndex != VK_SUBPASS_EXTERNAL)
    {
        const RhiResourceState srcState = usageTable[slotIndex][_SrcSubPassIndex];

        const RhiResourceState dstState = usageTable[slotIndex][_DstSubPasIndex];

        std::pair<vk::AccessFlags, vk::PipelineStageFlags> Src = AttachementDependencies(srcState, RenderPassAttachementDescriptor);
        std::pair<vk::AccessFlags, vk::PipelineStageFlags> Dst = AttachementDependencies(dstState, RenderPassAttachementDescriptor);

        dependicies.srcAccessMask = Src.first;
        dependicies.srcStageMask = Src.second;

        dependicies.dstAccessMask = Dst.first;
        dependicies.dstStageMask = Dst.second;
    }
    else if (_SrcSubPassIndex == VK_SUBPASS_EXTERNAL)
    {
        const RhiResourceState dstState = usageTable[slotIndex][_DstSubPasIndex];

        std::pair<vk::AccessFlags, vk::PipelineStageFlags> Dst = AttachementDependencies(dstState, RenderPassAttachementDescriptor);

        dependicies.srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
        dependicies.srcAccessMask = {};

        dependicies.dstStageMask = Dst.second;
        dependicies.dstAccessMask = Dst.first;
    }
    else if (_DstSubPasIndex == VK_SUBPASS_EXTERNAL)
    {
        const RhiResourceState srcState = usageTable[slotIndex][_SrcSubPassIndex];

        std::pair<vk::AccessFlags, vk::PipelineStageFlags> Src = AttachementDependencies(srcState, RenderPassAttachementDescriptor);

        dependicies.srcStageMask = Src.second;
        dependicies.srcAccessMask = Src.first;

        dependicies.dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
        dependicies.dstAccessMask = {};
    }
    else
    {
        // Dont exist
    }
   
    return dependicies;
}

std::pair<vk::AccessFlags, vk::PipelineStageFlags> VulkanRenderPass::AttachementDependencies(const PC_CORE::RhiResource::State _State, const PC_CORE::RenderPassAttachementDescriptor& RenderPassAttachementDescriptor)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::pair<vk::AccessFlags, vk::PipelineStageFlags> out{};
    vk::AccessFlags& accesFlag = out.first;
    vk::PipelineStageFlags& stage = out.second;

    switch (_State)
    {
    case PC_CORE::RhiResource::State::Undefined: // no dependcies
        stage |= vk::PipelineStageFlagBits::eTopOfPipe;
        break;
    case PC_CORE::RhiResource::State::CopySrc:
        accesFlag |= vk::AccessFlagBits::eTransferRead;
        stage |= vk::PipelineStageFlagBits::eTransfer;
        break;
    case PC_CORE::RhiResource::State::CopyDst:
        accesFlag |= vk::AccessFlagBits::eTransferWrite;
        stage |= vk::PipelineStageFlagBits::eTransfer;
        break;
    case PC_CORE::RhiResource::State::VertexShaderResource:
        accesFlag |= vk::AccessFlagBits::eShaderRead;
        stage |= vk::PipelineStageFlagBits::eVertexShader;
        break;
    case PC_CORE::RhiResource::State::FragmentShaderResource:
    {
        accesFlag |= vk::AccessFlagBits::eShaderRead;
        stage |= vk::PipelineStageFlagBits::eFragmentShader;
    }
        break;
    case PC_CORE::RhiResource::State::RenderTarget:
        accesFlag |= vk::AccessFlagBits::eColorAttachmentWrite;
        stage |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
        break;
    case PC_CORE::RhiResource::State::DepthStencilWrite:
        accesFlag |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        stage |= vk::PipelineStageFlagBits::eEarlyFragmentTests |
            vk::PipelineStageFlagBits::eLateFragmentTests;
        break;
    case PC_CORE::RhiResource::State::DepthStencilRead:
        accesFlag |= vk::AccessFlagBits::eDepthStencilAttachmentRead;
        stage |= vk::PipelineStageFlagBits::eEarlyFragmentTests |
            vk::PipelineStageFlagBits::eLateFragmentTests;
        break;
    case PC_CORE::RhiResource::State::ComputeRead:
        accesFlag |= vk::AccessFlagBits::eShaderRead;
        stage |= vk::PipelineStageFlagBits::eComputeShader;
        break;
    case PC_CORE::RhiResource::State::ComputeWrite:
        accesFlag |= vk::AccessFlagBits::eShaderWrite;
        stage |= vk::PipelineStageFlagBits::eComputeShader;
        break;
    case PC_CORE::RhiResource::State::ComputeReadWrite:
        accesFlag |= vk::AccessFlagBits::eShaderRead |
            vk::AccessFlagBits::eShaderWrite;
        stage |= vk::PipelineStageFlagBits::eComputeShader;
        break;
    case PC_CORE::RhiResource::State::Present:
        stage |= vk::PipelineStageFlagBits::eBottomOfPipe;
        break;
    case PC_CORE::RhiResource::State::VertexBuffer:
    case PC_CORE::RhiResource::State::IndexBuffer:
    case PC_CORE::RhiResource::State::UniformBuffer:
    default:
        assert(false && "Unsupported AttachementDependencies");
        break;
    }

    return out;
}

}