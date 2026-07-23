#include <Rendering/Camera.hpp>

#include "PerfRegion.hpp"
#include "Math/MatrixTransformation.hpp"

using namespace PC_CORE;

void Camera::SetProjectionType(ProjectionType _projectionType)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    m_ProjectionType = _projectionType;
    ComputeProjection();
    ComputeViewProjection();
}

ProjectionType Camera::GetProjectionType() const
{
    return m_ProjectionType;
}

void Camera::SetFov(double _fov)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    m_Fov = _fov;
    ComputeProjection();
    ComputeViewProjection();
}

double Camera::GetFov() const
{
    return m_Fov;
}

void Camera::SetAspect(double _aspect)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    m_Aspect = _aspect;
}

double Camera::GetAspect() const
{
    return m_Aspect;
}

void Camera::SetNear(double _near)
{
    m_Near = _near;
}

double Camera::GetNear() const
{
    return m_Near;
}

void Camera::SetFar(double _far)
{
    m_Far = _far;
}

double Camera::GetFar() const
{
    return m_Far;
}

void Camera::LookAt(const Tbx::Vector3d& _point, const Tbx::Vector3d& _up)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    Front = (_point - Position).Normalize();
    Up = _up;
}

void Camera::LookAt(const Tbx::Vector3d& _point)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    Front = (_point - Position);

    if (Front.Magnitude() < 1e-8)
    {
        return;
    }
    Front = Front.Normalize();

    const Tbx::Vector3d WorldUp = Tbx::Vector3d::UnitY();

    const Tbx::Vector3d right = Tbx::Vector3d::Cross(Front, WorldUp).Normalize();
    Up = Tbx::Vector3d::Cross(right, Front).Normalize();
}

void Camera::SetScreenSize(int width, int height)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);
    SetAspect(static_cast<float>(width) / static_cast<float>(height));
}

void Camera::ComputeMatricies()
{
    ComputeView();
    ComputeProjection();
    ComputeViewProjection();
}

Camera::Camera(float _fov, float _aspect, float _near, float _far, const Tbx::Vector3d& _pos, const Tbx::Vector3d& _forward,
               const Tbx::Vector3d& _up) : Position(_pos), Up(_up), Front(_forward), m_Fov(_fov), m_Aspect(_aspect)
{
    ComputeMatricies();
}

Camera::Camera(Tbx::Vector2f _screenSize, float _near, float _far, const Tbx::Vector3d& _pos, const Tbx::Vector3d& _forward,
               const Tbx::Vector3d& _up) : Position(_pos), Up(_up), Front(_forward), m_BottomTopScreen(
                                        Tbx::Vector2f(0.f - _screenSize.y, _screenSize.y)),
                                    m_LeftRightScreen(Tbx::Vector2f(0.f - _screenSize.x, _screenSize.x))
{
    ComputeMatricies();
}

void Camera::ComputeView()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);
    m_View = Tbx::LookAtRH(Position, Position + Front, Up);
}

void Camera::ComputeProjection()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    const double Aspect = static_cast<double>(m_Aspect);
    const double Near = static_cast<double>(m_Near);
    const double Far = static_cast<double>(m_Far);

    m_Projection = m_ProjectionType == ProjectionType::Perspective
        ? Tbx::PerspectiveMatrixMinusOneToOne(
            static_cast<double>(m_Fov),
            Aspect,
            Near,
            Far)
        : Tbx::OrthoGraphicMatrix(
            static_cast<double>(m_LeftRightScreen.x),
            static_cast<double>(m_LeftRightScreen.y), 
            static_cast<double>(m_BottomTopScreen.x),
            static_cast<double>(m_BottomTopScreen.y), Near, Far);
}

void Camera::ComputeViewProjection()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    m_ViewProjection = m_Projection * m_View;
}
