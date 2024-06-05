#include "rendering/vulkan/vulkan_index_buffer.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

void VulkanIndexBuffer::Init(const std::vector<uint32_t>& _indicies)
{
    CreateGpuBuffer(&m_Buffer, &m_Allocation, _indicies.data(), sizeof(_indicies[0]) * _indicies.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}
