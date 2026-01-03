#include "Resources/VulkanShaderProgram.hpp"

#include <map>
#include <string_view>

#include "include/spirv/unified1/spirv.h"
#include "Resources/ResourceFormat.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiRenderPass.hpp"
#include "Resources/VulkanDescriptorSets.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "VulkanContext.hpp"
#include "VulkanDescritptorManager.hpp"
#include "VulkanRenderPass.hpp"


using namespace Vulkan;

VulkanShaderProgram::~VulkanShaderProgram()
{
    vk::Device device = GET_VK_DEVICE;

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
    switch (m_Type)
    {
    case PipelineType::Graphic:
        return vk::PipelineBindPoint::eGraphics;
    case PipelineType::Compute:
        return vk::PipelineBindPoint::eCompute;
    case PipelineType::RayTracing:
        return vk::PipelineBindPoint::eRayTracingKHR;
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

void VulkanShaderProgram::PushConstant(vk::CommandBuffer _commandBuffer, const std::string& _pushConstantKey,
                                       const void* data, size_t _size) const
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

#ifdef _DEBUG
    if (_size > VULKAN_MAX_PUSH_CONSTANTS)
    {
        throw std::runtime_error("VULKAN_MAX_PUSH_CONSTANTS have been exceeded");
    }
#endif

    if (!m_PushConstantMap.contains(_pushConstantKey))
    {
        PC_LOGERROR("There is no PushConstant name as {}, in {}", _pushConstantKey,
            GetName().data())
        return;
    }

    const PushConstantField& pushConstatnField = m_PushConstantMap.at(_pushConstantKey);

    if (pushConstatnField.pushConstantSize != _size)
    {
        PC_LOGERROR("MissMatch size in pushconstant")
    }

    _commandBuffer.pushConstants(m_PipelineLayout, pushConstatnField.shaderStage,
                                 pushConstatnField.pushConstantOffSet, pushConstatnField.pushConstantSize, data);
}

VulkanShaderProgram::VulkanShaderProgram(PC_CORE::Rhi& _Rhi) :
    RhiShaderProgram(_Rhi)
{
   
}

bool VulkanShaderProgram::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    
    if (!m_Modules)
        return false;

    if (m_Modules->empty())
        return false;


    VulkanShaderProgramCreateContex vulkanShaderProgramCreateContex = CreateShaderProgramCreateContext(*m_Modules);
    return CreateFromContext(vulkanShaderProgramCreateContex);
}


bool Vulkan::VulkanShaderProgram::CreateFromContext(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    switch (m_Type)
    {
    case PipelineType::Graphic:
        CreatePipeLinePointGraphicsPipeline(_vulkanShaderProgramCreateContex);
        break;
    case PipelineType::Compute:
        CreateComputePipeline(_vulkanShaderProgramCreateContex);

        break;
    case PipelineType::RayTracing:
    case PipelineType::Count:
    default:
        return false;
    }

    for (auto& spvReflect : _vulkanShaderProgramCreateContex.modulesReflected)
        spvReflectDestroyShaderModule(&spvReflect);

    for (auto& vkShaderModule : _vulkanShaderProgramCreateContex.vkShaderModules)
        _vulkanShaderProgramCreateContex.device.destroyShaderModule(vkShaderModule);

    m_Modules.reset();
}

