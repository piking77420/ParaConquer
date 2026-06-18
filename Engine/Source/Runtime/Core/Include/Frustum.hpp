#ifndef CORE_FRUSTUM
#define CORE_FRUSTUM

#include <array>
#include <Math/ToolBoxTypeDef.hpp>

namespace PC_CORE
{
	class Frustum
	{
	public:
		enum Corner
		{
			// Near plane
			NearTopLeft,
			NearTopRight,
			NearBottomRight,
			NearBottomLeft,

			// Far plane
			FarTopLeft,
			FarTopRight,
			FarBottomRight,
			FarBottomLeft,
		};

		static constexpr std::array<Tbx::Vector4d, 8> OpenglNdc =
		{
			// Near plane, z = 0
			Tbx::Vector4d(-1.0,  1.0, -1.0, 1.0), // 0 Near top left
			Tbx::Vector4d(1.0,  1.0, -1.0, 1.0), // 1 Near top right
			Tbx::Vector4d(1.0, -1.0, -1.0, 1.0), // 2 Near bottom right
			Tbx::Vector4d(-1.0, -1.0, -1.0, 1.0), // 3 Near bottom left

			// Far plane, z = 1
			Tbx::Vector4d(-1.0,  1.0, 1.0, 1.0), // 4 Far top left
			Tbx::Vector4d(1.0,  1.0, 1.0, 1.0), // 5 Far top right
			Tbx::Vector4d(1.0, -1.0, 1.0, 1.0), // 6 Far bottom right
			Tbx::Vector4d(-1.0, -1.0, 1.0, 1.0)  // 7 Far bottom left
		};


		Frustum(const std::array<Tbx::Vector4d, 8>& NdcCorner , const Tbx::Matrix4x4d& FrustumToWorld);
		Frustum() = default;
		~Frustum() = default;

		[[nodiscard]] bool IsOnFrustum(const Tbx::Vector3d& _Center, const Tbx::Vector3d& _Extend) const;
		void StreamPlanes(float* _Planes) const;
	private:
		enum Planes
		{
			Near,
			Far,
			Left,
			Right,
			Top,
			Bottom,

			Count
		};


		struct Plane
		{
			static constexpr double epsilon = 1e-8;

			Tbx::Vector3d Normal;
			double Distance;

			Plane() = default;
			~Plane() = default;
			Plane(const Tbx::Vector4d& _P1, const Tbx::Vector4d& _P2, const Tbx::Vector4d& _P3);

			[[nodiscard]] double SignedDistanceToPlane(const Tbx::Vector3d& _Point) const;

			[[nodiscard]] bool IsInsideOrIntersects(const Tbx::Vector3d& _Point) const;

			[[nodiscard]] bool IsInsideOrIntersects(const Tbx::Vector3d& _SphereCenter, double radius) const;

			[[nodiscard]] bool IsInsideOrIntersects(const Tbx::Vector3d& _Center, const Tbx::Vector3d& _Extend) const;
		};

		std::array<Plane, std::to_underlying(Planes::Count)> m_Planes;
	};
}

#endif
