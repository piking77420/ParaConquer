#pragma once

#include "vulkan_header.h"
#include "resources/shader_source.hpp"


BEGIN_PCCORE

class VulkanShaderProgram
{
public:
	void Create(const VkDevice& device, const std::vector<ShaderSource&>& _shaderSource);

	

private:
	VkPipeline pipline;
	std::vector<ShaderSource::ShaderType> m_ShaderPipelineState;
	std::vector<VkShaderModule> shaderModule;
};

END_PCCORE