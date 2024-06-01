#include "rendering/vulkan/vertex_buffer.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VertexBuffer::Init(const std::vector<Vertex>& vertices)
{
    const VkDevice& device = VulkanInterface::GetDevice().device;
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(VulkanInterface::GetDevice().device, &bufferInfo, nullptr, &vertexBuffer);
    VK_CHECK_ERROR(result,"Failed To create vertex buffer")

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanInterface::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    result = vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory);
    VK_CHECK_ERROR(result,"failed to allocate vertex buffer memory!")
    
    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    std::memcpy(data, vertices.data(), (size_t) bufferInfo.size);
    vkUnmapMemory(device, vertexBufferMemory);
}

void VertexBuffer::Destroy()
{
    vkDestroyBuffer(VulkanInterface::GetDevice().device, vertexBuffer, nullptr);
}
