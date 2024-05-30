#include "rendering/vulkan/vulkan_shader_program.hpp"

using namespace PC_CORE;

void VulkanShaderProgram::Create(const std::vector<ShaderSource&>& _shaderSource)
{

	shaderModule.resize(_shaderSource.size());

	for(ShaderSource& shader : _shaderSource)
	{
		std::vector<uint8_t>* shaderData = &shader.data;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderData->size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderData->data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
	}
}
