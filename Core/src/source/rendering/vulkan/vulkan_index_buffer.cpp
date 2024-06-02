#include "rendering/vulkan/vulkan_index_buffer.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanIndexBuffer::Init(const std::vector<uint32_t>& _indicies)
{
    CreateBuffer(&m_Buffer , &m_Allocation,VK_BUFFER_USAGE_INDEX_BUFFER_BIT,VMA_MEMORY_USAGE_CPU_TO_GPU,
        _indicies.data(), sizeof(_indicies[0]) * _indicies.size());
}
