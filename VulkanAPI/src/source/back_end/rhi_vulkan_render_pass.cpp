#include "back_end/rhi_vulkan_render_pass.hpp"

#include "rhi_vulkan_parser.hpp"

PC_CORE::RenderPassHandle Vulkan::Backend::CreateRenderPass(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, vk::Device _device)
{
     std::vector<vk::AttachmentDescription> attachmentDescriptions{};
    attachmentDescriptions.resize(_renderPassCreateInfo.attachmentDescriptions.size());
    
    for (size_t i = 0; i < _renderPassCreateInfo.attachmentDescriptions.size(); i++)
    {
        attachmentDescriptions[i].format = RHIFormatToVkFormat(_renderPassCreateInfo.attachmentDescriptions[i].format);  
        attachmentDescriptions[i].samples = vk::SampleCountFlagBits::e1;
            
        switch (_renderPassCreateInfo.attachmentDescriptions[i].renderPassTargetType)
        {
        case PC_CORE::AttachementUsage::COLOR:
            attachmentDescriptions[i].loadOp = vk::AttachmentLoadOp::eClear;
            attachmentDescriptions[i].storeOp = vk::AttachmentStoreOp::eStore;
                
            attachmentDescriptions[i].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            attachmentDescriptions[i].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            
            attachmentDescriptions[i].initialLayout = vk::ImageLayout::eUndefined;
            attachmentDescriptions[i].finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
            break;
        case PC_CORE::AttachementUsage::DEPTH:
        case PC_CORE::AttachementUsage::STENCIL:
            attachmentDescriptions[i].loadOp = vk::AttachmentLoadOp::eClear;
            attachmentDescriptions[i].storeOp = vk::AttachmentStoreOp::eStore;
                
            attachmentDescriptions[i].stencilLoadOp = vk::AttachmentLoadOp::eClear;
            attachmentDescriptions[i].stencilStoreOp = vk::AttachmentStoreOp::eStore;
            
            attachmentDescriptions[i].initialLayout = vk::ImageLayout::eUndefined;
            attachmentDescriptions[i].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            break;
        case PC_CORE::AttachementUsage::NONE:
            break;
        default: ;
        }
    }

    const uint32_t subPassesSize = static_cast<uint32_t>(_renderPassCreateInfo.subPasses.size());
    
    std::vector<vk::SubpassDescription> subpasses{};
    subpasses.resize(subPassesSize);

    std::vector<vk::SubpassDependency> dependencies{};
    dependencies.resize(subPassesSize);

    std::vector<vk::AttachmentReference> colorAttachmentRef; 
    std::vector<vk::AttachmentReference> depthAttachmentRef;
    
    uint32_t colorAttachementCount = 0;
    uint32_t depthAttachmentCount = 0;
    for (size_t i = 0; i < subPassesSize; i++)
    {
        const PC_CORE::SubPasse& subPasse = _renderPassCreateInfo.subPasses[i];
        for (size_t j = 0; j < subPasse.attachementUsage.size(); j++)
        {
            for (size_t j = 0; j < subPasse.attachementUsage.size(); j++)
            {
                switch (subPasse.attachementUsage[j])
                {
                case PC_CORE::AttachementUsage::COLOR:
                    colorAttachementCount++;
                    break;
                case PC_CORE::AttachementUsage::DEPTH:
                case PC_CORE::AttachementUsage::STENCIL:
                    depthAttachmentCount++;
                case PC_CORE::AttachementUsage::NONE:
                case PC_CORE::AttachementUsage::COUNT:
                    break;
                }
            }
        }
    }
    colorAttachmentRef.resize(static_cast<uint32_t>(colorAttachmentRef.size()) + colorAttachementCount);
    depthAttachmentRef.resize(static_cast<uint32_t>(depthAttachmentRef.size()) + depthAttachmentCount);

    // Reseting
    colorAttachementCount = 0;
    depthAttachmentCount = 0;
    
    // For each SubPass
    for (size_t i = 0; i < subPassesSize; i++)
    {
        const PC_CORE::SubPasse& subPasse = _renderPassCreateInfo.subPasses[i];
        vk::SubpassDescription& subpassDescription = subpasses[i];
        subpassDescription.pipelineBindPoint = RhiPipelineBindPointToVulkan(_renderPassCreateInfo.subPasses.at(i).shaderProgramPipelineType);
        
        // For each Attachement ref in subpass
        uint32_t surpassLocalColorAttachmentCount = 0;
        uint32_t surpassLocalDepthAttachmentCount = 0;
        
        for (size_t j = 0; j < subPasse.attachementUsage.size(); j++)
        {
            const PC_CORE::AttachementUsage& attachmentDescription =  subPasse.attachementUsage[j];
            vk::AttachmentReference* attachmentReference = nullptr;
            switch (attachmentDescription)
            {
            case PC_CORE::AttachementUsage::COLOR:
                attachmentReference = &colorAttachmentRef[colorAttachementCount + j];
                surpassLocalColorAttachmentCount++;

                attachmentReference->layout = vk::ImageLayout::eColorAttachmentOptimal;
                break;
            case PC_CORE::AttachementUsage::DEPTH:
            case PC_CORE::AttachementUsage::STENCIL:
                attachmentReference = &depthAttachmentRef[depthAttachmentCount + j];
                surpassLocalDepthAttachmentCount++;

                attachmentReference->layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                attachmentReference->layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                break;
            case PC_CORE::AttachementUsage::NONE:
            case PC_CORE::AttachementUsage::COUNT:
                break;
            }
            attachmentReference->attachment = surpassLocalColorAttachmentCount;
            surpassLocalColorAttachmentCount++;
        }

        subpassDescription.colorAttachmentCount = surpassLocalColorAttachmentCount;
        subpassDescription.pColorAttachments = colorAttachmentRef.data() + colorAttachementCount - 1;
        if (surpassLocalDepthAttachmentCount > 1)
        {
            throw std::runtime_error("depth attachment count greater than 1 is not supported");
        }
        subpassDescription.pDepthStencilAttachment = depthAttachmentRef.data() + depthAttachmentCount - 1;
        
        vk::SubpassDependency& dependency = dependencies[i];
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        if (!colorAttachmentRef.empty())
        {
            dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
        else
        {
            dependency.srcStageMask = {};
        }
        dependency.srcAccessMask = {};
        
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    }

    vk::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassCreateInfo.pSubpasses = subpasses.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassCreateInfo.pDependencies = dependencies.data();


    PC_CORE::RenderPassHandle renderPassHandle = CastObjectToVkObject<vk::RenderPass>(_device.createRenderPass(renderPassCreateInfo, nullptr));
    
    return renderPassHandle;
}
