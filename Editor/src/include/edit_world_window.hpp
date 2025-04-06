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
    
    PC_CORE::LowPassFilter<Tbx::Vector2f, 8> deltass;

    Tbx::Vector3f cameraSpeed = Tbx::Vector3f::Zero();

    float cameraSpeedValue = 50.f;

    float drag = 0.001f;
    
    float pitch = 0.0f;
    
    float yaw = 0.f;

    void RotateCamera(float _deltatime);

    void CameratMovment(float _deltatime);

    void CameraChangeSpeed();
};

END_EDITOR_PCCORE