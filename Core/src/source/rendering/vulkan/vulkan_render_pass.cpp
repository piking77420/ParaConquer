#include "rendering/vulkan/vulkan_render_pass.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanRenderPass::Init(const std::vector<Attachment>& _attachments)
{
    uint32_t nbrOfColorAtt = 0;
    uint32_t nbrOfDepth = 0;
    uint32_t nbrOfResolve = 0;
    ParseAttachementType(_attachments, &nbrOfColorAtt, &nbrOfDepth, &nbrOfResolve);
    
    if (nbrOfDepth > 1)
    {
        PC_LOGERROR("nbrOfDepth attachemnt is superior to one");
        throw std::runtime_error("nbrOfDepth attachemnt is superior to one");
    }

    std::vector<VkAttachmentReference> vkColorAttachmentReferences;
    vkColorAttachmentReferences.reserve(nbrOfColorAtt);
    
    std::vector<VkAttachmentReference> vkDepthAttachmentReferences;
    vkDepthAttachmentReferences.reserve(nbrOfDepth);

    std::vector<VkAttachmentDescription> vkAttachmentDescription;
    vkAttachmentDescription.resize(_attachments.size());

    for (size_t i = 0; i < _attachments.size(); i++)
    {
        VkAttachmentDescription& attachmentDescription = vkAttachmentDescription[i];

        ParseAttachementDescriptor(_attachments[i], &attachmentDescription);
        ParseAttachementReference(_attachments[i], &vkColorAttachmentReferences,&vkDepthAttachmentReferences , i);
    }

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(vkColorAttachmentReferences.size());
    subpass.pColorAttachments = vkColorAttachmentReferences.data();
    subpass.pDepthStencilAttachment = vkDepthAttachmentReferences.data();
    

    // TO DO FIX srcStageMask dstStageMask dstAccessMask to not be hardcoded
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(vkAttachmentDescription.size());
    renderPassInfo.pAttachments = vkAttachmentDescription.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    const VkResult r = vkCreateRenderPass(VulkanInterface::GetDevice().device, &renderPassInfo, nullptr, &renderPass);

    VK_CHECK_ERROR(r, "Failed to create Render Pass");
}


void VulkanRenderPass::Destroy()
{
    vkDestroyRenderPass(VulkanInterface::GetDevice().device, renderPass, nullptr);
}

void VulkanRenderPass::ParseAttachementDescriptor(const Attachment& _in, VkAttachmentDescription* _out)
{
    _out->format = _in.format;
    _out->samples = VK_SAMPLE_COUNT_1_BIT;

    _out->loadOp = _in.clearOnLoad ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    _out->storeOp = _in.write ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;

    _out->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    _out->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // TO DO FIX THIS 
    _out->initialLayout = _in.imageLayoutInit;
    _out->finalLayout = _in.imageLayoutFinal;
}

void VulkanRenderPass::ParseAttachementReference(const Attachment& _in, std::vector<VkAttachmentReference>* _outColor,
                                                 std::vector<VkAttachmentReference>* _outDepth, const size_t _index)
{
    VkAttachmentReference vkAttachmentReference;
    vkAttachmentReference.attachment = static_cast<uint32_t>(_index);
    vkAttachmentReference.layout = _in.imageLayoutRef;
    
    switch (_in.attachementIndex)
    {
    case AttachementIndex::Color00:
    case AttachementIndex::Color01:
    case AttachementIndex::Color02:
    case AttachementIndex::Color03:
    case AttachementIndex::Color04:
    case AttachementIndex::Color05:
    case AttachementIndex::Color06:
    case AttachementIndex::Color07:
    case AttachementIndex::Color08:
    case AttachementIndex::Color09:
        _outColor->push_back(vkAttachmentReference);
        break;
    case AttachementIndex::Depth:
    case AttachementIndex::Stencil:
        _outDepth->push_back(vkAttachmentReference);
        break;
    case AttachementIndex::Max:
        break;
    default: ;
    }
}

void VulkanRenderPass::ParseAttachementType(const std::vector<Attachment>& _attachments, uint32_t* nbrOfColorPass,
                                            uint32_t* nbrOfDepth, [[maybe_unused]] uint32_t* ResolveAttachments)
{
    for (size_t i = 0; i < _attachments.size(); i++)
    {
        switch (_attachments[i].attachementIndex)
        {
        case AttachementIndex::Color00:
        case AttachementIndex::Color01:
        case AttachementIndex::Color02:
        case AttachementIndex::Color03:
        case AttachementIndex::Color04:
        case AttachementIndex::Color05:
        case AttachementIndex::Color06:
        case AttachementIndex::Color07:
        case AttachementIndex::Color08:
        case AttachementIndex::Color09:
            *nbrOfColorPass = *nbrOfColorPass + 1;
            break;
        case AttachementIndex::Depth:
        case AttachementIndex::Stencil:
            *nbrOfDepth = *nbrOfDepth + 1;
            break;
        case AttachementIndex::Max:
            break;
        default: ; // NOLINT(clang-diagnostic-covered-switch-default)
        }
    }
}