VulkanShaderProgramCreateContex VulkanShaderProgram::CreateShaderProgramCreateContext(const std::vector<ShaderModule>& _programShaderCreateInfo, bool _createDescriptorResources)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const size_t shaderStageCount = _programShaderCreateInfo.size();

    VulkanShaderProgramCreateContex vulkanShaderProgramCreateContex;
    vulkanShaderProgramCreateContex.device = std::reinterpret_pointer_cast<VulkanDevice>(
        GET_VK_CONTEXT.rhiDevice)->GetDevice();
    vulkanShaderProgramCreateContex.spvModuleSourceCode = std::vector<std::vector<char>>(shaderStageCount);
    vulkanShaderProgramCreateContex.modulesReflected = std::vector<SpvReflectShaderModule>(shaderStageCount);
    vulkanShaderProgramCreateContex.vkShaderModules = std::vector<vk::ShaderModule>(shaderStageCount);
    vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos = std::vector<vk::PipelineShaderStageCreateInfo>(
        shaderStageCount);

    for (size_t i = 0; i < shaderStageCount; i++)
    {
        const ShaderModule& shaderSource = _programShaderCreateInfo[i];
        const char* format = nullptr;
        if (!PC_CORE::GetFormatFromValue(ShaderSourceFormat, shaderSource.first, &format))
        {
            PC_LOGERROR("Failed to parse shader source for shader ");
        }
        vulkanShaderProgramCreateContex.spvModuleSourceCode[i] = shaderSource.second;
    }

    for (size_t i = 0; i < vulkanShaderProgramCreateContex.spvModuleSourceCode.size(); i++)
        spvReflectCreateShaderModule(vulkanShaderProgramCreateContex.spvModuleSourceCode[i].size(),
                                     vulkanShaderProgramCreateContex.spvModuleSourceCode[i].data(),
                                     &vulkanShaderProgramCreateContex.modulesReflected[i]);

    // Reflection Start
    if (_createDescriptorResources)
        ParseDescriptor(vulkanShaderProgramCreateContex);

    ParsePushConstantRange(vulkanShaderProgramCreateContex);
    CreatePushConstantMapFromReflection(vulkanShaderProgramCreateContex.modulesReflected);

    // Create Modules
    for (size_t i = 0; i < vulkanShaderProgramCreateContex.spvModuleSourceCode.size(); i++)
    {
        vk::ShaderModuleCreateInfo createInfo = {};
        createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        createInfo.codeSize = vulkanShaderProgramCreateContex.spvModuleSourceCode[i].size();
        createInfo.pCode = reinterpret_cast<uint32_t*>(vulkanShaderProgramCreateContex.spvModuleSourceCode[i].data());

        VK_CALL(
            vulkanShaderProgramCreateContex.device.createShaderModule(&createInfo, nullptr, &
                vulkanShaderProgramCreateContex.vkShaderModules[i]));
    }

    // CreatePipelineShaderStage 
    for (size_t i = 0; i < vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos.size(); i++)
    {
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].sType =
            vk::StructureType::ePipelineShaderStageCreateInfo;
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].stage = Utils::RhiToShaderStage(
            _programShaderCreateInfo[i].first);
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].module = vulkanShaderProgramCreateContex.
            vkShaderModules[i];
        vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[i].pName = vulkanShaderProgramCreateContex.
            modulesReflected[i].entry_point_name;
    }

    return vulkanShaderProgramCreateContex;
}


void VulkanShaderProgram::CreateComputePipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    vk::Device d = GET_VK_DEVICE;

    if (_vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos.size() != 1)
    {
        PC_LOGERROR("Compute Pipeline shoulde have one shader stage");
        return;
    }

    assert(_vulkanShaderProgramCreateContex.modulesReflected.size() == 1 && 
        _vulkanShaderProgramCreateContex.modulesReflected[0].shader_stage & SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT);

    std::get<ComputePipelineData>(m_PipelineData).LocalSize =
    {
        _vulkanShaderProgramCreateContex.modulesReflected[0].entry_points[0].local_size.x,
        _vulkanShaderProgramCreateContex.modulesReflected[0].entry_points[0].local_size.y,
        _vulkanShaderProgramCreateContex.modulesReflected[0].entry_points[0].local_size.z
    };

    CreatePipelineLayout(d, _vulkanShaderProgramCreateContex);

    vk::ComputePipelineCreateInfo vkComputeCreateInfo;
    vkComputeCreateInfo.sType = vk::StructureType::eComputePipelineCreateInfo;
    vkComputeCreateInfo.layout = m_PipelineLayout;
    vkComputeCreateInfo.stage = _vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos[0];

    auto r = d.createComputePipeline(nullptr, vkComputeCreateInfo);
    VK_CALL(r.result);

    m_Pipeline = r.value;
}


