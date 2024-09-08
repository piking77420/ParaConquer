#include "rendering/low_level/rhi.hpp"

using namespace PC_CORE;

void RHI::Init(const RHICreateInfo& _RHICreateInfo)
{
	switch (_RHICreateInfo.graphicApi)
	{
	case NONE : 
		break;
	case VULKAN:
		graphicInterface = new VulkanInterface(_RHICreateInfo);
		break;
	case OPENGL:
		break;
	default:
		break;
	}
}

void RHI::Destroy()
{
	delete graphicInterface;
}
