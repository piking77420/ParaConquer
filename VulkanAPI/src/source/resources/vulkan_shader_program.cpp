#include <spriv_reflect/spirv_reflect.h>
#include "resources/vulkan_shader_program.hpp"

#include <map>

#include "resources/vulkan_descriptor_sets.hpp"
#include "rhi_vulkan_parser.hpp"
#include "vulkan_context.hpp"
#include "vulkan_render_pass.hpp"
#include "io/in_out.h"

using namespace Vulkan;





VulkanShaderProgram::~VulkanShaderProgram()
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice)->GetDevice();

    for (size_t i = 0; i < m_DescriptorSetLayout.size(); i++)
    {
        if (m_DescriptorSetLayout[i] != VK_NULL_HANDLE)
        {
            device.destroyDescriptorSetLayout(m_DescriptorSetLayout[i]);
            m_DescriptorSetLayout[i] = VK_NULL_HANDLE;
        }
    }
   

    if (m_DescriptorPool != VK_NULL_HANDLE)
    {
        device.destroyDescriptorPool(m_DescriptorPool);
        m_DescriptorPool = VK_NULL_HANDLE;
    }

    if (m_PipelineLayout != VK_NULL_HANDLE)
    {
        device.destroyPipelineLayout(m_PipelineLayout);
        m_PipelineLayout = VK_NULL_HANDLE;
    }

    if (m_Pipeline != VK_NULL_HANDLE)
    {
        device.destroyPipeline(m_Pipeline);
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

vk::PipelineLayout VulkanShaderProgram::GetPipelineLayout() const
{
    return m_PipelineLayout;
}

void VulkanShaderProgram::AllocDescriptorSet(PC_CORE::ShaderProgramDescriptorSets** shaderProgramDescriptorSets, size_t set)
{
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout[set]);
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice)->GetDevice();
    VulkanDescriptorSets* vulkanDescriptorSets = new VulkanDescriptorSets();
    
    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
    descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    descriptorSetAllocateInfo.pSetLayouts = layouts.data();
    
    
    VK_CALL(device.allocateDescriptorSets(&descriptorSetAllocateInfo, vulkanDescriptorSets->descriptorSets.data()));

    *shaderProgramDescriptorSets = vulkanDescriptorSets;
    m_DescriptorSetAllocCount++;
}

void VulkanShaderProgram::FreeDescriptorSet(PC_CORE::ShaderProgramDescriptorSets** shaderProgramDescriptorSets)
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice)->GetDevice();
    VulkanDescriptorSets* vulkanDescriptorSets = reinterpret_cast<VulkanDescriptorSets*>(*shaderProgramDescriptorSets);

    device.freeDescriptorSets(m_DescriptorPool, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT), vulkanDescriptorSets->descriptorSets.data());
    
    delete *shaderProgramDescriptorSets;
    *shaderProgramDescriptorSets = nullptr;
    m_DescriptorSetAllocCount = 0;
}

void VulkanShaderProgram::PushConstant(vk::CommandBuffer _commandBuffer, const std::string& _pushConstantKey, void* data, size_t _size) const
{
#ifdef _DEBUG
    if (_size > VULKAN_MAX_PUSH_CONSTANTS)
    {
        throw std::runtime_error("VULKAN_MAX_PUSH_CONSTANTS have been exceeded");
    }
#endif

    if (!m_PushConstantMap.contains(_pushConstantKey))
        return;

    const PushConstantField& pushConstatnField = m_PushConstantMap.at(_pushConstantKey);

    if (pushConstatnField.pushConstantSize != _size)
    {
        PC_LOGERROR("MissMatch size in pushconstant")
    }
    
    _commandBuffer.pushConstants(m_PipelineLayout,pushConstatnField.shaderStage, pushConstatnField.pushConstantOffSet, pushConstatnField.pushConstantSize, data);
}