void VulkanShaderProgram::CreatePipeLinePointGraphicsPipeline(const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const GraphicPipelineData& Data = std::get<GraphicPipelineData>(m_PipelineData);

    vk::Device device = GET_VK_DEVICE;
    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStateArray.size());
    dynamicState.pDynamicStates = DynamicStateArray.data();

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
    ParseRasterizer(&rasterizer);

    vk::SampleCountFlagBits sample = Utils::RhSampleCountToVulkan(Data.Sample);
    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisampling.sampleShadingEnable = sample == vk::SampleCountFlagBits::e1 ? VK_FALSE : VK_TRUE;
    multisampling.rasterizationSamples = sample;
    multisampling.minSampleShading = sample == vk::SampleCountFlagBits::e1 ? 1.0f : 0.1f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments(
        Data.attachementCount);

    for (size_t i = 0; i < colorBlendAttachments.size(); ++i)
    {
        ParsePipelineColorAttachementBlendState(&colorBlendAttachments[i], &Data.BlendState);
    }

    vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
    ParsePipelineDepthStencilAttachmentState(&depthStencilState, Data.DephStencilInfo);

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
    colorBlending.pAttachments = colorBlendAttachments.data();

    ParseParsePipelineColorBlendState(&colorBlending, colorBlendAttachments.data(), colorBlendAttachments.size(),
                                      &Data.BlendState);


    CreatePipelineLayout(device, _vulkanShaderProgramCreateContex);

    vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{};
    graphicsPipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    graphicsPipelineInfo.stageCount = static_cast<uint32_t>(_vulkanShaderProgramCreateContex.
                                                            pipelineShaderStageCreateInfos.size());
    graphicsPipelineInfo.pStages = _vulkanShaderProgramCreateContex.pipelineShaderStageCreateInfos.data();
    graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
    graphicsPipelineInfo.pInputAssemblyState = &inputAssembly;
    graphicsPipelineInfo.pViewportState = &viewportState;
    graphicsPipelineInfo.pRasterizationState = &rasterizer;
    if (Data.DephStencilInfo.enableDepthTest || Data.DephStencilInfo.enableDepthWrite)
        graphicsPipelineInfo.pDepthStencilState = &depthStencilState;
    graphicsPipelineInfo.pMultisampleState = &multisampling;
    graphicsPipelineInfo.pColorBlendState = &colorBlending;
    graphicsPipelineInfo.pDynamicState = &dynamicState;
    graphicsPipelineInfo.layout = m_PipelineLayout;

    const VulkanRenderPass& VulkanRenderPassObject = *reinterpret_cast<VulkanRenderPass*>(Data.RenderPass);
    graphicsPipelineInfo.renderPass = VulkanRenderPassObject.GetVulkanRenderPass();
    graphicsPipelineInfo.subpass = Data.subPassIndex;


    auto result = device.createGraphicsPipeline(nullptr, graphicsPipelineInfo);

    vk::DebugUtilsObjectNameInfoEXT nameInfo{};
    nameInfo.objectType = vk::ObjectType::ePipeline;
    nameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkPipeline>(result.value));
    nameInfo.pObjectName = GetName().data();

    SET_VK_DEBUG_NAME(nameInfo);

    m_Pipeline = result.value;
}

