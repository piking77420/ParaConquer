#include "rendering/vulkan/vulkan_pipeline.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_shader_stage.hpp"

using namespace PC_CORE;

void VulkanPipeline::Init(const VkGraphicsPipelineCreateInfo* _vkGraphicsPipelineCreateInfo,
                          const VulkanShaderStage& _vulkanShaderStage, const VkPipelineLayout& _pipelineLayout
                          , const VkRenderPass _renderPass)
{
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo = *_vkGraphicsPipelineCreateInfo;


    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;


    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;


    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    if(vkGraphicsPipelineCreateInfo.pInputAssemblyState == VK_NULL_HANDLE)
        vkGraphicsPipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    
    if (vkGraphicsPipelineCreateInfo.pViewportState == VK_NULL_HANDLE)
        vkGraphicsPipelineCreateInfo.pViewportState = &viewportState;

    if (vkGraphicsPipelineCreateInfo.pRasterizationState == VK_NULL_HANDLE)
        vkGraphicsPipelineCreateInfo.pRasterizationState = &rasterizer;

    if (vkGraphicsPipelineCreateInfo.pMultisampleState == VK_NULL_HANDLE)
        vkGraphicsPipelineCreateInfo.pMultisampleState = &multisampling;

    if (vkGraphicsPipelineCreateInfo.pColorBlendState == VK_NULL_HANDLE)
        vkGraphicsPipelineCreateInfo.pColorBlendState = &colorBlending;

    if (vkGraphicsPipelineCreateInfo.pVertexInputState == VK_NULL_HANDLE)
        vkGraphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;

    
    constexpr std::array<VkDynamicState, 2> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    vkGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    vkGraphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(_vulkanShaderStage.vkPipelineShaderStageCreateInfo.
        size());
    vkGraphicsPipelineCreateInfo.pStages = _vulkanShaderStage.vkPipelineShaderStageCreateInfo.data();
    vkGraphicsPipelineCreateInfo.pDynamicState = &dynamicState;
    vkGraphicsPipelineCreateInfo.layout = _pipelineLayout;
    vkGraphicsPipelineCreateInfo.renderPass = _renderPass;
    

    VkResult res = vkCreateGraphicsPipelines(VulkanInterface::GetDevice().device, VK_NULL_HANDLE, 1,
                                             &vkGraphicsPipelineCreateInfo, nullptr, &m_Pipeline);
    VK_CHECK_ERROR(res, "Failed To create GraphicPipeline")
}

void VulkanPipeline::Destroy()
{
    vkDestroyPipeline(VulkanInterface::GetDevice().device, m_Pipeline, nullptr);
}

VkPipeline& VulkanPipeline::Get()
{
    return m_Pipeline;
}

const VkPipeline VulkanPipeline::Get() const
{
    return m_Pipeline;
}

