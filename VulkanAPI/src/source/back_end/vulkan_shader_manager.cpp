#include "back_end/vulkan_shader_manager.hpp"

#include <filesystem>

#include "back_end/vulkan_harware_wrapper.hpp"
#include <shaderc/shaderc.hpp>

#include "back_end/vulkan_present_chain.hpp"


using namespace VK_NP;

void VulkanShaderManager::BindProgram(const std::string& _shaderName,vk::CommandBuffer _commandBuffer)
{
    if (!m_InternalShadersMap.contains(_shaderName))
    {
        assert(false, "Missing shader name");
    }

    ShaderInternal& shaderInternal = m_InternalShadersMap[_shaderName];
    _commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, shaderInternal.pipeline);
}

void VulkanShaderManager::ReloadShader(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreatInfo,
                                                 const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    if (!DestroyShader(_programShaderCreatInfo.prograShaderName))
        return;

    CreateShaderFromSource(_programShaderCreatInfo, _shaderSource);
}


bool VulkanShaderManager::CreateShaderFromSource(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreatInfo,
                                                 const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    ShaderInternal shaderInternalBack = {};
    // determine shader stage type and name
    FillShaderInfo(&shaderInternalBack, _shaderSource);
    std::vector<ShaderStageInfo>& shaderStagesInfo = shaderInternalBack.shaderStages;
    std::vector<vk::ShaderModule> shaderModules(shaderInternalBack.shaderStages.size());
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStagesCreateInfos(shaderInternalBack.shaderStages.size());

    for (uint32_t i = 0; i < _shaderSource.size(); i++)
    {
        PC_CORE::LowLevelShaderStageType shaderStage = shaderStagesInfo.at(i).shaderStage;
        // SOURCE TO MODULE and get spv reflection
        m_ShaderCompiler.CreateModuleFromSource(_shaderSource[i].shaderSourceCode.data(),
                                                shaderStage,&shaderStagesInfo.at(i).reflectShaderModule, &shaderModules[i]);
        
        shaderStagesCreateInfos.at(i).sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStagesCreateInfos.at(i).stage = ShaderBitFromType(shaderStage);
        shaderStagesCreateInfos.at(i).module = shaderModules[i];
        shaderStagesCreateInfos.at(i).pName = shaderStagesInfo.at(i).reflectShaderModule.entry_point_name;
    }

    
    CreatePipelineFromModule(shaderStagesCreateInfos, &shaderInternalBack.pipeline, &shaderInternalBack.pipelineLayout);
    
    m_InternalShadersMap.insert({ _programShaderCreatInfo.prograShaderName,shaderInternalBack });

    // graphic and layout as been created no need module modules then
    for (auto& module : shaderModules)
    {
        m_Device.destroyShaderModule(module);
    }
    
    return true;
}

void VulkanShaderManager::FillShaderInfo(ShaderInternal* _shaderInternalBack,
    const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    _shaderInternalBack->shaderStages.resize(_shaderSource.size());
    
    for (uint32_t i = 0; i < _shaderSource.size(); i++)
    {
        const PC_CORE::ShaderSourceAndPath& sourceCodeAndPath = _shaderSource[i];
        std::filesystem::path path = std::filesystem::path(sourceCodeAndPath.shaderSourceCodePath);
        
        // Get TypeFromFormat
        std::string format = path.extension().generic_string();

        const PC_CORE::LowLevelShaderStageType lowLevelShader = PC_CORE::ShaderFormatToShaderType(format.c_str());
        _shaderInternalBack->shaderStages[i].shaderStage = lowLevelShader;
        // Get shader Name with format
        _shaderInternalBack->shaderStages[i].name = path.filename().stem().generic_string() + format;
    }
}

void VulkanShaderManager::CreatePipelineFromModule(const std::vector<vk::PipelineShaderStageCreateInfo>& _shaderStageCreateInfos, vk::Pipeline* _outPipeline, vk::PipelineLayout* _outLayout)
{
    const std::vector<vk::DynamicState> dynamicStates =
        {
          vk::DynamicState::eViewport,
          vk::DynamicState::eScissor,
        };

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_SwapChainExtent.width);
    viewport.height = static_cast<float>(m_SwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_SwapChainExtent;
    
    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    
    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional


    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    *_outLayout = m_Device.createPipelineLayout(pipelineLayoutInfo, nullptr);

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = _shaderStageCreateInfos.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = *_outLayout;
    pipelineInfo.renderPass = VulkanContext::currentContext->swapChainRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CALL(m_Device.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, _outPipeline));
    
}


void VulkanShaderManager::DestroyInternalShaders(VulkanShaderManager::ShaderInternal* _shaderInternalBack)
{
    // destroy each reflected spv module
    for (auto& s : _shaderInternalBack->shaderStages)
    {
        if (s.reflectShaderModule.source_file != nullptr)
        {
            spvReflectDestroyShaderModule(&s.reflectShaderModule);
        }
    }
    // Destroy pipeline
    if (_shaderInternalBack->pipeline != VK_NULL_HANDLE)
        m_Device.destroyPipeline(_shaderInternalBack->pipeline);

    // Destroy pipelune layout
    if (_shaderInternalBack->pipelineLayout != VK_NULL_HANDLE)
        m_Device.destroyPipelineLayout(_shaderInternalBack->pipelineLayout);
}



bool VulkanShaderManager::DestroyShader(const std::string& _shaderName)
{
    const bool success = m_InternalShadersMap.contains(_shaderName);
    if (!success)
        return success;

    auto it = m_InternalShadersMap.at(_shaderName);
    DestroyInternalShaders(&it);
    
    m_InternalShadersMap.erase(_shaderName);
    return true;
}

VulkanShaderManager::VulkanShaderManager()
{
    m_Device = VulkanContext::currentContext->device;
    m_SwapChainExtent = VulkanContext::currentContext->m_Extent2D;
}

VulkanShaderManager::~VulkanShaderManager()
{
    // for each shader programm
    for (auto it = m_InternalShadersMap.begin(); it != m_InternalShadersMap.end(); it++)
    {
        DestroyInternalShaders(&it->second);
    }
}

#pragma region PARSING

vk::ShaderStageFlagBits VulkanShaderManager::ShaderBitFromType(const PC_CORE::LowLevelShaderStageType _shaderType)
{
    switch (_shaderType)
    {
    case PC_CORE::LowLevelShaderStageType::VERTEX:
        return vk::ShaderStageFlagBits::eVertex;
        break;
    case PC_CORE::LowLevelShaderStageType::FRAGMENT:
        return vk::ShaderStageFlagBits::eFragment;
        break;
    case PC_CORE::LowLevelShaderStageType::GEOMETRY:
        return vk::ShaderStageFlagBits::eGeometry;
        break;
    case PC_CORE::LowLevelShaderStageType::TESSELATION:
        assert(false, "NOTSUPPORTED SHADER TESSELATION");
        return vk::ShaderStageFlagBits::eTessellationControl;
        break;
    case PC_CORE::LowLevelShaderStageType::COMPUTE:
        return vk::ShaderStageFlagBits::eCompute;
        break;
    case PC_CORE::LowLevelShaderStageType::COUNT:
        break;
    }
}

#pragma endregion PARSING