#include <VulkanGraphicPipeline.hpp>
#include <Utils/VulkanPipelineHelper.hpp>
#include <Utils/RhiToVulkan.hpp>
#include <LowRenderer/Rhi.hpp>
#include <Resources/ShaderSourceBinary.hpp>
#include <VulkanContext.hpp>
#include <VulkanRenderPass.hpp>

namespace Vulkan
{
	VulkanGraphicPipeline::VulkanGraphicPipeline(PC_CORE::Rhi& _Rhi)
		: RhiGraphicPipeline(_Rhi)
	{

	}

	VulkanGraphicPipeline::~VulkanGraphicPipeline() = default;

    bool VulkanGraphicPipeline::Build()
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rhi);

        if (!m_Modules)
            return false;

        if (m_Modules->empty())
            return false;

        vk::Device d = GET_VK_DEVICE;
        Utils::VulkanShaderProgramCreateContext ctx = Utils::CreateContext(d, *m_Modules);

        m_PipelineLayout = Utils::CreatePipelineLayout(GET_VK_CONTEXT.descritptorManager, d, ctx);

        CreatePipeLineGraphicsPipeline(ctx);

        Utils::DestroyContext(ctx);
        return true;
    }

	std::vector<vk::DynamicState> VulkanGraphicPipeline::GetDynamicState() const
    {
        std::vector<vk::DynamicState> State;

        State.emplace_back(vk::DynamicState::eViewport);
        State.emplace_back(vk::DynamicState::eScissor);
        State.emplace_back(vk::DynamicState::eLineWidth);
        State.emplace_back(vk::DynamicState::eDepthBias);
        State.emplace_back(vk::DynamicState::eDepthBounds);
        State.emplace_back(vk::DynamicState::eStencilCompareMask);
        State.emplace_back(vk::DynamicState::eStencilWriteMask);
        State.emplace_back(vk::DynamicState::eStencilReference);
        State.emplace_back(vk::DynamicState::eBlendConstants);

        bool isMeshShader = false;

        if (auto& Modules = m_Modules)
            for (const auto& it : *m_Modules)
            {
                PC_CORE::RhiPipeline::ShaderStageTypeBits ShaderStageType = it.ShaderStage;
                if (ShaderStageType == ShaderStageTypeBits::Mesh || ShaderStageType == ShaderStageTypeBits::Amp)
                {
                    isMeshShader = true;
                    break;
                }
            }

        if (!isMeshShader)
            State.emplace_back(vk::DynamicState::ePrimitiveTopology);

        return State;
    };

    vk::PipelineLayout VulkanGraphicPipeline::GetPipelineLayout() const
    {
        return m_PipelineLayout;
    }

    vk::Pipeline VulkanGraphicPipeline::GetPipeline() const
    {
        return m_Pipeline;
    }

    void VulkanGraphicPipeline::CreatePipeLineGraphicsPipeline(const Utils::VulkanShaderProgramCreateContext& _VulkanShaderProgramCreateContex)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rhi);

        std::vector<vk::DynamicState> DynamicState = GetDynamicState();

        vk::Device device = GET_VK_DEVICE;
        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicState.size());
        dynamicState.pDynamicStates = DynamicState.data();

        // VertexInput
        std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
        std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = ParseVertexInputState(&vertexInputBindingDescriptions, &vertexInputAttributeDescriptions);

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
       

        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(1920);
        viewport.height = static_cast<float>(1080);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{ 0, 0 };
        scissor.extent = vk::Extent2D{ 1920, 1080 };

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
        ParseRasterizer(&rasterizer);

        vk::SampleCountFlagBits sample = Utils::RhSampleCountToVulkan(m_Descriptor.Sample);
        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
        multisampling.sampleShadingEnable = sample == vk::SampleCountFlagBits::e1 ? VK_FALSE : VK_TRUE;
        multisampling.rasterizationSamples = sample;
        multisampling.minSampleShading = sample == vk::SampleCountFlagBits::e1 ? 1.0f : 0.1f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        assert(m_Descriptor.RenderPass != nullptr);

        const PC_CORE::RhiRenderPass& RhiRenderPass = *m_Descriptor.RenderPass;
        const PC_CORE::SubPass& SubPass = RhiRenderPass.GetSubPasses()[m_Descriptor.SubPassIndex];
        const uint32_t ColorAttachmentCount = static_cast<uint32_t>(SubPass.ColorAttachements.size());

        std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments(ColorAttachmentCount);
        for (size_t i = 0; i < colorBlendAttachments.size(); ++i)
            ParsePipelineColorAttachementBlendState(&colorBlendAttachments[i], m_Descriptor.BlendState);


        vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
        if (m_Descriptor.DephStencilInfo)
            ParsePipelineDepthStencilAttachmentState(&depthStencilState, *m_Descriptor.DephStencilInfo);

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
        colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
        colorBlending.pAttachments = colorBlendAttachments.data();

        ParseParsePipelineColorBlendState(&colorBlending, colorBlendAttachments.data(), colorBlendAttachments.size(), m_Descriptor.BlendState);

        vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{};
        graphicsPipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
        graphicsPipelineInfo.stageCount = static_cast<uint32_t>(_VulkanShaderProgramCreateContex.
            pipelineShaderStageCreateInfos.size());
        graphicsPipelineInfo.pStages = _VulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos.data();
        graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
        graphicsPipelineInfo.pInputAssemblyState = &inputAssembly;
        graphicsPipelineInfo.pViewportState = &viewportState;
        graphicsPipelineInfo.pRasterizationState = &rasterizer;
        if (m_Descriptor.DephStencilInfo)
            graphicsPipelineInfo.pDepthStencilState = &depthStencilState;
        graphicsPipelineInfo.pMultisampleState = &multisampling;
        graphicsPipelineInfo.pColorBlendState = &colorBlending;
        graphicsPipelineInfo.pDynamicState = &dynamicState;
        graphicsPipelineInfo.layout = m_PipelineLayout;

        const VulkanRenderPass& VulkanRenderPassObject = *reinterpret_cast<VulkanRenderPass*>(m_Descriptor.RenderPass);
        graphicsPipelineInfo.renderPass = VulkanRenderPassObject.GetVulkanRenderPass();
        graphicsPipelineInfo.subpass = m_Descriptor.SubPassIndex;

        auto result = device.createGraphicsPipeline(nullptr, graphicsPipelineInfo);
        VK_CALL(result.result);

        vk::DebugUtilsObjectNameInfoEXT nameInfo{};
        nameInfo.objectType = vk::ObjectType::ePipeline;
        nameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkPipeline>(result.value));
        nameInfo.pObjectName = GetName().data();

        SET_VK_DEBUG_NAME(nameInfo);

        m_Pipeline = result.value;
    }