VulkanShaderProgram::VulkanShaderProgram(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo) : ShaderProgram(_programShaderCreateInfo)
{
    VulkanShaderProgramCreateContex vulkanShaderProgramCreateContex = CreateShaderProgramCreateContext(m_ProgramShaderCreateInfo);
    
        
    switch (m_ProgramShaderCreateInfo.shaderInfo.shaderProgramPipelineType)
    {
    case PC_CORE::ShaderProgramPipelineType::POINT_GRAPHICS:
        {
            const PC_CORE::ShaderGraphicPointInfo& shaderGraphicPointInfo = std::get<0>(_programShaderCreateInfo.shaderInfo.shaderInfoData);
            CreatePipeLinePointGraphicsPipeline(vulkanShaderProgramCreateContex, shaderGraphicPointInfo);
        }
        break;
    case PC_CORE::ShaderProgramPipelineType::COMPUTE:
        break;
    case PC_CORE::ShaderProgramPipelineType::RAYTRACING:
        break;
    case PC_CORE::ShaderProgramPipelineType::COUT:
        break;
    default: ;
    }
    
    for (auto& spvReflect : vulkanShaderProgramCreateContex.modulesReflected)
        spvReflectDestroyShaderModule(&spvReflect);
    
    for (auto& vkShaderModule : vulkanShaderProgramCreateContex.vkShaderModules)
        vulkanShaderProgramCreateContex.device.destroyShaderModule(vkShaderModule);
}


VulkanShaderProgramCreateContex VulkanShaderProgram::CreateShaderProgramCreateContext(
    const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo)
{
    const  size_t shaderStageCount = _programShaderCreateInfo.shaderInfo.shaderSources.size();

    VulkanShaderProgramCreateContex vulkanShaderProgramCreateContex;
    vulkanShaderProgramCreateContex.device = std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice)->GetDevice();
    vulkanShaderProgramCreateContex.spvModuleSourceCode = std::vector<std::vector<char>>(shaderStageCount);
    vulkanShaderProgramCreateContex.modulesReflected = std::vector<SpvReflectShaderModule>(shaderStageCount);
    vulkanShaderProgramCreateContex.vkShaderModules = std::vector<vk::ShaderModule>(shaderStageCount);
    vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos = std::vector<vk::PipelineShaderStageCreateInfo>(shaderStageCount);

    for (size_t i = 0; i < shaderStageCount; i++)
    {
        const std::pair<PC_CORE::ShaderStageType, std::string>& shaderSource = _programShaderCreateInfo.shaderInfo.shaderSources[i];
        const char* format = nullptr;
         if (!GetFormatFromValue(PC_CORE::ShaderSourceFormat, shaderSource.first, &format))
         {
             PC_LOGERROR("Failed to parse shader source for shader ");
         }
        std::string spvFile = SHADER_CACHE_PATH + shaderSource.second;

        vulkanShaderProgramCreateContex.spvModuleSourceCode[i] = PC_CORE::InOut::ReadFile(spvFile);

    }

    for (size_t i = 0; i < vulkanShaderProgramCreateContex.spvModuleSourceCode.size(); i++)
    {
        spvReflectCreateShaderModule(vulkanShaderProgramCreateContex.spvModuleSourceCode[i].size(), vulkanShaderProgramCreateContex.spvModuleSourceCode[i].data(), &vulkanShaderProgramCreateContex.modulesReflected[i]);
    }
    
    // Reflection Start
    ParseSpvRelfection(vulkanShaderProgramCreateContex);
    CreatePushConstantMapFromReflection(vulkanShaderProgramCreateContex.modulesReflected);
    
    // Create Modules
    for (size_t i = 0; i < vulkanShaderProgramCreateContex.spvModuleSourceCode.size(); i++)
    {
        vk::ShaderModuleCreateInfo createInfo = {};
        createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        createInfo.codeSize = vulkanShaderProgramCreateContex.spvModuleSourceCode[i].size();
        createInfo.pCode = reinterpret_cast<uint32_t*>(vulkanShaderProgramCreateContex.spvModuleSourceCode[i].data());
        
        VK_CALL(vulkanShaderProgramCreateContex.device.createShaderModule(&createInfo, nullptr, &vulkanShaderProgramCreateContex.vkShaderModules[i]));
    }

    // CreatePipelineShaderStage 
    for (size_t i = 0; i < vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos.size(); i++)
    {
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].stage = RhiToShaderStage(_programShaderCreateInfo.shaderInfo.shaderSources[i].first);
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].module = vulkanShaderProgramCreateContex.vkShaderModules[i];
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].pName = vulkanShaderProgramCreateContex.modulesReflected[i].entry_point_name;
    }

    return vulkanShaderProgramCreateContex;
}



