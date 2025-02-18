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

    Tbx::Vector3f position = {0,1,-10};

    Tbx::Vector3f up = Tbx::Vector3f::UnitY();

    Tbx::Vector3f front = Tbx::Vector3f::UnitZ();
    
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

    PC_CORE_API Tbx::Matrix4x4f GetViewMatrix() const;

    PC_CORE_API Tbx::Matrix4x4f GetProjectionMatrix() const;

    PC_CORE_API Tbx::Matrix4x4f GetVPMatrix() const;
    
    PC_CORE_API void LookAt(Tbx::Vector3f _point, Tbx::Vector3f _up);

    PC_CORE_API void LookAt(Tbx::Vector3f _point);

    PC_CORE_API Camera() = default;
    
    // Fov in radians
    PC_CORE_API Camera(float _fov, float _aspect, float _near, float _far,
        Tbx::Vector3f _pos,  Tbx::Vector3f _forward, Tbx::Vector3f _up);
    
    PC_CORE_API Camera(Tbx::Vector2f screenSize, float _near, float _far,
        Tbx::Vector3f _pos,  Tbx::Vector3f _forward, Tbx::Vector3f _up);

    PC_CORE_API ~Camera() = default;
    
private:
    ProjectionType m_ProjectionType = ProjectionType::PERSPECTIVE;
    
    // in radians
    float m_Fov = 90.f * Deg2Rad;
    float m_Aspect = 16.f / 9.f;    
    float m_Near = 0.1f;
    float m_Far = 1000.f;

    
    Tbx::Vector2f m_BottomTopScreen;
    Tbx::Vector2f m_LeftRightScreen;
};

END_PCCORE