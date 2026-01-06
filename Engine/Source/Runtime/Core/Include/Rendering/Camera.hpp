#pragma once

#include "CoreHeader.hpp"
#include "Math/ToolboxTypedef.hpp"

BEGIN_PCCORE
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

    class Camera
    {
    public:
        // Fov in radians
        PC_CORE_API Camera(float _fov, float _aspect, float _near, float _far,
            const Tbx::Vector3d& _pos, const Tbx::Vector3d& _forward, const Tbx::Vector3d& _up);

        PC_CORE_API Camera(Tbx::Vector2f _screenSize, float _near, float _far,
            const Tbx::Vector3d& _pos, const Tbx::Vector3d& _forward, const Tbx::Vector3d& _up);

        PC_CORE_API ~Camera() = default;

        PC_CORE_API Camera() = default;

        PC_CORE_API void SetProjectionType(ProjectionType _projectionType);

        PC_CORE_API ProjectionType GetProjectionType() const;

        PC_CORE_API void SetFov(double _fov);

        PC_CORE_API double GetFov() const;

        PC_CORE_API void SetAspect(double _aspect);

        PC_CORE_API double GetAspect() const;

        PC_CORE_API void SetNear(double _near);

        PC_CORE_API double GetNear() const;

        PC_CORE_API void SetFar(double _far);

        PC_CORE_API double GetFar() const;

        PC_CORE_API const Tbx::Matrix4x4d& GetViewMatrix() const
        {
            return m_View;
        }

        PC_CORE_API const Tbx::Matrix4x4d& GetProjection() const
        {
            return m_Projection;
        }

        PC_CORE_API const Tbx::Matrix4x4d& GetViewProjection() const
        {
            return m_ViewProjection;
        }

        PC_CORE_API void LookAt(const Tbx::Vector3d& _point, const Tbx::Vector3d& _up);

        PC_CORE_API void LookAt(const Tbx::Vector3d& _point);

        PC_CORE_API void SetScreenSize(int width, int height);

        PC_CORE_API void ComputeMatricies();

        Tbx::Vector3d Position = Tbx::Vector3d(0, 0, -10);

        Tbx::Vector3d Up = Tbx::Vector3d::UnitY();

        Tbx::Vector3d Front = Tbx::Vector3d::UnitZ();

    private:
        ProjectionType m_ProjectionType = ProjectionType::Perspective;

        // in radians
        double m_Fov = 90.0 * Tbx::fDeg2Rad;
        double m_Aspect = 16.0 / 9.0;
        double m_Near = 0.1;
        double m_Far = 10000.0;

        Tbx::Matrix4x4d m_View = Tbx::Matrix4x4d::Identity();
        Tbx::Matrix4x4d m_Projection = Tbx::Matrix4x4d::Identity();
        Tbx::Matrix4x4d m_ViewProjection = Tbx::Matrix4x4d::Identity();

        Tbx::Vector2f m_BottomTopScreen{ 0.f,0.f };
        Tbx::Vector2f m_LeftRightScreen{ 0.f,0.f };

        void ComputeView();

        void ComputeProjection();

        void ComputeViewProjection();
    };

END_PCCORE