void VulkanShaderProgram::CreatePipeLinePointGraphicsPipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex, const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInfo)
{
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice);

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateArray.size());
    dynamicState.pDynamicStates = dynamicStateArray.data();

    // VertexInput
    std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = ParseVertexInputState(_shaderGraphicPointInfo, &vertexInputBindingDescriptions, &vertexInputAttributeDescriptions);
    
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
    ParseRasterizer(&rasterizer, _shaderGraphicPointInfo.rasterizerInfo);
    
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


    vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = vk::CompareOp::eLess;
    depthStencilState.minDepthBounds = 0.0f;
    depthStencilState.maxDepthBounds = 1.0f;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.front = vk::StencilOpState(); // Optional
    depthStencilState.back = vk::StencilOpState(); // Optional

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    ParseParsePipelineColorBlendAttachmentState(&colorBlending, &colorBlendAttachment);


    vk::PipelineLayoutCreateInfo  pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_DescriptorSetLayout.size()); // Optional
    pipelineLayoutInfo.pSetLayouts = m_DescriptorSetLayout.data(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(_vulkanShaderProgramCreateContex.pushConstantRanges.size()); // Optional
    pipelineLayoutInfo.pPushConstantRanges = _vulkanShaderProgramCreateContex.pushConstantRanges.data(); // Optional

    m_PipelineLayout = device->GetDevice().createPipelineLayout(pipelineLayoutInfo);

 
    
    vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{};
    graphicsPipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    graphicsPipelineInfo.stageCount = static_cast<uint32_t>(_vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos.size());
    graphicsPipelineInfo.pStages = _vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos.data();
    graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
    graphicsPipelineInfo.pInputAssemblyState = &inputAssembly;
    graphicsPipelineInfo.pViewportState = &viewportState;
    graphicsPipelineInfo.pRasterizationState = &rasterizer;
    
    if (_shaderGraphicPointInfo.enableDepthTest)
        graphicsPipelineInfo.pDepthStencilState = &depthStencilState;
    
    graphicsPipelineInfo.pMultisampleState = &multisampling;
    graphicsPipelineInfo.pColorBlendState = &colorBlending;
    graphicsPipelineInfo.pDynamicState = &dynamicState;
    graphicsPipelineInfo.layout = m_PipelineLayout;
    graphicsPipelineInfo.renderPass = std::reinterpret_pointer_cast<VulkanRenderPass>(m_ProgramShaderCreateInfo.renderPass)->GetVulkanRenderPass();
    graphicsPipelineInfo.subpass = 0;

    auto result = device->GetDevice().createGraphicsPipeline(nullptr, graphicsPipelineInfo);
    m_Pipeline = result.value;
}

void VulkanShaderProgram::CreatePushConstantMapFromReflection(const std::vector<SpvReflectShaderModule>& _spvReflectShaderModule)
{
    for (size_t module = 0; module < _spvReflectShaderModule.size(); module++)
    {
        for (size_t pushConstant = 0; pushConstant < _spvReflectShaderModule[module].push_constant_block_count; pushConstant++)
        {
            SpvReflectBlockVariable* pushConstantBlock = _spvReflectShaderModule[module].push_constant_blocks;
            if (pushConstantBlock == nullptr)
                continue;

            const PushConstantField pushConstantField =
                {
                .pushConstantSize = pushConstantBlock->size,
                .pushConstantOffSet = pushConstantBlock->absolute_offset,
                .shaderStage = static_cast<vk::ShaderStageFlags>(_spvReflectShaderModule[module].shader_stage)
                };


            m_PushConstantMap.insert({pushConstantBlock[pushConstant].name, pushConstantField});
        }
    }
}
#pragma region ParseRegion

