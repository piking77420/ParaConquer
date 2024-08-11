#include "skybox_render.h"

#include "rendering/renderer.hpp"
#include "rendering/vertex.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

void SkyboxRender::Init(Renderer* _renderer)
{
    m_Renderer = _renderer;
    InitCubeMapBackend();
    InitProceduralBackend();
}

void SkyboxRender::DrawSkybox(VkCommandBuffer commandBuffer, const Skybox& skybox)
{
    switch (skybox.skyboxType)
    {
    case Skybox::SkyBoxType::CubeMap:
        DrawSkyboxCubemap(commandBuffer);
        break;
    case Skybox::SkyBoxType::Procedural:
        DrawSkyboxProcedural(commandBuffer);
        break;
    default: ;
    }
}

void SkyboxRender::CreateSkyboxBackend(const Skybox& skybox)
{
    switch (skybox.skyboxType)
    {
    case Skybox::SkyBoxType::CubeMap:
        CreateCubemapDescriptorSetBackend(skybox);
        break;
    case Skybox::SkyBoxType::Procedural:
        CreateProcuralDescriptorSetBackend(skybox);
        break;
    default: ;
    }
}

void SkyboxRender::Destroy()
{
    DestroyCubeMapBackend();
    DestroyProceduralSkyboxBackend();
}


void SkyboxRender::InitCubeMapBackend()
{
    m_SkyBoxCubMapBackend.cubeMesh = ResourceManager::Get<Mesh>("cube_map.obj");;

    // Shader
    ShaderSource* shaderVert = ResourceManager::Get<ShaderSource>("skybox.vert");
    ShaderSource* shaderFrag = ResourceManager::Get<ShaderSource>("skybox.frag");
    m_SkyBoxCubMapBackend.vulkanShaderStageSkyBox.Init({shaderVert, shaderFrag});

    // PipelineLayout and commandPool
    const uint32_t nbrOfFrames = VulkanInterface::GetNbrOfImage();
    VkDescriptorSetLayoutBinding uboLayoutBinding = VulkanUniformBuffer::GetLayoutBinding(0, 1,
        VK_SHADER_STAGE_VERTEX_BIT);
    VkDescriptorSetLayoutBinding skyboxBinding = VulkanTextureSampler::GetDescriptorSetLayoutBinding(1, 1,
        VK_SHADER_STAGE_FRAGMENT_BIT);
    const std::vector<VkDescriptorSetLayoutBinding> layout =
    {
        uboLayoutBinding,
        skyboxBinding,
    };
    m_SkyBoxCubMapBackend.descriptorSetLayout.Init(layout);
    // TODO maybe change mor multple cube map
    m_SkyBoxCubMapBackend.descriptorPool.Init({m_SkyBoxCubMapBackend.descriptorSetLayout.GetLayoutBinding()},
                                              nbrOfFrames);

    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts =
    {
        m_SkyBoxCubMapBackend.descriptorSetLayout.Get(),
    };
    m_SkyBoxCubMapBackend.vkPipelineLayout.Init(vkDescriptorSetLayouts);

    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::GetAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.pNext = nullptr;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pDepthStencilState = &depthStencil;

    m_SkyBoxCubMapBackend.skyBoxPipeline.Init(&pipelineInfo, m_SkyBoxCubMapBackend.vulkanShaderStageSkyBox,
                                              m_SkyBoxCubMapBackend.vkPipelineLayout.Get(),
                                              m_Renderer->renderPasses.at(FORWARD).renderPass);


    // Init Descreiptors and descriptor Pool
    m_SkyBoxCubMapBackend.descriptorSetMap.resize(VulkanInterface::GetNbrOfImage());
    m_SkyBoxCubMapBackend.descriptorPool.CreateDescriptorSet(m_SkyBoxCubMapBackend.descriptorSetLayout.Get(),
                                                             m_SkyBoxCubMapBackend.descriptorSetMap.size(),
                                                             m_SkyBoxCubMapBackend.descriptorSetMap.data());
}


void SkyboxRender::DrawSkyboxCubemap(VkCommandBuffer commandBuffer)
{
    const VkBuffer vertexBuffers[] = {m_SkyBoxCubMapBackend.cubeMesh->vulkanVertexBuffer.GetHandle()};
    const VkDeviceSize offsets[] = {0};
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_SkyBoxCubMapBackend.skyBoxPipeline.Get());

    
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_SkyBoxCubMapBackend.vkPipelineLayout.Get(), 0,
                            1, &m_SkyBoxCubMapBackend.descriptorSetMap[VulkanInterface::GetCurrentFrame()], 0, nullptr);
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_SkyBoxCubMapBackend.cubeMesh->vulkanIndexBuffer.GetHandle(), 0,
                         VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_SkyBoxCubMapBackend.cubeMesh->indicies.size()), 1, 0, 0, 0);
}

