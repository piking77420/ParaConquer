#include "RenderingTypedef.h"

namespace PC_CORE::Rendering 
{
	struct RenderSettings
	{
		PC_CORE::SampleCount MSAASampleCount = SampleCount::S1;

		REFLECT(RenderSettings)
		REFLECT_MEMBER(RenderSettings, MSAASampleCount);
	};

	
}