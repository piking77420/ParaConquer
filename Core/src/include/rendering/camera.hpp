#pragma once

#include "core_header.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE
enum class ProjectionType
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class Camera
{
public:

    Tbx::Vector3d position = Tbx::Vector3d(0,1,-10);

    Tbx::Vector3d up = Tbx::Vector3d::UnitY();

    Tbx::Vector3d front = Tbx::Vector3d::UnitZ();
    
    PC_CORE_API void SetProjectionType(ProjectionType projectionType);

    PC_CORE_API ProjectionType GetProjectionType() const;

    PC_CORE_API void SetFOV(float fov);

    PC_CORE_API float GetFOV() const;

    PC_CORE_API void SetAspect(float aspect);

    PC_CORE_API float GetAspect() const;

    PC_CORE_API void SetNear(float near);

    PC_CORE_API float GetNear() const;

    PC_CORE_API void SetFar(float far);

    PC_CORE_API float GetFar() const;

    PC_CORE_API Tbx::Matrix4x4d GetViewMatrix() const;

    PC_CORE_API Tbx::Matrix4x4d GetProjectionMatrix() const;

    PC_CORE_API Tbx::Matrix4x4d GetVPMatrix() const;
    
    PC_CORE_API void LookAt(Tbx::Vector3d _point, Tbx::Vector3d _up);

    PC_CORE_API void LookAt(Tbx::Vector3d _point);

    PC_CORE_API Camera() = default;

    PC_CORE_API void SetScreenSize(int width, int height);
    
    // Fov in radians
    PC_CORE_API Camera(float _fov, float _aspect, float _near, float _far,
        Tbx::Vector3d _pos,  Tbx::Vector3d _forward, Tbx::Vector3d _up);
    
    PC_CORE_API Camera(Tbx::Vector2f screenSize, float _near, float _far,
        Tbx::Vector3d _pos,  Tbx::Vector3d _forward, Tbx::Vector3d _up);

    PC_CORE_API ~Camera() = default;
    
private:
    ProjectionType m_ProjectionType = ProjectionType::PERSPECTIVE;
    
    // in radians
    float m_Fov = 90.f * Tbx::fDeg2Rad;
    float m_Aspect = 16.f / 9.f;    
    float m_Near = 0.1f;
    float m_Far = 10000.f;

    
    Tbx::Vector2f m_BottomTopScreen;
    Tbx::Vector2f m_LeftRightScreen;
};

END_PCCORE