void VulkanShaderProgram::ParseSpvRelfection(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContext)
{
    std::map<vk::DescriptorType, uint32_t> descriptorTypeCount;
    
    std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> layoutsMap;
    std::vector<vk::PushConstantRange>* pushConstantRanges = &_vulkanShaderProgramCreateContext.pushConstantRanges;

    // Compute Unique Set
    for (auto& moduleIndex : _vulkanShaderProgramCreateContext.modulesReflected)
        for (size_t i = 0; i < moduleIndex.descriptor_set_count; i++)
        {
            if (moduleIndex.descriptor_sets[i].set != std::numeric_limits<uint32_t>::max())
            {
                layoutsMap.insert({ moduleIndex.descriptor_bindings[i].set ,{} });
            }
        }
    
    // Resize Vector
    uint32_t pushConstantRangeCount = 0;
    for (auto& moduleIndex : _vulkanShaderProgramCreateContext.modulesReflected)
    {
        //for (auto& set : moduleIndex.descriptor_sets)
        //{
            //if (set.set == std::numeric_limits<uint32_t>::max())
              //  continue;
            //layoutsMap[set.set].resize(layoutsMap[set.set].size() + set.binding_count);
        //}
        pushConstantRangeCount += moduleIndex.push_constant_block_count;
    }
    pushConstantRanges->resize(pushConstantRangeCount);


    uint32_t descritptorCount = 0;
    for (auto& moduleIndex : _vulkanShaderProgramCreateContext.modulesReflected)
    {
        for (size_t setIndex = 0 ; setIndex < moduleIndex.descriptor_set_count; setIndex++)
        {
            SpvReflectDescriptorSet& s = moduleIndex.descriptor_sets[setIndex];

            if (s.binding_count == 0)
                continue;


            for (size_t descriptorIndex = 0; descriptorIndex < s.binding_count; descriptorIndex++)
            {
                 SpvReflectDescriptorBinding& spvBinding = *s.bindings[descriptorIndex];

                 vk::DescriptorSetLayoutBinding descriptorSetLayout{};
                descriptorSetLayout.binding =  spvBinding.binding;
                descriptorSetLayout.descriptorType = static_cast<vk::DescriptorType>(spvBinding.descriptor_type);
                descriptorSetLayout.descriptorCount = spvBinding.count;
                descriptorSetLayout.stageFlags = static_cast<vk::ShaderStageFlags>(moduleIndex.shader_stage);
                descriptorSetLayout.pImmutableSamplers = nullptr;// optional

                descriptorTypeCount[static_cast<vk::DescriptorType>(spvBinding.descriptor_type)] += spvBinding.count;
                descritptorCount += spvBinding.count;

                layoutsMap[s.set].push_back(descriptorSetLayout);
            }
        }
    }

    pushConstantRangeCount = 0;

    // PushRange
    for (size_t i = 0; i < _vulkanShaderProgramCreateContext.modulesReflected.size(); i++)
    {
        const uint32_t pushConstantCount = _vulkanShaderProgramCreateContext.modulesReflected[i].push_constant_block_count;
        if (pushConstantCount == 0)
            continue;

        for (size_t j = 0; j < pushConstantCount; j++)
        {
            SpvReflectBlockVariable* spvReflectBlockVariablePushConstant = _vulkanShaderProgramCreateContext.modulesReflected[j].push_constant_blocks;

            vk::PushConstantRange& pushConstantRange = pushConstantRanges->at(pushConstantRangeCount);
            pushConstantRange.offset = spvReflectBlockVariablePushConstant->offset;
            pushConstantRange.size = spvReflectBlockVariablePushConstant->size;
            pushConstantRange.stageFlags = static_cast<vk::ShaderStageFlags>(_vulkanShaderProgramCreateContext.modulesReflected[i].shader_stage);
            pushConstantRangeCount++;
        }
    }

    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> layouts;
    layouts.reserve(layoutsMap.size());

    for (auto& l : layoutsMap)
    {
        layouts.emplace_back(l.second);
    }

    // DescriptoSetLayout
    m_DescriptorSetLayout.reserve(layouts.size());
    for (size_t i = 0; i < layouts.size(); i++)
    {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
        descriptorSetLayoutCreateInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
        descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layouts[i].size());
        descriptorSetLayoutCreateInfo.pBindings = layouts[i].data();

       m_DescriptorSetLayout.emplace_back(_vulkanShaderProgramCreateContext.device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo));
    }


    // Descriptor Pool
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes(descriptorTypeCount.size());

    size_t i = 0;
    for (auto it = descriptorTypeCount.begin(); it != descriptorTypeCount.end();)
    {
        vk::DescriptorPoolSize& descriptorPoolSize = descriptorPoolSizes[i];
        descriptorPoolSize.type = it->first;
        descriptorPoolSize.descriptorCount = it->second * static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        i++;
        it++;
    }
    
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolCreateInfo.maxSets = MAX_ALLOC_DESCRIPTOR_SET;

    m_DescriptorPool = _vulkanShaderProgramCreateContext.device.createDescriptorPool(descriptorPoolCreateInfo);

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