void SkyboxRender::CreateProcuralDescriptorSetBackend(const Skybox& skybox)
{
    const std::vector<VulkanUniformBuffer>& cameraUnBuffer = m_Renderer->m_CameraBuffers;

    for (size_t i = 0; i < skyBoxProceduralBackend.descriptorSet.size(); i++)
    {
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = cameraUnBuffer[i].GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(CameraBuffer);

        cameraUnBuffer[i].Bind(&descriptorWrites[0], skyBoxProceduralBackend.descriptorSet[i],
                               0, 0, 1,
                               bufferInfo);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = skybox.texture->vulkanTexture.textureImageView;
        imageInfo.sampler = VulkanInterface::vulkanTextureSampler.defaultSampler.textureSampler;
    
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = skyBoxProceduralBackend.descriptorSet[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;
        
        vkUpdateDescriptorSets(VulkanInterface::GetDevice().device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}
void SkyboxRender::CreateCubemapDescriptorSetBackend(const Skybox& skybox)
{
    
    const std::vector<VulkanUniformBuffer>& cameraUnBuffer = m_Renderer->m_CameraBuffers;

    for (size_t i = 0; i < m_SkyBoxCubMapBackend.descriptorSetMap.size(); i++)
    {
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = cameraUnBuffer[i].GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(CameraBuffer);

        cameraUnBuffer[i].Bind(&descriptorWrites[0], m_SkyBoxCubMapBackend.descriptorSetMap[i],
                               0, 0, 1,
                               bufferInfo);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = skybox.texture->vulkanTexture.textureImageView;
        imageInfo.sampler = VulkanInterface::vulkanTextureSampler.defaultSampler.textureSampler;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = m_SkyBoxCubMapBackend.descriptorSetMap[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(VulkanInterface::GetDevice().device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}

void SkyboxRender::InitProceduralBackend()
{
    skyBoxProceduralBackend.skydom.Generate(40,40,1);
    
    skyBoxProceduralBackend.quad = ResourceManager::Get<Mesh>("quad.obj");
    // Shader
    ShaderSource* shaderVert = ResourceManager::Get<ShaderSource>("skybox_procedural.vert");
    ShaderSource* shaderFrag = ResourceManager::Get<ShaderSource>("skybox_procedural.frag");
    skyBoxProceduralBackend.vulkanShaderStageSkyBox.Init({shaderVert, shaderFrag});

    // PipelineLayout and commandPool
    const uint32_t nbrOfFrames = VulkanInterface::GetNbrOfImage();
    
    VkDescriptorSetLayoutBinding uboLayoutBinding = VulkanUniformBuffer::GetLayoutBinding(0, 1,
        VK_SHADER_STAGE_VERTEX_BIT);
    VkDescriptorSetLayoutBinding samplerLayoutBinding = VulkanTextureSampler::GetDescriptorSetLayoutBinding(1,1,VK_SHADER_STAGE_FRAGMENT_BIT);

    
    const std::vector<VkDescriptorSetLayoutBinding> layout =
    {
        uboLayoutBinding,samplerLayoutBinding
    };

    skyBoxProceduralBackend.descriptorSetLayout.Init({layout});
    skyBoxProceduralBackend.descriptorPool.Init({skyBoxProceduralBackend.descriptorSetLayout.GetLayoutBinding()},
                                              nbrOfFrames);
    VkPushConstantRange pushConstantMatrix;
    pushConstantMatrix.offset = 0;
    pushConstantMatrix.size = sizeof(Tbx::Matrix4x4f);
    pushConstantMatrix.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts =
    {
        skyBoxProceduralBackend.descriptorSetLayout.Get(),
    };
    skyBoxProceduralBackend.vkPipelineLayout.Init({vkDescriptorSetLayouts} , {pushConstantMatrix});

    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::GetAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.pNext = nullptr;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    skyBoxProceduralBackend.skyBoxPipeline.Init(&pipelineInfo, skyBoxProceduralBackend.vulkanShaderStageSkyBox,
                                              skyBoxProceduralBackend.vkPipelineLayout.Get(),
                                              m_Renderer->renderPasses.at(FORWARD).renderPass);


    // Init Descreiptors and descriptor Pool
    skyBoxProceduralBackend.descriptorSet.resize(VulkanInterface::GetNbrOfImage());
    skyBoxProceduralBackend.descriptorPool.CreateDescriptorSet(skyBoxProceduralBackend.descriptorSetLayout.Get(),
                                                               skyBoxProceduralBackend.descriptorSet.size(),
                                                               skyBoxProceduralBackend.descriptorSet.data());
}

void SkyboxRender::DestroyProceduralSkyboxBackend()
{
    skyBoxProceduralBackend.vulkanShaderStageSkyBox.Destroy();
    skyBoxProceduralBackend.skyBoxPipeline.Destroy();
    skyBoxProceduralBackend.vkPipelineLayout.Destroy();
    skyBoxProceduralBackend.descriptorSetLayout.Destroy();
    skyBoxProceduralBackend.descriptorPool.Destroy();
    skyBoxProceduralBackend.descriptorSet.clear();
    skyBoxProceduralBackend.skydom.Destroy();
}

void SkyboxRender::DrawSkyboxProcedural(VkCommandBuffer commandBuffer)
{
    const VkBuffer vertexBuffers[] = {skyBoxProceduralBackend.skydom.m_VulkanVertexBuffer.GetHandle()};
    const VkDeviceSize offsets[] = {0};
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxProceduralBackend.skyBoxPipeline.Get());

    VkDescriptorSet descriptorSet = skyBoxProceduralBackend.descriptorSet[VulkanInterface::GetCurrentFrame()];
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                         skyBoxProceduralBackend.vkPipelineLayout.Get(), 0,
                         1, &descriptorSet, 0, nullptr);
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdDraw(commandBuffer, skyBoxProceduralBackend.skydom.nbrOfVerticies, 1,0, 0);
}



void SkyboxRender::DestroyCubeMapBackend()
{
    m_SkyBoxCubMapBackend.vulkanShaderStageSkyBox.Destroy();
    m_SkyBoxCubMapBackend.skyBoxPipeline.Destroy();
    m_SkyBoxCubMapBackend.vkPipelineLayout.Destroy();
    m_SkyBoxCubMapBackend.descriptorSetLayout.Destroy();
    m_SkyBoxCubMapBackend.descriptorPool.Destroy();
    m_SkyBoxCubMapBackend.descriptorSetMap.clear();
}
