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

Tbx::Matrix4x4f Camera::GetViewMatrix() const
{
    Tbx::Matrix4x4f viewMatrix;
    return Tbx::LookAtRH(position, position + m_Front, m_Up);
}

Tbx::Matrix4x4f Camera::GetProjectionMatrix() const
{
    Tbx::Matrix4x4f projectionMatrix;
    
    return m_ProjectionType == ProjectionType::PERSPECTIVE ? Tbx::PerspectiveMatrix(m_Fov, m_Aspect ,m_Near, m_Far) :
    Tbx::OrthoGraphicMatrix(m_LeftRightScreen.x,m_LeftRightScreen.y , m_BottomTopScreen.x,  m_BottomTopScreen.y, m_Near, m_Far);
}

Tbx::Matrix4x4f Camera::GetVPMatrix() const
{
    return GetViewMatrix() * GetProjectionMatrix();
}

Tbx::Vector3f Camera::GetFront() const
{
    return m_Front;
}

Tbx::Vector3f Camera::GetUp() const
{
    return m_Up;
}

void Camera::LookAt(Tbx::Vector3f _point, Tbx::Vector3f _up)
{
    m_Front = (_point - position).Normalize();
    m_Up = _up; 
}

void Camera::LookAt(Tbx::Vector3f _point)
{
    m_Front = (_point - position).Normalize();
    const Tbx::Vector3f right = Tbx::Vector3f::Cross(m_Front, Tbx::Vector3f::UnitY()).Normalize();
    m_Up = Tbx::Vector3f::Cross(right, m_Front).Normalize();
}

Camera::Camera(float _fov, float _aspect, float _near, float _far, Tbx::Vector3f _pos, Tbx::Vector3f _forward,
               Tbx::Vector3f _up) : m_Fov(_fov), m_Aspect(_aspect),  position(_pos), m_Front(_forward), m_Up(_up) 
{
    
}

Camera::Camera(Tbx::Vector2f screenSize, float _near, float _far, Tbx::Vector3f _pos, Tbx::Vector3f _forward,
    Tbx::Vector3f _up) : m_LeftRightScreen(Tbx::Vector2f(0.f - screenSize.x, screenSize.x)) ,m_BottomTopScreen(
        Tbx::Vector2f(0.f - screenSize.y, screenSize.y)), position(_pos), m_Front(_forward), m_Up(_up)
{
    
}

