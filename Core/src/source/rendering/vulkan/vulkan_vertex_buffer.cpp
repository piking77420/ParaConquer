#include "rendering\vulkan\vulkan_vertex_buffer.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanVertexBuffer::Init(const std::vector<Vertex>& vertices)
{
	CreateBuffer(&m_Buffer , &m_Allocation,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_TO_GPU,
		vertices.data(), sizeof(vertices[0]) * vertices.size());
}
