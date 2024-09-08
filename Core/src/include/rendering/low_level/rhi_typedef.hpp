#pragma once
#include "core_header.hpp"

BEGIN_PCCORE

enum GRAPHICSAPI
{
	NONE,
	VULKAN,
	OPENGL,

	COUNT
};


struct RHICreateInfo
{
	GRAPHICSAPI graphicApi;
};


END_PCCORE