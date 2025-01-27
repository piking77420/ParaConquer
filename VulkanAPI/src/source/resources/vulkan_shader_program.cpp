#include "resources/vulkan_shader_program.hpp"

#include "rhi_vulkan_parser.hpp"
#include "vulkan_context.hpp"
#include "vulkan_header.h"
#include "vulkan_render_pass.hpp"

using namespace Vulkan;


VulkanShaderProgram::~VulkanShaderProgram()
{
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice);

    if (m_PipelineLayout != VK_NULL_HANDLE)
    {
        device->GetDevice().destroyPipelineLayout(m_PipelineLayout);
        m_PipelineLayout = VK_NULL_HANDLE;
    }

    if (m_Pipeline != VK_NULL_HANDLE)
    {
        device->GetDevice().destroyPipeline(m_Pipeline);
        m_Pipeline = VK_NULL_HANDLE;
    }
}

vk::PipelineBindPoint VulkanShaderProgram::GetPipelineBindPoint() const
{
    switch (m_ProgramShaderCreateInfo.shaderInfo.shaderProgramPipelineType)
    {
    case PC_CORE::ShaderProgramPipelineType::POINT_GRAPHICS:
        return vk::PipelineBindPoint::eGraphics;
    case PC_CORE::ShaderProgramPipelineType::COMPUTE:
        return vk::PipelineBindPoint::eCompute;
    case PC_CORE::ShaderProgramPipelineType::RAYTRACING:
        return vk::PipelineBindPoint::eRayTracingKHR;
    case PC_CORE::ShaderProgramPipelineType::COUT:
        default:
        throw std::runtime_error("Unsupported shader program pipeline!");
    }
    return {};
}

vk::Pipeline VulkanShaderProgram::GetPipeline() const
{
    return m_Pipeline;
}

VulkanShaderProgram::VulkanShaderProgram(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo) : ShaderProgram(_programShaderCreateInfo)
{
    std::vector<std::vector<char>> spvModuleSourceCode = std::vector<std::vector<char>>(m_ProgramShaderCreateInfo.shaderInfo.shaderSources.size());

    for (size_t i = 0; i < spvModuleSourceCode.size(); i++)
    {
        const std::pair<PC_CORE::ShaderStageType, std::string>& shaderSource = _programShaderCreateInfo.shaderInfo.shaderSources[i];
        const char* format = nullptr;
         if (!Resource::GetFormatFromValue(PC_CORE::ShaderSourceFormat, shaderSource.first, &format))
         {
             PC_LOGERROR("Failed to parse shader source for shader ");
         }
        std::string spvFile = SHADER_CACHE_PATH + shaderSource.second;

        spvModuleSourceCode[i] = ReadFile(spvFile);
    }
    
    const PC_CORE::ShaderGraphicPointInfo& shaderGraphicPointInfo = std::get<0>(_programShaderCreateInfo.shaderInfo.shaderInfoData);


    std::vector<vk::ShaderModule> vkShaderModules(spvModuleSourceCode.size());
    std::vector<vk::PipelineShaderStageCreateInfo> PipelineShaderStageCreateInfos(vkShaderModules.size());
    
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice);

    // Create Modules
    for (size_t i = 0; i < spvModuleSourceCode.size(); i++)
    {
        vk::ShaderModuleCreateInfo createInfo = {};
        createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        createInfo.codeSize = spvModuleSourceCode[i].size();
        createInfo.pCode = reinterpret_cast<uint32_t*>(spvModuleSourceCode[i].data());
        
        VK_CALL(device->GetDevice().createShaderModule(&createInfo, nullptr, &vkShaderModules[i]));
    }

    // CreatePipelineShaderStage 

    for (size_t i = 0; i < PipelineShaderStageCreateInfos.size(); i++)
    {
        PipelineShaderStageCreateInfos[i].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        PipelineShaderStageCreateInfos[i].stage = RhiToShaderStage(_programShaderCreateInfo.shaderInfo.shaderSources[i].first);
        PipelineShaderStageCreateInfos[i].module = vkShaderModules[i];
        PipelineShaderStageCreateInfos[i].pName = "main";
    }
    
    

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateArray.size());
    dynamicState.pDynamicStates = dynamicStateArray.data();

    // VertexInput
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

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
    scissor.offset = vk::Offset2D{0, 0};
    scissor.extent = vk::Extent2D{1920, 1080};

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    ParseRasterizer(&rasterizer, shaderGraphicPointInfo.rasterizerInfo);
    
    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    ParsePipelineColorBlendAttachmentState(&colorBlendAttachment);

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    ParseParsePipelineColorBlendAttachmentState(&colorBlending, &colorBlendAttachment);


    vk::PipelineLayoutCreateInfo  pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

   m_PipelineLayout = device->GetDevice().createPipelineLayout(pipelineLayoutInfo);
    
    vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{};
    graphicsPipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    graphicsPipelineInfo.stageCount = static_cast<uint32_t>(spvModuleSourceCode.size());
    graphicsPipelineInfo.pStages = PipelineShaderStageCreateInfos.data();
    graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
    graphicsPipelineInfo.pInputAssemblyState = &inputAssembly;
    graphicsPipelineInfo.pViewportState = &viewportState;
    graphicsPipelineInfo.pRasterizationState = &rasterizer;
    graphicsPipelineInfo.pMultisampleState = &multisampling;
    graphicsPipelineInfo.pDepthStencilState = nullptr; // Optional
    graphicsPipelineInfo.pColorBlendState = &colorBlending;
    graphicsPipelineInfo.pDynamicState = &dynamicState;
    graphicsPipelineInfo.layout = m_PipelineLayout;
    graphicsPipelineInfo.renderPass = std::reinterpret_pointer_cast<VulkanRenderPass>(m_ProgramShaderCreateInfo.renderPass)->GetVulkanRenderPass();
    graphicsPipelineInfo.subpass = 0;

    auto result = device->GetDevice().createGraphicsPipeline(nullptr, graphicsPipelineInfo);
    m_Pipeline = result.value;
    
    for (size_t i = 0; i < spvModuleSourceCode.size(); i++)
    {
        device->GetDevice().destroyShaderModule(vkShaderModules[i]);
    }
        
    

}


