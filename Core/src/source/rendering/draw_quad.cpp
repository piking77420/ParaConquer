#include "rendering/draw_quad.hpp"

#include "rendering/vulkan/vulkan_interface.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

void DrawQuad::Init(Renderer* renderer)
{
    m_Renderer = renderer;
    quad = ResourceManager::Get<Mesh>("quad.obj");
    InitShader();
    InitDescriptor();
    CreateGraphiPipeline();
}

void DrawQuad::Draw(VkCommandBuffer _commandBuffer, VkDescriptorSet _descriptorSet)
{
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline.Get());
    
    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipelineLayout.Get(), 0,
                        1, &_descriptorSet, 0, nullptr);
    
    const VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(_commandBuffer, 0, 1, &quad->vulkanVertexBuffer.GetHandle(), offsets);
    vkCmdBindIndexBuffer(_commandBuffer, quad->vulkanIndexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(_commandBuffer, quad->indicies.size(), 1, 0, 0, 0);
}

void DrawQuad::Destroy()
{
    m_VulkanShaderStage.Destroy();
    m_Pipeline.Destroy();
    m_VkPipelineLayout.Destroy();
    m_DescriptorSetLayout.Destroy();
    m_DescriptorPool.Destroy();
}

VkDescriptorSet DrawQuad::DrawQuadAddTexture(const VulkanTexture& _vulkanTexture)
{
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    m_DescriptorPool.CreateDescriptorSet(m_DescriptorSetLayout.Get(), 1, &descriptorSet);
    
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = _vulkanTexture.textureImageView;
    imageInfo.sampler = VulkanInterface::vulkanTextureSampler.defaultSampler.textureSampler;
    
    VkWriteDescriptorSet vkWriteDescriptorSet;
    vkWriteDescriptorSet.pNext = nullptr;
    vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    vkWriteDescriptorSet.dstSet = descriptorSet;
    vkWriteDescriptorSet.dstBinding = 0;
    vkWriteDescriptorSet.dstArrayElement = 0;   
    vkWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkWriteDescriptorSet.descriptorCount = 1;
    vkWriteDescriptorSet.pImageInfo = &imageInfo;
    
       
    vkUpdateDescriptorSets(VulkanInterface::GetDevice().device,
        1, &vkWriteDescriptorSet, 0, nullptr);

    return descriptorSet;
}

void DrawQuad::InitShader()
{
    const ShaderSource* vertex = ResourceManager::Get<ShaderSource>("draw_texture.vert");
    const ShaderSource* frag = ResourceManager::Get<ShaderSource>("draw_texture.frag");
    m_VulkanShaderStage.Init({ vertex, frag });
}

void DrawQuad::CreateGraphiPipeline()
{
    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts =
{
        m_DescriptorSetLayout.Get(),
    };
    m_VkPipelineLayout.Init(vkDescriptorSetLayouts);
    
    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::GetAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    
    VkPipelineDepthStencilStateCreateInfo depthStencil {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.pNext = nullptr;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    // TODO REMOVE IT
    pipelineInfo.pDepthStencilState = &depthStencil;

    m_Pipeline.Init(&pipelineInfo, m_VulkanShaderStage, m_VkPipelineLayout.Get(),
                        VulkanInterface::vulkanSwapChapchain.mainRenderPass.renderPass);
}

void DrawQuad::InitDescriptor()
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding = VulkanTextureSampler::GetDescriptorSetLayoutBinding(0,1,VK_SHADER_STAGE_FRAGMENT_BIT);
    m_DescriptorSetLayout.Init({samplerLayoutBinding});
    m_DescriptorPool.Init(m_DescriptorSetLayout.GetLayoutBinding(), MaxDrawQuadAlloc);
    
}
