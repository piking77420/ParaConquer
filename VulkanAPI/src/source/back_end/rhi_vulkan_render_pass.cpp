#include "back_end/rhi_vulkan_render_pass.hpp"

#include "rhi_vulkan_parser.hpp"

PC_CORE::RenderPassHandle Vulkan::Backend::CreateRenderPass(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, vk::Device _device)
{
	
	RenderPassDescription renderPassDescription = {};

	std::vector<vk::AttachmentDescription> attachmentDescriptions{};
	ResolveAttachementDescriptrion(_renderPassCreateInfo, attachmentDescriptions);


	CountSubPasses(_renderPassCreateInfo, &renderPassDescription);
	ResolveSubPasses(_renderPassCreateInfo, &renderPassDescription);

	vk::RenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = vk::StructureType::eRenderPassCreateInfo;
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
	renderPassCreateInfo.subpassCount = static_cast<uint32_t>(renderPassDescription.subpassesDescription.size());
	renderPassCreateInfo.pSubpasses = renderPassDescription.subpassesDescription.data();
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(renderPassDescription.dependencies.size());
	renderPassCreateInfo.pDependencies = renderPassDescription.dependencies.data();


	vk::RenderPass renderpass = _device.createRenderPass(renderPassCreateInfo, nullptr);

	return *reinterpret_cast<PC_CORE::RenderPassHandle*>(&renderpass);
}

void Vulkan::Backend::ResolveAttachementDescriptrion(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, std::vector<vk::AttachmentDescription>& _vkAttachmentDescriptions)
{
	_vkAttachmentDescriptions.resize(_renderPassCreateInfo.attachmentDescriptions.size());

	for (size_t i = 0; i < _renderPassCreateInfo.attachmentDescriptions.size(); i++)
	{
		_vkAttachmentDescriptions[i].format = RHIFormatToVkFormat(_renderPassCreateInfo.attachmentDescriptions[i].format);
		_vkAttachmentDescriptions[i].samples = vk::SampleCountFlagBits::e1;

		switch (_renderPassCreateInfo.attachmentDescriptions[i].attachementUsage)
		{
		case PC_CORE::AttachementUsage::COLOR:
			_vkAttachmentDescriptions[i].loadOp = vk::AttachmentLoadOp::eClear;
			_vkAttachmentDescriptions[i].storeOp = vk::AttachmentStoreOp::eStore;

			_vkAttachmentDescriptions[i].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			_vkAttachmentDescriptions[i].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

			_vkAttachmentDescriptions[i].initialLayout = vk::ImageLayout::eUndefined;
			// TMPR
			_vkAttachmentDescriptions[i].finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			break;
		case PC_CORE::AttachementUsage::DEPTH:
		case PC_CORE::AttachementUsage::STENCIL:
			_vkAttachmentDescriptions[i].loadOp = vk::AttachmentLoadOp::eClear;
			_vkAttachmentDescriptions[i].storeOp = vk::AttachmentStoreOp::eStore;

			_vkAttachmentDescriptions[i].stencilLoadOp = vk::AttachmentLoadOp::eClear;
			_vkAttachmentDescriptions[i].stencilStoreOp = vk::AttachmentStoreOp::eStore;

			_vkAttachmentDescriptions[i].initialLayout = vk::ImageLayout::eUndefined;
			_vkAttachmentDescriptions[i].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
			break;
		case PC_CORE::AttachementUsage::NONE:
			break;
		default:;
		}
	}

}

