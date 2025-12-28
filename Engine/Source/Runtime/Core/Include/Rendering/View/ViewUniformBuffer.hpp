#pragma once

#include "Rendering/RenderingTypedef.h"

namespace PC_CORE::Rendering::Gpu
{
	struct ViewUniformBuffer
	{
        mat4 View;
        mat4 ViewInv;

        mat4 Projection;
        mat4 ProjectionInv;

        mat4 ViewProjection;
        mat4 ViewProjectionInv;

        float CameraNear;
        float CameraFar;

        float Time;
        float Deltatime; 
	};


} // PC_CORE