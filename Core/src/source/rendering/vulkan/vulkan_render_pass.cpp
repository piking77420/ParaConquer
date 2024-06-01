#include "rendering/vulkan/vulkan_render_pass.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanRenderPass::Init(const std::vector<Attachment>& _attachments)
{
    std::vector<VkAttachmentDescription> vkAttachmentDescription;
    vkAttachmentDescription.resize(_attachments.size());

    std::vector<VkAttachmentReference> vkAttachmentReferences;
    vkAttachmentReferences.resize(_attachments.size());
    
    for (size_t i = 0; i < _attachments.size(); i++)
    {
        VkAttachmentDescription& attachmentDescription = vkAttachmentDescription[i];
        VkAttachmentReference& attachmentReference = vkAttachmentReferences[i];

        ParseAttachementDescriptor(_attachments[i], &attachmentDescription);
        ParseAttachementReference(_attachments[i],&attachmentReference);
    }

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(vkAttachmentReferences.size());
    subpass.pColorAttachments = vkAttachmentReferences.data();

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(vkAttachmentDescription.size());
    renderPassInfo.pAttachments = vkAttachmentDescription.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    const VkResult r = vkCreateRenderPass(VulkanInterface::GetDevice().device, &renderPassInfo, nullptr, &renderPass);

    VK_CHECK_ERROR(r,"Failed to create Render Pass");

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
    _out->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    _out->finalLayout = _in.imageLayoutDes;
}

void VulkanRenderPass::ParseAttachementReference(const Attachment& _in, VkAttachmentReference* _out)
{
    
    if (_in.attachementIndex != AttachementIndex::Depth && _in.attachementIndex != AttachementIndex::Stencil
        && _in.attachementIndex != AttachementIndex::Max)
    {
        _out->attachment = static_cast<uint32_t>(_in.attachementIndex);
    }
    _out->layout = _in.imageLayoutRef;
}