void Vulkan::Backend::CountSubPasses(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, RenderPassDescription* _renderPassDescription)
{
	uint32_t colorAttachementCount = 0;
	uint32_t depthAttachmentCount = 0;
	for (size_t i = 0; i < _renderPassCreateInfo.subPasses.size(); i++)
	{
		const PC_CORE::SubPasse& subPasse = _renderPassCreateInfo.subPasses[i];
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
	_renderPassDescription->colorAttachmentRef.resize(colorAttachementCount);
	_renderPassDescription->depthAttachmentRef.resize(depthAttachmentCount);
}

void Vulkan::Backend::ResolveSubPasses(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo, RenderPassDescription* _renderPassDescription)
{
	uint32_t colorAttachementCount = 0;
	uint32_t depthAttachmentCount = 0;

	size_t subPassCount = _renderPassCreateInfo.subPasses.size();

	_renderPassDescription->subpassesDescription.resize(subPassCount);
	_renderPassDescription->dependencies.resize(subPassCount);

	std::vector<vk::AttachmentReference>& vkColorAttachementsRef = _renderPassDescription->colorAttachmentRef;
	std::vector<vk::AttachmentReference>& vkDepthAttachementsRef = _renderPassDescription->depthAttachmentRef;

	// For each SubPass
	for (size_t i = 0; i < subPassCount; i++)
	{
		const PC_CORE::SubPasse& subPasse = _renderPassCreateInfo.subPasses[i];
		vk::SubpassDescription& subpassDescription = _renderPassDescription->subpassesDescription[i];


		// For each Attachement ref in subpass
		uint32_t surpassLocalColorAttachmentCount = 0;
		uint32_t surpassLocalDepthAttachmentCount = 0;

		for (size_t j = 0; j < subPasse.attachementUsage.size(); j++)
		{
			const PC_CORE::AttachementUsage& attachmentDescription = subPasse.attachementUsage[j];
			vk::AttachmentReference* attachmentReference = nullptr;
			subpassDescription.pipelineBindPoint = RhiPipelineBindPointToVulkan(_renderPassCreateInfo.subPasses.at(i).shaderProgramPipelineType);
			uint32_t attachmentIndex = static_cast<uint32_t>(j);
			
			switch (attachmentDescription)
			{
			case PC_CORE::AttachementUsage::COLOR:
				attachmentReference = &vkColorAttachementsRef[colorAttachementCount];
				attachmentReference->attachment = attachmentIndex;
				attachmentReference->layout = vk::ImageLayout::eColorAttachmentOptimal;
				subpassDescription.pColorAttachments = vkColorAttachementsRef.data() + colorAttachementCount + surpassLocalColorAttachmentCount;


				surpassLocalColorAttachmentCount++;
				colorAttachementCount += surpassLocalColorAttachmentCount;
				subpassDescription.colorAttachmentCount = surpassLocalColorAttachmentCount;
				break;
			case PC_CORE::AttachementUsage::DEPTH:
			case PC_CORE::AttachementUsage::STENCIL:
				attachmentReference = &vkDepthAttachementsRef[depthAttachmentCount];
				attachmentReference->attachment = attachmentIndex;
				attachmentReference->layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

				if (surpassLocalDepthAttachmentCount > 1)
				{
					throw std::runtime_error("depth attachment count greater than 1 is not supported");
				}
				subpassDescription.pDepthStencilAttachment = vkDepthAttachementsRef.data() + depthAttachmentCount + surpassLocalDepthAttachmentCount;

				surpassLocalDepthAttachmentCount++;
				depthAttachmentCount += surpassLocalDepthAttachmentCount;
				break;
			case PC_CORE::AttachementUsage::NONE:
			case PC_CORE::AttachementUsage::COUNT:
				break;
			}



		}

		ResolveSubPassDependencies(&_renderPassDescription->dependencies[i], surpassLocalColorAttachmentCount, surpassLocalDepthAttachmentCount);
	}

}

void Vulkan::Backend::ResolveSubPassDependencies(vk::SubpassDependency* _vkSubpassDependency, uint32_t _colorCount, uint32_t _depthCout)
{
	//TO DO MAY CHANGE IN FUTURE

	vk::SubpassDependency& dependency = *_vkSubpassDependency;

	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	if (_colorCount != 0)
	{
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else
	{
		dependency.srcStageMask = {};
	}

	dependency.srcAccessMask = {};

	dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
	if (_depthCout > 0)
	{
		dependency.dstAccessMask |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}
	
}

void Vulkan::Backend::ResolveBeginRenderPass(PC_CORE::RenderPassHandle _renderPassHandle, const PC_CORE::BeginRenderPassInfo& _renderPassInfo,
	vk::RenderPassBeginInfo* _vkRenderPassBeginInfo, std::vector<vk::ClearValue>* _vkClearValue)
{
	vk::RenderPassBeginInfo& vkRenderPassBeginInfo = *_vkRenderPassBeginInfo;

	_vkClearValue->resize(_renderPassInfo.clearValueCount);
	for (size_t i = 0; i < _vkClearValue->size(); i++)
	{
		_vkClearValue->at(i).color = _renderPassInfo.pClearValues[i].clearValueColor.float32;
		_vkClearValue->at(i).depthStencil.depth = _renderPassInfo.pClearValues[i].clearDepthStencilValue.depth;
		_vkClearValue->at(i).depthStencil.stencil = _renderPassInfo.pClearValues[i].clearDepthStencilValue.stencil;
	}

	
	vkRenderPassBeginInfo.sType = vk::StructureType::eRenderPassBeginInfo;
	vkRenderPassBeginInfo.renderPass = CastObjectToVkObject<vk::RenderPass>(_renderPassHandle);
	vkRenderPassBeginInfo.framebuffer = CastObjectToVkObject<vk::Framebuffer>(_renderPassInfo.framebuffer);
	vkRenderPassBeginInfo.renderArea.offset = vk::Offset2D(_renderPassInfo.renderArea.offset[0], _renderPassInfo.renderArea.offset[1]);
	vkRenderPassBeginInfo.renderArea.extent = vk::Extent2D(_renderPassInfo.renderArea.extend[0], _renderPassInfo.renderArea.extend[1]);
	vkRenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(_vkClearValue->size());
	vkRenderPassBeginInfo.pClearValues = _vkClearValue->data();
}
