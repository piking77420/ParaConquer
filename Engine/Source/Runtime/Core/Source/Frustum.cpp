#include <Frustum.hpp>
#include <Log.hpp>
#include <PerfRegion.hpp>

namespace PC_CORE
{
	Frustum::Frustum(const std::array<Tbx::Vector4d, 8>& NdcCorner, 
		const Tbx::Matrix4x4d& FrustumToWorld)
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Core);

		// TODO 
		// Compute Plane

		std::array<Tbx::Vector4d, 8> CornerWorld;
		for (size_t i = 0; i < NdcCorner.size(); i++)
		{
			CornerWorld[i] = FrustumToWorld * NdcCorner[i];
			CornerWorld[i] /= CornerWorld[i].w;
		} 
		
		// Normals are facing outside
		m_Planes[Planes::Near] = Plane(CornerWorld[Corner::NearBottomLeft], CornerWorld[Corner::NearTopLeft], CornerWorld[Corner::NearBottomRight]);
		m_Planes[Planes::Far] = Plane(CornerWorld[Corner::FarBottomLeft], CornerWorld[Corner::FarBottomRight], CornerWorld[Corner::FarTopLeft]);

		m_Planes[Planes::Left] = Plane(CornerWorld[Corner::NearBottomLeft], CornerWorld[Corner::FarBottomLeft], CornerWorld[Corner::FarTopLeft]);
		m_Planes[Planes::Right] = Plane(CornerWorld[Corner::NearBottomRight], CornerWorld[Corner::FarTopRight], CornerWorld[Corner::FarBottomRight]);

		m_Planes[Planes::Top] = Plane(CornerWorld[Corner::NearTopLeft], CornerWorld[Corner::FarTopLeft], CornerWorld[Corner::FarTopRight]);
		m_Planes[Planes::Bottom] = Plane(CornerWorld[Corner::NearBottomLeft], CornerWorld[Corner::FarBottomRight], CornerWorld[Corner::FarBottomLeft]);
	}

	bool Frustum::IsOnFrustum(const Tbx::Vector3d& _Center, const Tbx::Vector3d& _Extend) const
	{
		for (const Plane& plane : m_Planes)
		{
			if (!plane.IsInsideOrIntersects(_Center, _Extend))
				return false;
		}

		return true;
	}

	void Frustum::StreamPlanes(float* _Planes) const
	{
		for (size_t i = 0; i < m_Planes.size(); ++i)
		{
			Tbx::Vector4f* planeVec4 = reinterpret_cast<Tbx::Vector4f*>(&_Planes[i * 4]);
			*planeVec4 = Tbx::Vector4f(static_cast<float>(m_Planes[i].Normal.x), static_cast<float>(m_Planes[i].Normal.y), static_cast<float>(m_Planes[i].Normal.z), static_cast<float>(m_Planes[i].Distance));
		}
	}

	Frustum::Plane::Plane(const Tbx::Vector4d& _P1, const Tbx::Vector4d& _P2, const Tbx::Vector4d& _P3)
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Core);

		const Tbx::Vector4d v1 = (_P2 - _P1);
		const Tbx::Vector4d v2 = (_P3 - _P1);
		Normal = Tbx::Vector3d::Cross(Tbx::Vector3d(v1.x, v1.y, v1.z), Tbx::Vector3d(v2.x, v2.y, v2.z)).Normalize();
		if (Normal.Magnitude() <= std::numeric_limits<double>::epsilon())
		{
			PC_LOGERROR("Plane points are colinear or too close \n p1 = {}, p2 = {}, p3 = {}", _P1, _P2, _P3);
		}

		Distance = -Tbx::Vector3d::Dot(Normal, Tbx::Vector3d(_P1.x, _P1.y, _P1.z));
	}

	double Frustum::Plane::SignedDistanceToPlane(const Tbx::Vector3d& _Point) const
	{
		return (Tbx::Vector3d::Dot(_Point, Normal) + Distance);
	}

	bool Frustum::Plane::IsInsideOrIntersects(const Tbx::Vector3d& _Point) const
	{
		return SignedDistanceToPlane(_Point) <= epsilon;
	}

	bool Frustum::Plane::IsInsideOrIntersects(const Tbx::Vector3d& _SphereCenter, double radius) const
	{
		return SignedDistanceToPlane(_SphereCenter) - radius <= epsilon;
	}

	bool Frustum::Plane::IsInsideOrIntersects(const Tbx::Vector3d& _Center, const Tbx::Vector3d& _Extents) const
	{
		const Tbx::Vector3d absExtents(
			std::abs(_Extents.x),
			std::abs(_Extents.y),
			std::abs(_Extents.z));

		const Tbx::Vector3d absNormal(
			std::abs(Normal.x),
			std::abs(Normal.y),
			std::abs(Normal.z));

		const double r = Tbx::Vector3d::Dot(absExtents, absNormal);
		const double s = SignedDistanceToPlane(_Center);


		return (s - r) <= epsilon;
	}
}
