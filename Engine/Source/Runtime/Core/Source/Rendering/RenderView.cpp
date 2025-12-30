#include "Rendering/RenderView.hpp"

#include "Rendering/Camera.hpp"

namespace PC_CORE::Rendering  
{
	void PC_CORE::Rendering::RenderView::FromCamera(const PC_CORE::Camera& _Camera, float _Time, float _Deltatime)
	{
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rendering);

        View = _Camera.GetViewMatrix();
        ViewInv = View.Invert();
        Projection = _Camera.GetProjection();
        ProjectionInv = Projection.Invert();
        ViewProjection = _Camera.GetViewProjection();
        ViewProjectionInv = ViewInv * ProjectionInv;

        CameraNear = _Camera.GetNear();
        CameraFar = _Camera.GetNear();
        Time = _Time;
        Deltatime = _Deltatime;
	}

} // PC_CORE::Rendering

