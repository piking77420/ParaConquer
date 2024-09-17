#include "back_end/vulkan_shader_manager.hpp"

#include <filesystem>

#include "back_end/vulkan_harware_wrapper.hpp"
#include <shaderc/shaderc.hpp>

#include "back_end/vulkan_present_chain.hpp"

bool VK_NP::VulkanShaderManager::CreateShaderFromSource(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreatInfo,
                                                        const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    ShaderInternalBack shaderInternalBack = {};
    ShaderReflexion(&shaderInternalBack, _shaderSource);

    std::vector<ShaderStageInfo>& shaderStagesInfo = shaderInternalBack.shaderStages;
    std::vector<vk::ShaderModule> shaderModules(shaderInternalBack.shaderStages.size());
    
    for (size_t i = 0; i < shaderInternalBack.shaderStages.size(); i++)
    {
        CreateModuleForProgram(shaderStagesInfo[i].name, shaderStagesInfo[i], _shaderSource[i].shaderSourceCode, &shaderModules[i]); 
    }
    CreatePipelineFromModule(shaderInternalBack.shaderStages, shaderModules, &shaderInternalBack.pipeline , &shaderInternalBack.pipelineLayout);

    // graphic and layout as been created no need module modules then
    for (auto& module : shaderModules)
    {
        m_Device.destroyShaderModule(module);
    }
    
    return true;
}

void VK_NP::VulkanShaderManager::ShaderReflexion(
    VK_NP::VulkanShaderManager::ShaderInternalBack* _shaderInternalBack,
    const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    _shaderInternalBack->shaderStages.reserve(_shaderSource.size());
    
    for (uint32_t i = 0; i < _shaderSource.size(); i++)
    {
        const PC_CORE::ShaderSourceAndPath& sourceCodeAndPath = _shaderSource[i];
        std::filesystem::path path = std::filesystem::path(sourceCodeAndPath.shaderSourceCodePath);
        
    
        // Get TypeFromFormat
        const char* format = path.extension().generic_string().
        c_str();
        const PC_CORE::LowLevelShaderStageType lowLevelShader = PC_CORE::ShaderFormatToShaderType(format);
        _shaderInternalBack->shaderStages[i].shaderStage = lowLevelShader;
        // Get shader Name with format
        _shaderInternalBack->shaderStages[i].name = path.filename().stem().generic_string() + format;
        
        
        SpvReflectResult result = spvReflectCreateShaderModule(_shaderSource[i].shaderSourceCode.size(),
                                                               _shaderSource[i].shaderSourceCode.data(),
                                                               &_shaderInternalBack->shaderStages[i].reflectShaderModule);
        if (!result)
        {
            assert(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule failed");
            spvReflectDestroyShaderModule(&_shaderInternalBack->shaderStages[i].reflectShaderModule);
            return ;
        }
    }
}

void VK_NP::VulkanShaderManager::CreatePipelineFromModule(const std::vector<ShaderStageInfo>& shaderStages,
    const std::vector<vk::ShaderModule>& _shaderModules, vk::Pipeline* _ooutPipeline, vk::PipelineLayout* _outLayout)
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


    
}

void VK_NP::VulkanShaderManager::CreateModuleForProgram(const std::string _shaderName,
                                                        const ShaderStageInfo& shaderStageInfo, const std::vector<uint8_t>& _shaderCode, vk::ShaderModule* _pipileLineModule)
{
    // MAY CHANGE 
    std::string shaderCodes(_shaderCode.begin(), _shaderCode.end());
    std::vector<uint32_t> sprivCodes;
    VulkanShaderCompiler::CompileShaderToSpv(_shaderName, shaderCodes, shaderStageInfo.shaderStage, &sprivCodes);

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    shaderModuleCreateInfo.codeSize = sprivCodes.size();
    shaderModuleCreateInfo.pCode = sprivCodes.data();

    VK_CALL(m_Device.createShaderModule(&shaderModuleCreateInfo, nullptr, _pipileLineModule));
}


bool VK_NP::VulkanShaderManager::DestroyShader(const std::string& _shaderName)
{
    const bool success = m_InternalShadersMap.contains(_shaderName);
    if (!success)
        return success;

    m_InternalShadersMap.erase(_shaderName);
    return true;
}

VK_NP::VulkanShaderManager::VulkanShaderManager()
{
    m_Device = VulkanHarwareWrapper::GetDevice();
    m_SwapChainExtent = VulkanPresentChain::GetExtent();
}

VK_NP::VulkanShaderManager::~VulkanShaderManager()
{
    // for each shader programm
    for (auto it = m_InternalShadersMap.begin(); it != m_InternalShadersMap.end(); it++)
    {
        // destroy each reflected spv module
        for (auto& s : it->second.shaderStages)
        {
            if (s.reflectShaderModule.source_file != nullptr)
            {
                spvReflectDestroyShaderModule(&s.reflectShaderModule);
            }
        }
        // Destroy pipeline
        if (it->second.pipeline != VK_NULL_HANDLE)
            m_Device.destroyPipeline(it->second.pipeline);

        // Destroy pipelune layout
        if (it->second.pipelineLayout != VK_NULL_HANDLE)
            m_Device.destroyPipelineLayout(it->second.pipelineLayout);
    }
}



vk::PipelineShaderStageCreateInfo VK_NP::VulkanShaderManager::CreateShaderStageCreateInfoFromSource(
    const PC_CORE::LowLevelShaderStageType type, const SpvReflectShaderModule& _spvReflectShaderModule)
{
    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    shaderStageCreateInfo.stage = ShaderBitFromType(type);
    shaderStageCreateInfo.pName = _spvReflectShaderModule.entry_point_name;

    return shaderStageCreateInfo;
}

vk::ShaderStageFlagBits VK_NP::VulkanShaderManager::ShaderBitFromType(const PC_CORE::LowLevelShaderStageType _shaderType)
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
