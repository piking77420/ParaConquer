#include "front_end/vulkan_shader_manager.hpp"

#include <filesystem>
#include <stack>

#include "front_end/vulkan_harware_wrapper.hpp"
#include <shaderc/shaderc.hpp>

#include "back_end/rhi_vulkan_descriptor_write.hpp"
#include "front_end/vulkan_present_chain.hpp"
#include "render_harware_interface/vertex.hpp"
#include "rhi_vulkan_parser.hpp"


using namespace Vulkan;

void VulkanShaderManager::Init(VulkanContext* _vulkanContext)
{
}

void VulkanShaderManager::Destroy(VulkanContext* _vulkanContext)
{
    // for each shader programm
    for (auto it = m_InternalShadersMap.begin(); it != m_InternalShadersMap.end(); it++)
    {
        DestroyInternalShaders(_vulkanContext->device, &it->second);
    }
}

const ShaderInternal& VulkanShaderManager::GetShader(const std::string& _shaderName)
{
    return m_InternalShadersMap.at(_shaderName);
}


void VulkanShaderManager::BindProgram(vk::CommandBuffer _commandBuffer, const std::string& _shaderName)
{
    ShaderInternal* shaderInternal = GetShaderInternal(_shaderName);

    if (shaderInternal != nullptr)
        _commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, shaderInternal->pipeline);
}

ShaderInternal* VulkanShaderManager::GetShaderInternal(const std::string& _shaderName)
{
    if (!m_InternalShadersMap.contains(_shaderName))
    {
        assert(false, "Missing shader name");
        return nullptr;
    }

    return &m_InternalShadersMap[_shaderName];
}


void VulkanShaderManager::PushConstant(const std::string& _shaderName, const char* pushConstantName, const void* _value,
                                       size_t _size, vk::CommandBuffer _commandBuffer)
{
    ShaderInternal* shaderInternal = GetShaderInternal(_shaderName);

    std::vector<ReflectBlockVariable>::iterator it = std::ranges::find_if(shaderInternal->reflectBlockVariables,
                                                                          [&](const ReflectBlockVariable& _pushConstant)
                                                                          {
                                                                              return pushConstantName == _pushConstant.
                                                                                  name;
                                                                          });

    if (it == shaderInternal->reflectBlockVariables.end())
    {
        PC_CORE::RHI::RHILog(PC_CORE::LogType::INFO,
                             (std::string("There is no PushConstate Name as ") + pushConstantName).c_str());
        return;
    }

    const ReflectBlockVariable& pushConstantInternal = *it;
    assert(_size <= pushConstantInternal.size, "Size mismatch in push constant range");

    _commandBuffer.pushConstants(shaderInternal->pipelineLayout, pushConstantInternal.stageFlags,
                                 static_cast<uint32_t>(pushConstantInternal.absoluteOffSet)
                                 , static_cast<uint32_t>(pushConstantInternal.size), _value);
}


void VulkanShaderManager::FillShaderInfo(ShaderInternal* _shaderInternalBack,
                                         const std::vector<PC_CORE::ShaderSourcePath>& _shaderSource)
{
    _shaderInternalBack->shaderStages.resize(_shaderSource.size());

    for (uint32_t i = 0; i < _shaderSource.size(); i++)
    {
        const PC_CORE::ShaderSourcePath& sourceCodeAndPath = _shaderSource[i];
        std::filesystem::path path = std::filesystem::path(sourceCodeAndPath.shaderSourceCodePath);

        // Get TypeFromFormat
        std::string format = path.extension().generic_string();

        const PC_CORE::ShaderStageType lowLevelShader = PC_CORE::ShaderFormatToShaderType(format.c_str());
        _shaderInternalBack->shaderStages[i].shaderStage = lowLevelShader;
        // Get shader Name with format
        _shaderInternalBack->shaderStages[i].name = path.filename().stem().generic_string() + format;
    }
}