#pragma region ParseRegion
    void VulkanGraphicPipeline::ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo)
    {
        _pipelineRasterizationStateCreateInfo->depthClampEnable = VK_FALSE;
        _pipelineRasterizationStateCreateInfo->rasterizerDiscardEnable = VK_FALSE;
        _pipelineRasterizationStateCreateInfo->polygonMode = Utils::RhiPolygonModeToVulkan(m_Descriptor.PolygonMode);
        _pipelineRasterizationStateCreateInfo->lineWidth = 1.0f;
        _pipelineRasterizationStateCreateInfo->cullMode = Utils::RhiToCullMode(m_Descriptor.CullMode);

        _pipelineRasterizationStateCreateInfo->frontFace = m_Descriptor.FrontFace == RhiGraphicPipeline::FrontFace::Clockwise
            ? vk::FrontFace::eClockwise
            : vk::FrontFace::eCounterClockwise;

        _pipelineRasterizationStateCreateInfo->depthBiasEnable = VK_FALSE;
    }

    void VulkanGraphicPipeline::ParsePipelineColorAttachementBlendState(vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState,
        const std::optional<RhiGraphicPipeline::BlendState>& _blendInfo)
    {
        const bool enableBlend = _blendInfo.has_value();
        _PipelineColorBlendAttachmentState->blendEnable = enableBlend ? VK_TRUE : VK_FALSE;

        if (!enableBlend)
        {
            _PipelineColorBlendAttachmentState->srcColorBlendFactor = vk::BlendFactor::eOne;
            _PipelineColorBlendAttachmentState->dstColorBlendFactor = vk::BlendFactor::eZero;
            _PipelineColorBlendAttachmentState->colorBlendOp = vk::BlendOp::eAdd;

            _PipelineColorBlendAttachmentState->srcAlphaBlendFactor = vk::BlendFactor::eOne;
            _PipelineColorBlendAttachmentState->dstAlphaBlendFactor = vk::BlendFactor::eZero;
            _PipelineColorBlendAttachmentState->alphaBlendOp = vk::BlendOp::eAdd;

            _PipelineColorBlendAttachmentState->colorWriteMask =
                Utils::RhiColorComponent(static_cast<PC_CORE::ColorComponent>(
                    PC_CORE::ColorComponent::ColorComponentRGBA));

            return;
        }

        const auto& b = *_blendInfo;
        _PipelineColorBlendAttachmentState->srcColorBlendFactor = Utils::RhiBlendFactorToVulkan(b.ColorSrcFactor);
        _PipelineColorBlendAttachmentState->dstColorBlendFactor = Utils::RhiBlendFactorToVulkan(b.ColorDstFactor);
        _PipelineColorBlendAttachmentState->colorBlendOp = Utils::RhiBlendOpToVulkan(b.ColorOp);

        _PipelineColorBlendAttachmentState->srcAlphaBlendFactor = Utils::RhiBlendFactorToVulkan(b.AlphaSrcFactor);
        _PipelineColorBlendAttachmentState->dstAlphaBlendFactor = Utils::RhiBlendFactorToVulkan(b.AlphaDstFactor);
        _PipelineColorBlendAttachmentState->alphaBlendOp = Utils::RhiBlendOpToVulkan(b.AlphaOp);

        _PipelineColorBlendAttachmentState->colorWriteMask = Utils::RhiColorComponent(_blendInfo->BlendMask);
    }

    void VulkanGraphicPipeline::ParsePipelineDepthStencilAttachmentState(
        vk::PipelineDepthStencilStateCreateInfo* _PipelineDepthStencilStateCreateInfo,
        const RhiGraphicPipeline::DephStencilInfo& _dephInfo)
    {
        _PipelineDepthStencilStateCreateInfo->sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
        _PipelineDepthStencilStateCreateInfo->depthTestEnable = _dephInfo.enableDepthTest ? VK_TRUE : VK_FALSE;
        _PipelineDepthStencilStateCreateInfo->depthWriteEnable = _dephInfo.enableDepthWrite ? VK_TRUE : VK_FALSE;
        _PipelineDepthStencilStateCreateInfo->depthCompareOp = Utils::RhiToVulkanCompareOp(_dephInfo.depthCompareOp);
        _PipelineDepthStencilStateCreateInfo->minDepthBounds = 0.0f;
        _PipelineDepthStencilStateCreateInfo->maxDepthBounds = 1.0f;
        _PipelineDepthStencilStateCreateInfo->stencilTestEnable = VK_FALSE;
        _PipelineDepthStencilStateCreateInfo->front = vk::StencilOpState(); // Optional
        _PipelineDepthStencilStateCreateInfo->back = vk::StencilOpState(); // Optional
    }

    void VulkanGraphicPipeline::ParseParsePipelineColorBlendState(
        vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo,
        const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState,
        size_t _PipelineColorBlendAttachmentSize, const std::optional<RhiGraphicPipeline::BlendState>& _blendInfo)
    {
        _PipelineColorBlendStateCreateInfo->logicOpEnable = VK_FALSE;
        _PipelineColorBlendStateCreateInfo->logicOp = vk::LogicOp::eCopy; // Optional
        _PipelineColorBlendStateCreateInfo->attachmentCount = _PipelineColorBlendAttachmentSize;
        _PipelineColorBlendStateCreateInfo->pAttachments = _PipelineColorBlendAttachmentState;
        _PipelineColorBlendStateCreateInfo->blendConstants[0] = 0.0f; // Optional
        _PipelineColorBlendStateCreateInfo->blendConstants[1] = 0.0f; // Optional
        _PipelineColorBlendStateCreateInfo->blendConstants[2] = 0.0f; // Optional
        _PipelineColorBlendStateCreateInfo->blendConstants[3] = 0.0f; // Optional
    }


    vk::VertexInputBindingDescription VulkanGraphicPipeline::ParseVertexInputBindingDescription(
        const PC_CORE::VertexInputBindingDescrition& _vertexInputBindingDescrition)
    {
        vk::VertexInputBindingDescription vkvertexInputBindingDescription{};
        vkvertexInputBindingDescription.binding = _vertexInputBindingDescrition.Binding;
        vkvertexInputBindingDescription.stride = _vertexInputBindingDescrition.Stride;
        vkvertexInputBindingDescription.inputRate = Utils::RhiInputRateToVkInputRate(
            _vertexInputBindingDescrition.VertexInputRate);

        return vkvertexInputBindingDescription;
    }

    vk::VertexInputAttributeDescription VulkanGraphicPipeline::ParseVertexInputAttributeDescription(
        const PC_CORE::VertexAttributeDescription& _vertexAttributeDescription)
    {
        vk::VertexInputAttributeDescription vkvertexInputAttributeDescription;
        vkvertexInputAttributeDescription.location = _vertexAttributeDescription.Location;
        vkvertexInputAttributeDescription.binding = _vertexAttributeDescription.Binding;
        vkvertexInputAttributeDescription.offset = _vertexAttributeDescription.Offset;
        vkvertexInputAttributeDescription.format = Utils::RhiFormatToVkFormat(_vertexAttributeDescription.Format);

        return vkvertexInputAttributeDescription;
    }

    vk::PipelineVertexInputStateCreateInfo VulkanGraphicPipeline::ParseVertexInputState(std::vector<vk::VertexInputBindingDescription>* _vertexInputBindingDescriptions
        , std::vector<vk::VertexInputAttributeDescription>* _vertexInputAttributeDescriptions)
    {
        if (_vertexInputBindingDescriptions == nullptr || _vertexInputAttributeDescriptions == nullptr)
        {
            PC_LOGERROR("_vertexInputBindingDescriptions is null or _vertexInputAttributeDescriptions is null");
            return {};
        }

        // Parse vertexInputBindingDescriptions
        _vertexInputBindingDescriptions->reserve(m_Descriptor.VertexInputBindingDescritions.size());
        for (size_t i = 0; i < m_Descriptor.VertexInputBindingDescritions.size(); i++)
        {
            _vertexInputBindingDescriptions->emplace_back(
                ParseVertexInputBindingDescription(m_Descriptor.VertexInputBindingDescritions[i]));
        }

        // Parse verteixAttributes
        _vertexInputAttributeDescriptions->reserve(m_Descriptor.VertexAttributeDescriptions.size());
        for (size_t i = 0; i < m_Descriptor.VertexAttributeDescriptions.size(); i++)
        {
            _vertexInputAttributeDescriptions->emplace_back(
                ParseVertexInputAttributeDescription(m_Descriptor.VertexAttributeDescriptions[i]));
        }

        vk::PipelineVertexInputStateCreateInfo returnVertexInputStateCreateInfo{};
        returnVertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
        returnVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(
            _vertexInputBindingDescriptions->size());
        returnVertexInputStateCreateInfo.pVertexBindingDescriptions = _vertexInputBindingDescriptions->data();

        returnVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
            _vertexInputAttributeDescriptions->size());
        returnVertexInputStateCreateInfo.pVertexAttributeDescriptions = _vertexInputAttributeDescriptions->data();

        return returnVertexInputStateCreateInfo;
    }


#pragma endregion ParseRegion
}

