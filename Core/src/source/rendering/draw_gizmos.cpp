#include "rendering/draw_gizmos.hpp"

#include "math/matrix_transformation.hpp"
#include "math/toolbox_typedef.hpp"
#include "physics/sphere_collider.hpp"
#include "rendering/renderer.hpp"
#include "rendering/vertex.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "resources/resource_manager.hpp"
#include "world/scene.hpp"

namespace PC_CORE
{
    class ShaderSource;
}

using namespace PC_CORE;

void DrawGizmos::Init(PC_CORE::Renderer* renderer)
{
    m_Renderer = renderer;
    InitShader();
    InitDescriptor();
    CreateGraphiPipeline();
    sphere = ResourceManager::Get<Mesh>("sphere.obj");
}

void DrawGizmos::DrawGizmosForward(VkCommandBuffer _commandBuffer, uint32_t _imageIndex , const ViewPort& viewPort)
{
    if (!viewPort.isEditor)
        return;
        
    currentCommandBuffer = &_commandBuffer;
    imageIndex = _imageIndex;
    vkCmdBindPipeline(*currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GizmoPipeline.Get());
    DrawColliders();
}

void DrawGizmos::Destroy()
{
    m_DescriptorPool.Destroy();
    m_VulkanShaderStageGizmo.Destroy();
    m_GizmoPipeline.Destroy();
    m_VkPipelineLayout.Destroy();
    m_DescriptorSetLayout.Destroy();
}

void DrawGizmos::InitShader()
{
    const ShaderSource* vertex = ResourceManager::Get<ShaderSource>("gizmo.vert");
    const ShaderSource* frag = ResourceManager::Get<ShaderSource>("gizmo.frag");
    m_VulkanShaderStageGizmo.Init({ vertex, frag });
}

void DrawGizmos::CreateGraphiPipeline()
{
    VkPushConstantRange pushConstantMatrix;
    pushConstantMatrix.offset = 0;
    pushConstantMatrix.size = sizeof(GizmoStruct);
    pushConstantMatrix.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    m_VkPipelineLayout.Init({m_DescriptorSetLayout.Get()}, {pushConstantMatrix});
    /////////////////////////
    
    
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;


    VkPipelineDepthStencilStateCreateInfo depthStencil {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    
    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::GetAttributeDescriptions();
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pRasterizationState = &rasterizer;

    m_GizmoPipeline.Init(&pipelineInfo, m_VulkanShaderStageGizmo, m_VkPipelineLayout.Get(),
                         m_Renderer->renderPasses.at(FORWARD).renderPass);
    
}

void DrawGizmos::DrawColliders()
{
    const Scene& scene = m_Renderer->m_CurrentWorld->scene;
    const std::vector<SphereCollider>* sphereColliders = nullptr;
    scene.GetComponentData<SphereCollider>(&sphereColliders);
    DrawSphereCollider(*sphereColliders,scene);
    
}

void DrawGizmos::InitDescriptor()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = VulkanUniformBuffer::GetLayoutBinding(0,1 ,
    VK_SHADER_STAGE_VERTEX_BIT);
    m_DescriptorSetLayout.Init({uboLayoutBinding});
    m_DescriptorPool.Init(m_DescriptorSetLayout.GetLayoutBinding(), VulkanInterface::GetNbrOfImage());
    
    m_DescriptorSet.resize(VulkanInterface::GetNbrOfImage());
    m_DescriptorPool.CreateDescriptorSet(m_DescriptorSetLayout.Get(), static_cast<uint32_t>(m_DescriptorSet.size()),m_DescriptorSet.data());

    for (size_t i = 0; i < m_DescriptorSet.size(); i++)
    {
       
        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_Renderer->m_CameraBuffers[i].GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(m_Renderer->cameraBuffer);
        
        m_Renderer->m_CameraBuffers[i].Bind(&descriptorWrites[0], m_DescriptorSet[i],
            0, 0, 1,
            bufferInfo);
        
        
        vkUpdateDescriptorSets(VulkanInterface::GetDevice().device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void DrawGizmos::DrawSphereCollider(const std::vector<SphereCollider>& sphereColliders, const Scene& scene)
{
    const VkBuffer vertexBuffers[] = { sphere->vulkanVertexBuffer.GetHandle()};
    const VkDeviceSize offsets[] = {0};
        
    vkCmdBindVertexBuffers(*currentCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(*currentCommandBuffer, sphere->vulkanIndexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(*currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipelineLayout.Get(), 0,
               1, &m_DescriptorSet[VulkanInterface::GetCurrentFrame()], 0, nullptr);
    
    for (size_t i = 0; i < sphereColliders.size(); i++)
    {
        const SphereCollider& sphereCollider = sphereColliders.at(i);
        
        if (!IsValid(sphereCollider.componentHolder))
            continue;

        const Transform* transform = scene.GetComponent<Transform>(sphereCollider.componentHolder.entityID);
        if (!transform)
            continue;
        
        Tbx::Vector3f color = {0.f,1.f,0.f};
        Tbx::Matrix4x4f trs;
        Trs3D<float>(transform->localPosition, Tbx::Quaternionf::Identity(), sphereCollider.radius * 0.5f, &trs);

        const GizmoStruct gizmoStruct =
            {
            trs,
            color
            };
        
        vkCmdPushConstants(*currentCommandBuffer, m_VkPipelineLayout.Get(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0, sizeof(GizmoStruct), &gizmoStruct);
        vkCmdDrawIndexed(*currentCommandBuffer, sphere->indicies.size(), 1, 0, 0, 0);
    }
}