void VulkanShaderManager::CreateShaderResourceFromSpvReflectModule(vk::Device _device, ShaderInternal* _shaderInternal)
{
    const std::vector<ShaderStageInfo>& shaderStageInfos = _shaderInternal->shaderStages;
    std::vector<ReflectBlockVariable>& reflectBlockVariables = _shaderInternal->reflectBlockVariables;

    std::vector<vk::PushConstantRange> pushConstantRange;
    ReflectPushConstantBlock(_device, _shaderInternal, &pushConstantRange);


    std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
    RelflectDescriptorLayout(_device, _shaderInternal, &DescriptorSetLayoutBindings);

    vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
    layoutInfo.bindingCount = static_cast<uint32_t>(DescriptorSetLayoutBindings.size());
    layoutInfo.pBindings = DescriptorSetLayoutBindings.data();

    // TODO HANDLE MORE
    _shaderInternal->descriptorSetLayouts.resize(1);
    VK_CALL(_device.createDescriptorSetLayout(&layoutInfo, nullptr, _shaderInternal->descriptorSetLayouts.data()));
    CreateDescriptorPool(_device, _shaderInternal);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(_shaderInternal->descriptorSetLayouts.size()); // Optional
    pipelineLayoutInfo.pSetLayouts = _shaderInternal->descriptorSetLayouts.data(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRange.size()); // Optional
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data(); // Optional
    _shaderInternal->pipelineLayout = _device.createPipelineLayout(pipelineLayoutInfo, nullptr);
    
}

void VulkanShaderManager::CreatePipelineGraphicPointFromModule(vk::Device _device, vk::RenderPass _renderPass,
                                                               const PC_CORE::ShaderInfo& _shaderInfo
                                                               , const std::vector<vk::PipelineShaderStageCreateInfo>&
                                                               _shaderStageCreateInfos,
                                                               vk::PipelineLayout _pipelineLayout,
                                                               vk::Pipeline* _outPipeline)
{
    // Stais call for dynamic state
    auto dynamicStates = VulkanHarwareWrapper::GetDynamicState();

    // DYNAMIC STATE SO DONT CARE
    VkViewport viewport{};
    VkRect2D scissor{};


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
    rasterizer.polygonMode = RhiPolygonModeToVulkan(std::get<PC_CORE::ShaderGraphicPointInfo>(_shaderInfo.shaderInfoData).polygonMode);
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
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

    
    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = vk::CompareOp::eLess;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
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


    std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescription{};
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions{};
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = GetVertexInputStateCreateInfoFromShaderStruct(
        std::get<PC_CORE::ShaderGraphicPointInfo>(_shaderInfo.shaderInfoData),
        &vertexInputBindingDescription, &vertexInputAttributeDescriptions);


    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    pipelineInfo.stageCount = static_cast<uint32_t>(_shaderStageCreateInfos.size());
    pipelineInfo.pStages = _shaderStageCreateInfos.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CALL(_device.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, _outPipeline));
}


vk::PipelineVertexInputStateCreateInfo VulkanShaderManager::GetVertexInputStateCreateInfoFromShaderStruct(
    const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInfo,
    std::vector<vk::VertexInputBindingDescription>* _bindingDescriptions,
    std::vector<vk::VertexInputAttributeDescription>* _attributeDescriptions)
{
    // Biding Description
    _bindingDescriptions->resize(_shaderGraphicPointInfo.vertexInputBindingDescritions.size());
    for (size_t i = 0; i < _shaderGraphicPointInfo.vertexInputBindingDescritions.size(); i++)
    {
        _bindingDescriptions->at(i).binding = _shaderGraphicPointInfo.vertexInputBindingDescritions[i].binding;
        _bindingDescriptions->at(i).stride = _shaderGraphicPointInfo.vertexInputBindingDescritions[i].stride;
        _bindingDescriptions->at(i).inputRate = RhiInputRateToVkInputRate(
            _shaderGraphicPointInfo.vertexInputBindingDescritions[i].vertexInputRate);
    }
    // Attribute description
    _attributeDescriptions->resize(_shaderGraphicPointInfo.vertexAttributeDescriptions.size());
    for (size_t i = 0; i < _shaderGraphicPointInfo.vertexAttributeDescriptions.size(); i++)
    {
        const PC_CORE::VertexAttributeDescription& attributeDesription = _shaderGraphicPointInfo.
            vertexAttributeDescriptions[i];

        _attributeDescriptions->at(i).binding = attributeDesription.binding;
        _attributeDescriptions->at(i).location = attributeDesription.location;
        _attributeDescriptions->at(i).format = Vulkan::RHIFormatToVkFormat(attributeDesription.format);
        _attributeDescriptions->at(i).offset = attributeDesription.offset;
    }

    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    pipelineVertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    // Out binding description
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(_bindingDescriptions->
        size());
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = _bindingDescriptions->data();

    // Out attribute Description
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributeDescriptions->
        size());
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = _attributeDescriptions->data();

    return pipelineVertexInputStateCreateInfo;
}


