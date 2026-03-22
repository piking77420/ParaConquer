#include <array>

#include "Rendering/RenderPasses/Renderpass.hpp"
#include "Resources/ResourceManager.hpp"

PC_CORE::Rendering::RenderPass::RenderPass()
{
	DYNAMIC_REFLECT_INIT
}

size_t PC_CORE::Rendering::RenderPass::PickLodCount(const std::vector<double>& LodThreshold, const MotionCore::Aabb<double>& AABBW, const Tbx::Vector3d& CameraPos, double FovRad) const
{
	if (LodThreshold.empty())
		return 0;

	double BoundingSphereRadius = (AABBW.GetSize() * 0.5).Magnitude();
	double DistanceToCamera = (AABBW.GetCenter() - CameraPos).Magnitude();
	double ScreenSize = BoundingSphereRadius / (DistanceToCamera * std::tan(FovRad * 0.5));

	for (size_t i = 0; i < LodThreshold.size(); i++)
	{
		if (ScreenSize >= LodThreshold[i])
		{
			return i;
		}
	}
	return LodThreshold.size();
}