void VulkanShaderProgram::CreatePipelineLayout(vk::Device _device,
                                               const VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex)
{
    auto cache = GET_VK_CONTEXT.descritptorManager.GetDescriptorSets(m_DescriptorId);

    assert(false);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(cache->descriptorSetLayout.size()); // Optional
    pipelineLayoutInfo.pSetLayouts = cache->descriptorSetLayout.data(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(_vulkanShaderProgramCreateContex.
                                                                      pushConstantRanges.size()); // Optional
    pipelineLayoutInfo.pPushConstantRanges = _vulkanShaderProgramCreateContex.pushConstantRanges.data(); // Optional

    m_PipelineLayout = _device.createPipelineLayout(pipelineLayoutInfo);
}

void VulkanShaderProgram::CreatePushConstantMapFromReflection(
    const std::vector<SpvReflectShaderModule>& _spvReflectShaderModule)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    for (size_t module = 0; module < _spvReflectShaderModule.size(); module++)
    {
        for (size_t pushConstant = 0; pushConstant < _spvReflectShaderModule[module].push_constant_block_count;
             pushConstant++)
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

void VulkanShaderProgram::ParseDescriptor(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContext)
{
    //m_DescriptorId = GET_VK_CONTEXT.descritptorManager.GetDescriptorId(
      //  _vulkanShaderProgramCreateContext.modulesReflected);
}


void VulkanShaderProgram::ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo)
{
    const GraphicPipelineData& PipelineData = std::get<GraphicPipelineData>(m_PipelineData);

    _pipelineRasterizationStateCreateInfo->depthClampEnable = VK_FALSE;
    _pipelineRasterizationStateCreateInfo->rasterizerDiscardEnable = VK_FALSE;
    _pipelineRasterizationStateCreateInfo->polygonMode = Utils::RhiPolygonModeToVulkan(PipelineData.PolygonMode);
    _pipelineRasterizationStateCreateInfo->lineWidth = 1.0f;
    _pipelineRasterizationStateCreateInfo->cullMode = Utils::RhiToCullMode(PipelineData.CullMode);

    _pipelineRasterizationStateCreateInfo->frontFace = PipelineData.FrontFace == RhiShaderProgram::FrontFace::Clockwise
        ? vk::FrontFace::eClockwise
        : vk::FrontFace::eCounterClockwise;

    _pipelineRasterizationStateCreateInfo->depthBiasEnable = VK_FALSE;
}

void VulkanShaderProgram::ParsePipelineColorAttachementBlendState(
    vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState, const RhiShaderProgram::BlendState* _blendInfo)
{
    _PipelineColorBlendAttachmentState->colorWriteMask =
        Utils::RhiColorComponent(_blendInfo->colorMask);
    _PipelineColorBlendAttachmentState->blendEnable = _blendInfo->enabled ? VK_TRUE : VK_FALSE;
    _PipelineColorBlendAttachmentState->srcColorBlendFactor = Utils::RhiBlendFactorToVulkan(
        _blendInfo->srcColorBlendFactor);
    _PipelineColorBlendAttachmentState->dstColorBlendFactor = Utils::RhiBlendFactorToVulkan(
        _blendInfo->dstColorBlendFactor);
    _PipelineColorBlendAttachmentState->colorBlendOp = Utils::RhiBlendOpToVulkan(_blendInfo->colorBlendOp);
    _PipelineColorBlendAttachmentState->srcAlphaBlendFactor = Utils::RhiBlendFactorToVulkan(
        _blendInfo->srcAlphaBlendFactor);
    _PipelineColorBlendAttachmentState->dstAlphaBlendFactor = Utils::RhiBlendFactorToVulkan(
        _blendInfo->dstAlphaBlendFactor);
    _PipelineColorBlendAttachmentState->alphaBlendOp = Utils::RhiBlendOpToVulkan(_blendInfo->alphaBlendOp);
}

void VulkanShaderProgram::ParsePipelineDepthStencilAttachmentState(
    vk::PipelineDepthStencilStateCreateInfo* _PipelineDepthStencilStateCreateInfo,
    const RhiShaderProgram::DephStencilInfo& _dephInfo)
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

void VulkanShaderProgram::ParseParsePipelineColorBlendState(
    vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo,
    const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState,
    size_t _PipelineColorBlendAttachmentSize, const RhiShaderProgram::BlendState* _blendInfo)
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


vk::VertexInputBindingDescription VulkanShaderProgram::ParseVertexInputBindingDescription(
    const PC_CORE::VertexInputBindingDescrition& _vertexInputBindingDescrition)
{
    vk::VertexInputBindingDescription vkvertexInputBindingDescription{};
    vkvertexInputBindingDescription.binding = _vertexInputBindingDescrition.Binding;
    vkvertexInputBindingDescription.stride = _vertexInputBindingDescrition.Stride;
    vkvertexInputBindingDescription.inputRate = Utils::RhiInputRateToVkInputRate(
        _vertexInputBindingDescrition.VertexInputRate);

    return vkvertexInputBindingDescription;
}

vk::VertexInputAttributeDescription VulkanShaderProgram::ParseVertexInputAttributeDescription(
    const PC_CORE::VertexAttributeDescription& _vertexAttributeDescription)
{
    vk::VertexInputAttributeDescription vkvertexInputAttributeDescription;
    vkvertexInputAttributeDescription.location = _vertexAttributeDescription.Location;
    vkvertexInputAttributeDescription.binding = _vertexAttributeDescription.Binding;
    vkvertexInputAttributeDescription.offset = _vertexAttributeDescription.Offset;
    vkvertexInputAttributeDescription.format = Utils::RhiFormatToVkFormat(_vertexAttributeDescription.Format);

    return vkvertexInputAttributeDescription;
}

vk::PipelineVertexInputStateCreateInfo VulkanShaderProgram::ParseVertexInputState(std::vector<vk::VertexInputBindingDescription>* _vertexInputBindingDescriptions
    , std::vector<vk::VertexInputAttributeDescription>* _vertexInputAttributeDescriptions)
{
    if (_vertexInputBindingDescriptions == nullptr || _vertexInputAttributeDescriptions == nullptr)
    {
        PC_LOGERROR("_vertexInputBindingDescriptions is null or _vertexInputAttributeDescriptions is null");
        return {};
    }
    const GraphicPipelineData& Data = std::get<GraphicPipelineData>(m_PipelineData);

    // Parse vertexInputBindingDescriptions
    _vertexInputBindingDescriptions->reserve(Data.vertexInputBindingDescritions.size());
    for (size_t i = 0; i < Data.vertexInputBindingDescritions.size(); i++)
    {
        _vertexInputBindingDescriptions->emplace_back(
            ParseVertexInputBindingDescription(Data.vertexInputBindingDescritions[i]));
    }

    // Parse verteixAttributes
    _vertexInputAttributeDescriptions->reserve(Data.vertexAttributeDescriptions.size());
    for (size_t i = 0; i < Data.vertexAttributeDescriptions.size(); i++)
    {
        _vertexInputAttributeDescriptions->emplace_back(
            ParseVertexInputAttributeDescription(Data.vertexAttributeDescriptions[i]));
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

void VulkanShaderProgram::ParsePushConstantRange(VulkanShaderProgramCreateContex& _vulkanShaderProgramCreateContex)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::vector<vk::PushConstantRange>* pushConstantRanges = &_vulkanShaderProgramCreateContex.pushConstantRanges;

    uint32_t pushConstantRangeCount = 0;
    for (auto& moduleIndex : _vulkanShaderProgramCreateContex.modulesReflected)
    {
        pushConstantRangeCount += moduleIndex.push_constant_block_count;
    }
    pushConstantRanges->resize(pushConstantRangeCount);

    pushConstantRangeCount = 0;

    // PushRange
    for (size_t i = 0; i < _vulkanShaderProgramCreateContex.modulesReflected.size(); i++)
    {
        const uint32_t pushConstantCount = _vulkanShaderProgramCreateContex.modulesReflected[i].
            push_constant_block_count;
        if (pushConstantCount == 0)
            continue;

        for (size_t j = 0; j < pushConstantCount; j++)
        {
            SpvReflectBlockVariable* spvReflectBlockVariablePushConstant = _vulkanShaderProgramCreateContex.
                modulesReflected[j].push_constant_blocks;
            if (!spvReflectBlockVariablePushConstant)
                continue;

            vk::PushConstantRange& pushConstantRange = pushConstantRanges->at(pushConstantRangeCount);
            pushConstantRange.offset = spvReflectBlockVariablePushConstant->offset;
            pushConstantRange.size = spvReflectBlockVariablePushConstant->size;
            pushConstantRange.stageFlags = static_cast<vk::ShaderStageFlags>(_vulkanShaderProgramCreateContex.
                modulesReflected[i].shader_stage);
            pushConstantRangeCount++;
        }
    }
}

void VulkanShaderProgram::HotReload(const std::vector<RhiShaderProgram::ShaderModule>& _modules)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    PC_LOG("HotReload {}", GetName().data());

    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(GET_VK_CONTEXT.rhiDevice)->GetDevice();


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
    m_PushConstantMap.clear();

    VulkanShaderProgramCreateContex vulkanShaderProgramCreateContex = CreateShaderProgramCreateContext(_modules, false);
    if (!CreateFromContext(vulkanShaderProgramCreateContex))
    {
        PC_LOGERROR("Failed to hot reaload {}", GetName());
    }
}

#pragma endregion ParseRegion
