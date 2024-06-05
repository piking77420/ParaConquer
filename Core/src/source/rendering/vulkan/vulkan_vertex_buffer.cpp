#include "rendering\vulkan\vulkan_vertex_buffer.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

void VulkanVertexBuffer::Init(const std::vector<Vertex>& vertices)
{
	CreateGpuBuffer(&m_Buffer, &m_Allocation, vertices.data(), sizeof(vertices[0]) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}
