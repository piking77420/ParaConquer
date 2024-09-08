#pragma once

#include "rhi_interface.hpp"s
#include <front_end/vulkan_app.hpp>

BEGIN_PCCORE

PC_CORE_API class VulkanInterface : public RHIInterface
{
public:
	VulkanInterface(const RHICreateInfo& _rHICreateInfo);

	~VulkanInterface() override = default;

private:
	VK_NP::VulkanApp m_VulkanApp;

};

END_PCCORE