void VulkanShaderManager::DestroyInternalShaders(vk::Device _device,
                                                 ShaderInternal* _shaderInternalBack)
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
        _device.destroyPipeline(_shaderInternalBack->pipeline);

    // Destroy pipelune layout
    if (_shaderInternalBack->pipelineLayout != VK_NULL_HANDLE)
        _device.destroyPipelineLayout(_shaderInternalBack->pipelineLayout);

    // Destroy descriptorSetLayouts
    for (auto& descriptorSetLayout : _shaderInternalBack->descriptorSetLayouts)
    {
        if (descriptorSetLayout != VK_NULL_HANDLE)
            _device.destroyDescriptorSetLayout(descriptorSetLayout);
    }


    if (_shaderInternalBack->descriptorPool != VK_NULL_HANDLE)
        _device.destroyDescriptorPool(_shaderInternalBack->descriptorPool);
}


bool VulkanShaderManager::DestroyShader(vk::Device _device, const std::string& _shaderName)
{
    const bool success = m_InternalShadersMap.contains(_shaderName);
    if (!success)
        return success;

    auto it = m_InternalShadersMap.at(_shaderName);
    DestroyInternalShaders(_device, &it);

    m_InternalShadersMap.erase(_shaderName);
    return true;
}

#pragma region CreateShadersResourceFromReflection

bool Vulkan::VulkanShaderManager::CreateShaderFromSource(vk::Device _device, vk::RenderPass _tmprRenderPass,
                                                         const PC_CORE::ProgramShaderCreateInfo&
                                                         _programShaderCreatInfo,
                                                         const std::vector<PC_CORE::ShaderSourcePath>& _shaderSource)
{
    ShaderInternal shaderInternalBack = {};

    // determine shader stage type and name
    FillShaderInfo(&shaderInternalBack, _shaderSource);
    std::vector<ShaderStageInfo>& shaderStagesInfo = shaderInternalBack.shaderStages;
    std::vector<vk::ShaderModule> shaderModules(shaderInternalBack.shaderStages.size());
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStagesCreateInfos(shaderInternalBack.shaderStages.size());

    for (uint32_t i = 0; i < _shaderSource.size(); i++)
    {
        PC_CORE::ShaderStageType shaderStage = shaderStagesInfo.at(i).shaderStage;
        // SOURCE TO MODULE and get spv reflection
        m_ShaderCompiler.CreateModuleFromSource(_device, _shaderSource[i].spvCode.data(),
                                                _shaderSource[i].shaderSourceCodePath.c_str(),
                                                shaderStage, &shaderStagesInfo.at(i).reflectShaderModule,
                                                &shaderModules[i]);

        shaderStagesCreateInfos.at(i).sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStagesCreateInfos.at(i).stage = ShaderBitFromType(shaderStage);
        shaderStagesCreateInfos.at(i).module = shaderModules[i];
        shaderStagesCreateInfos.at(i).pName = shaderStagesInfo.at(i).reflectShaderModule.entry_point_name;
    }

    CreateShaderResourceFromSpvReflectModule(_device, &shaderInternalBack);
    CreatePipelineGraphicPointFromModule(_device, _tmprRenderPass, _programShaderCreatInfo.shaderInfo,
                                         shaderStagesCreateInfos, shaderInternalBack.pipelineLayout,
                                         &shaderInternalBack.pipeline);

    shaderInternalBack.pipelineBindPoint = RhiPipelineBindPointToVulkan(
        _programShaderCreatInfo.shaderInfo.shaderProgramPipelineType);

    // graphic and layout as been created no need module modules then
    for (auto& module : shaderModules)
    {
        _device.destroyShaderModule(module);
    }

    m_InternalShadersMap.insert({_programShaderCreatInfo.prograShaderName, shaderInternalBack});

    return true;
}

