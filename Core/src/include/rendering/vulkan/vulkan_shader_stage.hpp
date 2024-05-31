#pragma once

#include "vulkan_header.h"
#include "resources/shader_source.hpp"


BEGIN_PCCORE

class VulkanShaderStage
{	
public:
	std::vector<VkPipelineShaderStageCreateInfo> vkPipelineShaderStageCreateInfo;
	
	void Init(const std::vector<ShaderSource*>& _shaderSource);

	void Destroy(const VkDevice& _device);

private:
	struct ShaderWithModule
	{
		const ShaderSource* shaderSource = nullptr;
		VkShaderModule vkShaderModule = VK_NULL_HANDLE;
	};
	
	std::vector<ShaderWithModule> m_ShaderModules;
	
	VkShaderStageFlagBits ShaderTypeTOVukan(ShaderSource::ShaderType shaderType);

	void Compile(const VkDevice& _device);
	
};

END_PCCORE