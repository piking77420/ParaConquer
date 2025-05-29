#pragma once

#include "editor_header.hpp"
#include "world_view_window.hpp"
#include "io/low_pass_filter.hpp"

BEGIN_EDITOR_PCCORE
class EditWorldWindow : public WorldViewWindow
{
public:

    explicit EditWorldWindow(Editor& _editor, const std::string& _name);
    
    ~EditWorldWindow() override = default;
    
    void Update() override;

protected:
    void MoveCameraUpDate();
    
    PC_CORE::LowPassFilter<Tbx::Vector2f, 16> deltass;

    float m_BaseCameraSpeed = 5.f;

    float m_CameraSpeedValue = m_BaseCameraSpeed;
    
    float pitch = 0.0f;
    
    float yaw = 90.f;

    float cameraSensitivity = 100.f;

    Tbx::Vector3f m_CameraSpeed = Tbx::Vector3f::Zero();

    float smoothTime = 0.3f;

    void RotateCamera(float _deltatime);

    void CameratMovment(float _deltatime);

    void CameraChangeSpeed(float _deltatime);

    void HideCursor();
};

END_EDITOR_PCCORE