void VulkanShaderManager::CreateDescriptorPool(vk::Device _device, ShaderInternal* _shaderInternal)
{
    std::vector<vk::DescriptorPoolSize> descriptorPoolSize;

    const std::vector<ShaderStageInfo>& shaderStageInfos = _shaderInternal->shaderStages;
    size_t currentDescriptorPoolSize = 0;
    for (size_t i = 0; i < shaderStageInfos.size(); i++)
    {
        const ShaderStageInfo& shaderStageInfo = shaderStageInfos[i];
        const SpvReflectShaderModule* spvReflectBlockVariable = &shaderStageInfo.reflectShaderModule;

        size_t descriptorBindingCount = static_cast<size_t>(spvReflectBlockVariable->descriptor_binding_count);
        descriptorPoolSize.resize(descriptorPoolSize.size() + descriptorBindingCount);

        for (size_t j = 0; j < descriptorBindingCount; j++)
        {
            const SpvReflectDescriptorBinding* descriptorBindingReflected = &spvReflectBlockVariable->
                descriptor_bindings[j];
            descriptorPoolSize.at(currentDescriptorPoolSize).type = static_cast<vk::DescriptorType>(
                descriptorBindingReflected->descriptor_type);
            descriptorPoolSize.at(currentDescriptorPoolSize).descriptorCount = descriptorBindingReflected->count;

            currentDescriptorPoolSize++;
        }
    }

    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSize.size());
    descriptorPoolInfo.pPoolSizes = descriptorPoolSize.data();
    // TO DO MAKE IT PARAMETERABLE
    descriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet; 
    // TODO MAY CHANGE
    descriptorPoolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
    _shaderInternal->descriptorPool = _device.createDescriptorPool(descriptorPoolInfo, nullptr);
}

void VulkanShaderManager::AllocDescriptorSet(const std::string& _shaderName,vk::DescriptorSet* _descriptorSet, uint32_t _descriptorSetCount, vk::Device _device)
{
    const ShaderInternal& shaderInternal = GetShader(_shaderName);
    
    std::vector<vk::DescriptorSetLayout> layouts(_descriptorSetCount, shaderInternal.descriptorSetLayouts[0]);
    
    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = shaderInternal.descriptorPool;
    descriptorSetAllocateInfo.pSetLayouts = layouts.data();
    descriptorSetAllocateInfo.descriptorSetCount = _descriptorSetCount;

    VK_CALL(_device.allocateDescriptorSets(&descriptorSetAllocateInfo, _descriptorSet));
}

void VulkanShaderManager::FreeDescriptorSet(const std::string& _shaderName, vk::DescriptorSet* _descriptorSet,
    uint32_t _descriptorSetCount, vk::Device _device)
{
    const ShaderInternal& shaderInternal = GetShader(_shaderName);
    
    VK_CALL(_device.freeDescriptorSets(shaderInternal.descriptorPool, _descriptorSetCount, _descriptorSet));
}


#pragma endregion CreateShadersResourceFromReflection

#pragma region PARSING

vk::ShaderStageFlagBits VulkanShaderManager::ShaderBitFromType(const PC_CORE::ShaderStageType _shaderType)
{
    switch (_shaderType)
    {
    case PC_CORE::ShaderStageType::VERTEX:
        return vk::ShaderStageFlagBits::eVertex;
        break;
    case PC_CORE::ShaderStageType::FRAGMENT:
        return vk::ShaderStageFlagBits::eFragment;
        break;
    case PC_CORE::ShaderStageType::GEOMETRY:
        return vk::ShaderStageFlagBits::eGeometry;
        break;
    case PC_CORE::ShaderStageType::TESSELATION:
        assert(false, "NOTSUPPORTED SHADER TESSELATION");
        return vk::ShaderStageFlagBits::eTessellationControl;
        break;
    case PC_CORE::ShaderStageType::COMPUTE:
        return vk::ShaderStageFlagBits::eCompute;
        break;
    case PC_CORE::ShaderStageType::COUNT:
        break;
    }
}

#pragma endregion PARSING

#pragma region SpvReflect


