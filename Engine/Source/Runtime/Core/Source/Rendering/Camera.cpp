#include "rendering/camera.hpp"

#include "Math/MatrixTransformation.hpp"

using namespace PC_CORE;

void Camera::SetProjectionType(ProjectionType _projectionType)
{
    m_ProjectionType = _projectionType;
}

ProjectionType Camera::GetProjectionType() const
{
    return m_ProjectionType;
}

void Camera::SetFov(float _fov)
{
    m_Fov = _fov;
}

float Camera::GetFov() const
{
    return m_Fov;
}

void Camera::SetAspect(float _aspect)
{
    m_Aspect = _aspect;
}

float Camera::GetAspect() const
{
    return m_Aspect;
}

void Camera::SetNear(float _near)
{
    m_Near = _near;
}

float Camera::GetNear() const
{
    return m_Near;
}

void Camera::SetFar(float _far)
{
    m_Far = _far;
}

float Camera::GetFar() const
{
    return m_Far;
}

Tbx::Matrix4x4d Camera::GetViewMatrix() const
{
    Tbx::Matrix4x4f viewMatrix;
    return Tbx::LookAtRH(Position, Position + Front, Up);
}

Tbx::Matrix4x4d Camera::GetProjectionMatrix() const
{
    return m_ProjectionType == ProjectionType::Perspective
               ? Tbx::PerspectiveMatrix(m_Fov, m_Aspect, m_Near, m_Far)
               : Tbx::OrthoGraphicMatrix(m_LeftRightScreen.x, m_LeftRightScreen.y, m_BottomTopScreen.x,
                                         m_BottomTopScreen.y, m_Near, m_Far);
}

Tbx::Matrix4x4d Camera::GetVpMatrix() const
{
    return GetViewMatrix() * GetProjectionMatrix();
}


void Camera::LookAt(const Tbx::Vector3d& _point, const Tbx::Vector3d& _up)
{
    Front = (_point - Position).Normalize();
    Up = _up;
}

void Camera::LookAt(const Tbx::Vector3d& _point)
{
    Front = (_point - Position).Normalize();
    const Tbx::Vector3d right = Tbx::Vector3d::Cross(Front, Tbx::Vector3d::UnitY()).Normalize();
    Up = Tbx::Vector3d::Cross(right, Front).Normalize();
}

void Camera::SetScreenSize(int width, int height)
{
    m_Aspect = static_cast<float>(width) / static_cast<float>(height);
}

Camera::Camera(float _fov, float _aspect, float _near, float _far, const Tbx::Vector3d& _pos, const Tbx::Vector3d& _forward,
               const Tbx::Vector3d& _up) : Position(_pos), Up(_up), Front(_forward), m_Fov(_fov), m_Aspect(_aspect)
{
}

Camera::Camera(Tbx::Vector2f _screenSize, float _near, float _far, const Tbx::Vector3d& _pos, const Tbx::Vector3d& _forward,
               const Tbx::Vector3d& _up) : Position(_pos), Up(_up), Front(_forward), m_BottomTopScreen(
                                        Tbx::Vector2f(0.f - _screenSize.y, _screenSize.y)),
                                    m_LeftRightScreen(Tbx::Vector2f(0.f - _screenSize.x, _screenSize.x))
{
}