void VulkanShaderProgram::ParseRasterizer(
    vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo,
    const PC_CORE::RasterizerInfo& _rasterizerInfo)
{
    _pipelineRasterizationStateCreateInfo->depthClampEnable = VK_FALSE;
    _pipelineRasterizationStateCreateInfo->rasterizerDiscardEnable = VK_FALSE;
    _pipelineRasterizationStateCreateInfo->polygonMode = RhiPolygonModeToVulkan(_rasterizerInfo.polygonMode);
    _pipelineRasterizationStateCreateInfo->lineWidth = 1.0f;
    _pipelineRasterizationStateCreateInfo->cullMode = RhiToCullMode(_rasterizerInfo.cullModeFlag);
    
    _pipelineRasterizationStateCreateInfo->frontFace = _rasterizerInfo.frontFace == PC_CORE::FrontFace::Clockwise ?  vk::FrontFace::eClockwise
    : vk::FrontFace::eCounterClockwise;
    
    _pipelineRasterizationStateCreateInfo->depthBiasEnable = VK_FALSE;
    
}

void VulkanShaderProgram::ParsePipelineColorBlendAttachmentState(vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState)
{
    _PipelineColorBlendAttachmentState->colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    _PipelineColorBlendAttachmentState->blendEnable = VK_FALSE;
    _PipelineColorBlendAttachmentState->srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
    _PipelineColorBlendAttachmentState->dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
    _PipelineColorBlendAttachmentState->colorBlendOp = vk::BlendOp::eAdd; // Optional
    _PipelineColorBlendAttachmentState->srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
    _PipelineColorBlendAttachmentState->dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
    _PipelineColorBlendAttachmentState->alphaBlendOp = vk::BlendOp::eAdd;
    
}

void VulkanShaderProgram::ParseParsePipelineColorBlendAttachmentState(
    vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo,
    const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState)
{
    _PipelineColorBlendStateCreateInfo->logicOpEnable = VK_FALSE;
    _PipelineColorBlendStateCreateInfo->logicOp = vk::LogicOp::eCopy; // Optional
    _PipelineColorBlendStateCreateInfo->attachmentCount = 1;
    _PipelineColorBlendStateCreateInfo->pAttachments = _PipelineColorBlendAttachmentState;
    _PipelineColorBlendStateCreateInfo->blendConstants[0] = 0.0f; // Optional
    _PipelineColorBlendStateCreateInfo->blendConstants[1] = 0.0f; // Optional
    _PipelineColorBlendStateCreateInfo->blendConstants[2] = 0.0f; // Optional
    _PipelineColorBlendStateCreateInfo->blendConstants[3] = 0.0f; // Optional
}