void Vulkan::VulkanShaderManager::ReflectPushConstantBlock(vk::Device _device, ShaderInternal* _shaderInternal,
                                                           std::vector<vk::PushConstantRange>* _pushConstantRange)
{
    const std::vector<ShaderStageInfo>& shaderStageInfos = _shaderInternal->shaderStages;
    std::vector<ReflectBlockVariable>& reflectBlockVariables = _shaderInternal->reflectBlockVariables;

    // PUSH CONSTANT
    uint32_t pushConstantSize = 0;
    for (size_t i = 0; i < shaderStageInfos.size(); i++)
    {
        const SpvReflectShaderModule spvReflectShaderModule = shaderStageInfos[i].reflectShaderModule;
        pushConstantSize += spvReflectShaderModule.push_constant_block_count;
    }
    reflectBlockVariables.resize(pushConstantSize);

    size_t reflectedBlockVariableIndex = 0;
    // for each shader
    for (size_t i = 0; i < shaderStageInfos.size(); i++)
    {
        const SpvReflectShaderModule& spvReflectShaderModule = shaderStageInfos[i].reflectShaderModule;
        // for each Block

        for (size_t j = 0; j < spvReflectShaderModule.push_constant_block_count; j++)
        {
            SpvReflectBlockVariable* spvReflectBlockVariable = &spvReflectShaderModule.push_constant_blocks[j];

            ReflectBlockVariable* reflectBlockVariable = &reflectBlockVariables[reflectedBlockVariableIndex++];

            reflectBlockVariable->stageFlags = static_cast<vk::ShaderStageFlagBits>(spvReflectShaderModule.
                shader_stage);
            reflectBlockVariable->name = spvReflectBlockVariable->name;
            reflectBlockVariable->size = spvReflectBlockVariable->size;
            reflectBlockVariable->absoluteOffSet = spvReflectBlockVariable->absolute_offset;
            reflectBlockVariable->members = {};


            ReflectMember(spvReflectBlockVariable, reflectBlockVariable, reflectBlockVariable->stageFlags);
        }
    }


    _pushConstantRange->reserve(reflectBlockVariables.size());

    for (size_t i = 0; i < reflectBlockVariables.size(); i++)
    {
        _pushConstantRange->emplace_back(reflectBlockVariables[i].stageFlags, reflectBlockVariables[i].absoluteOffSet,
                                         reflectBlockVariables[i].size);
    }
}

void Vulkan::VulkanShaderManager::RelflectDescriptorLayout(vk::Device _device, ShaderInternal* _shaderInternal,
                                                           std::vector<vk::DescriptorSetLayoutBinding>*
                                                           _DescriptorSetLayoutBindings)
{
    // TO DO
    const std::vector<ShaderStageInfo>& shaderStageInfos = _shaderInternal->shaderStages;

    size_t currentDescriptorLayout = 0;
    for (size_t i = 0; i < shaderStageInfos.size(); i++)
    {
        const ShaderStageInfo& shaderStageInfo = shaderStageInfos[i];
        const SpvReflectShaderModule* spvReflectBlockVariable = &shaderStageInfo.reflectShaderModule;

        size_t descriptorBindingCount = static_cast<size_t>(spvReflectBlockVariable->descriptor_binding_count);
        _DescriptorSetLayoutBindings->resize(_DescriptorSetLayoutBindings->size() + descriptorBindingCount);
        for (size_t j = 0; j < descriptorBindingCount; j++)
        {
            const SpvReflectDescriptorBinding* descriptorBindingReflected = &spvReflectBlockVariable->
                descriptor_bindings[j];
            _DescriptorSetLayoutBindings->at(currentDescriptorLayout).binding = descriptorBindingReflected->binding;
            _DescriptorSetLayoutBindings->at(currentDescriptorLayout).descriptorType = static_cast<vk::DescriptorType>(
                descriptorBindingReflected->descriptor_type);
            _DescriptorSetLayoutBindings->at(currentDescriptorLayout).descriptorCount = 1;
            _DescriptorSetLayoutBindings->at(currentDescriptorLayout).stageFlags = static_cast<vk::ShaderStageFlags>(
                spvReflectBlockVariable->shader_stage);

            currentDescriptorLayout++;
        }
    }
}

void VulkanShaderManager::ReflectMember(SpvReflectBlockVariable* spvReflectBlockVariable,
                                        ReflectBlockVariable* reflectBlockVariable, vk::ShaderStageFlags _stageFlags)
{
    if (spvReflectBlockVariable->member_count == 0)
        return;

    reflectBlockVariable->members.reserve(spvReflectBlockVariable->member_count);

    for (size_t i = 0; i < spvReflectBlockVariable->member_count; i++)
    {
        reflectBlockVariable->members.emplace_back(_stageFlags, spvReflectBlockVariable->members[i].name,
                                                   spvReflectBlockVariable->members[i].size,
                                                   spvReflectBlockVariable->members[i].absolute_offset,
                                                   std::vector<ReflectBlockVariable>()
        );

        ReflectMember(&spvReflectBlockVariable->members[i], &reflectBlockVariable[i], _stageFlags);
    }
}


#pragma endregion SpvReflect
