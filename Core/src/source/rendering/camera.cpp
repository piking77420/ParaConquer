#include "rendering/camera.hpp"

#include "math/matrix_transformation.hpp"

using namespace PC_CORE;

void Camera::SetProjectionType(ProjectionType projectionType)
{
    m_ProjectionType = projectionType;        
}

ProjectionType Camera::GetProjectionType() const
{
    return m_ProjectionType;
}

void Camera::SetFOV(float _fov)
{
    m_Fov = _fov;    
}

float Camera::GetFOV() const
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

void Camera::SetNear(float near)
{
    m_Near = near;
}

float Camera::GetNear() const
{
    return m_Near;  
}

void Camera::SetFar(float far)
{
    m_Far = far;
}

float Camera::GetFar() const
{
    return m_Far;
}

Tbx::Matrix4x4d Camera::GetViewMatrix() const
{
    Tbx::Matrix4x4f viewMatrix;
    return Tbx::LookAtRH(position, position + front, up);
}

Tbx::Matrix4x4d Camera::GetProjectionMatrix() const
{    
    return m_ProjectionType == ProjectionType::PERSPECTIVE ? Tbx::PerspectiveMatrix(m_Fov, m_Aspect ,m_Near, m_Far) :
    Tbx::OrthoGraphicMatrix(m_LeftRightScreen.x,m_LeftRightScreen.y , m_BottomTopScreen.x,  m_BottomTopScreen.y, m_Near, m_Far);
}

Tbx::Matrix4x4d Camera::GetVPMatrix() const
{
    return GetViewMatrix() * GetProjectionMatrix();
}



void Camera::LookAt(Tbx::Vector3d _point, Tbx::Vector3d _up)
{
    front = (_point - position).Normalize();
    up = _up; 
}

void Camera::LookAt(Tbx::Vector3d _point)
{
    front = (_point - position).Normalize();
    const Tbx::Vector3d right = Tbx::Vector3d::Cross(front, Tbx::Vector3d::UnitY()).Normalize();
    up = Tbx::Vector3d::Cross(right, front).Normalize();
}

void Camera::SetScreenSize(int width, int height)
{
    m_Aspect = static_cast<float>(width) / static_cast<float>(height);
}

Camera::Camera(float _fov, float _aspect, float _near, float _far, Tbx::Vector3d _pos, Tbx::Vector3d _forward,
               Tbx::Vector3d _up) : m_Fov(_fov), m_Aspect(_aspect),  position(_pos), front(_forward), up(_up) 
{
    
}

Camera::Camera(Tbx::Vector2f screenSize, float _near, float _far, Tbx::Vector3d _pos, Tbx::Vector3d _forward,
    Tbx::Vector3d _up) : m_LeftRightScreen(Tbx::Vector2f(0.f - screenSize.x, screenSize.x)) ,m_BottomTopScreen(
        Tbx::Vector2f(0.f - screenSize.y, screenSize.y)), position(_pos), front(_forward), up(_up)
{
    
}