vk::VertexInputBindingDescription VulkanShaderProgram::ParseVertexInputBindingDescription(
    const PC_CORE::VertexInputBindingDescrition& _vertexInputBindingDescrition)
{
    vk::VertexInputBindingDescription vkvertexInputBindingDescription{};
    vkvertexInputBindingDescription.binding = _vertexInputBindingDescrition.binding;
    vkvertexInputBindingDescription.stride = _vertexInputBindingDescrition.stride;
    vkvertexInputBindingDescription.inputRate = RhiInputRateToVkInputRate(_vertexInputBindingDescrition.vertexInputRate);

    return vkvertexInputBindingDescription;
}

vk::VertexInputAttributeDescription VulkanShaderProgram::ParseVertexInputAttributeDescription(
    const PC_CORE::VertexAttributeDescription& _vertexAttributeDescription)
{
    vk::VertexInputAttributeDescription vkvertexInputAttributeDescription;
    vkvertexInputAttributeDescription.location = _vertexAttributeDescription.location;
    vkvertexInputAttributeDescription.binding = _vertexAttributeDescription.binding;
    vkvertexInputAttributeDescription.offset = _vertexAttributeDescription.offset;
    vkvertexInputAttributeDescription.format = RHIFormatToVkFormat(_vertexAttributeDescription.format);

    return vkvertexInputAttributeDescription;
}

vk::PipelineVertexInputStateCreateInfo VulkanShaderProgram::ParseVertexInputState(
    const PC_CORE::ShaderGraphicPointInfo& _shaderGraphicPointInfo,
    std::vector<vk::VertexInputBindingDescription>* _vertexInputBindingDescriptions
    ,std::vector<vk::VertexInputAttributeDescription>* _vertexInputAttributeDescriptions)
{
    if (_vertexInputBindingDescriptions == nullptr || _vertexInputAttributeDescriptions == nullptr)
    {
        PC_LOGERROR("_vertexInputBindingDescriptions is null or _vertexInputAttributeDescriptions is null");
        return {};
    }

    // Parse vertexInputBindingDescriptions
    _vertexInputBindingDescriptions->reserve(_shaderGraphicPointInfo.vertexInputBindingDescritions.size());
    for (size_t i = 0; i < _shaderGraphicPointInfo.vertexInputBindingDescritions.size(); i++)
    {
        _vertexInputBindingDescriptions->emplace_back(ParseVertexInputBindingDescription(_shaderGraphicPointInfo.vertexInputBindingDescritions[i]));
    }

    // Parse verteixAttributes
    _vertexInputAttributeDescriptions->reserve(_shaderGraphicPointInfo.vertexAttributeDescriptions.size());
    for (size_t i = 0; i < _shaderGraphicPointInfo.vertexAttributeDescriptions.size(); i++)
    {
        _vertexInputAttributeDescriptions->emplace_back(ParseVertexInputAttributeDescription(_shaderGraphicPointInfo.vertexAttributeDescriptions[i]));
    }
    
    vk::PipelineVertexInputStateCreateInfo returnVertexInputStateCreateInfo{};
    returnVertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    returnVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(_vertexInputBindingDescriptions->size());
    returnVertexInputStateCreateInfo.pVertexBindingDescriptions = _vertexInputBindingDescriptions->data();
    
    returnVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_vertexInputAttributeDescriptions->size());
    returnVertexInputStateCreateInfo.pVertexAttributeDescriptions = _vertexInputAttributeDescriptions->data();

    return returnVertexInputStateCreateInfo;
}
#pragma endregion ParseRegion 
