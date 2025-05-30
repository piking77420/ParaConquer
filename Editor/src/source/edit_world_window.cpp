#include "edit_world_window.hpp"

#include <Imgui/imgui.h>
#include "editor.hpp"
#include "app.hpp"
#include "easing_function.hpp"
#include "time/core_time.hpp"
#include "log.hpp"

using namespace PC_EDITOR_CORE;

PC_EDITOR_CORE::EditWorldWindow::EditWorldWindow(Editor& _editor, const std::string& _name) : WorldViewWindow(_editor,_name)
{
    
    RotateCamera(0.2f);
}


void PC_EDITOR_CORE::EditWorldWindow::Update()
{
    WorldViewWindow::Update();
    if(ImGui::IsWindowFocused())
        MoveCameraUpDate();
     
}

void PC_EDITOR_CORE::EditWorldWindow::MoveCameraUpDate()
{
    const float deltatime = PC_CORE::Time::DeltaTime();
    
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        deltass.Reset();
    }

    RotateCamera(deltatime);
    CameratMovment(deltatime);
    CameraChangeSpeed(deltatime);

    HideCursor();
}

void EditWorldWindow::RotateCamera(float _deltatime)
{
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        return;
    }


    const auto io = ImGui::GetIO();
    const Tbx::Vector2f vec = { io.MouseDelta.x , -io.MouseDelta.y };
    deltass.AddSample(vec);
    const Tbx::Vector2f average = deltass.GetAvarage<Tbx::Vector2f>();
    yaw += average.x * _deltatime * cameraSensitivity;
    pitch += average.y * _deltatime * cameraSensitivity;

    constexpr float MaxPitch = 89.f;

    if (pitch > MaxPitch)
        pitch = MaxPitch;
    if (pitch < -MaxPitch)
        pitch = -MaxPitch;

    camera.front = camera.front.Normalize();
    Tbx::Vector3f forward;
    forward.x = std::cos(yaw * Tbx::fDeg2Rad) * std::cos(pitch * Tbx::fDeg2Rad);
    forward.y = std::sin(pitch * Tbx::fDeg2Rad);
    forward.z = std::sin(yaw * Tbx::fDeg2Rad) * std::cos(pitch * Tbx::fDeg2Rad);
    
    camera.LookAt(camera.position + forward);
}

void EditWorldWindow::CameratMovment(float _deltatime)
{
    
    bool isPositionDirty = false;
    Tbx::Vector3f addVector = Tbx::Vector3f::Zero();
    const Tbx::Vector3f right = Tbx::Vector3f::Cross(camera.front, camera.up);
    
    if (ImGui::IsKeyDown(ImGuiKey_W))
    {
        addVector += camera.front;
    }
    if (ImGui::IsKeyDown(ImGuiKey_S))
    {
        addVector -= camera.front;
    }

    if (ImGui::IsKeyDown(ImGuiKey_A))
    {
        addVector -= right;
    }
    if (ImGui::IsKeyDown(ImGuiKey_D))
    {
        addVector += right;

    }

    if (ImGui::IsKeyDown(ImGuiKey_Space))
    {
        addVector += camera.up;
    }
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        addVector -= camera.up;
    }

    float mag = addVector.Magnitude();
    if (mag <= Tbx::Epsilon<float>())
    {
        camera.position = SmoothDamp(camera.position, camera.position, m_CameraSpeed, smoothTime, _deltatime);
    }
    else
    {
        Tbx::Vector3f desiredPosition = camera.position + (addVector.Normalize() * m_CameraSpeedValue);
        camera.position = SmoothDamp(camera.position, desiredPosition, m_CameraSpeed, smoothTime, _deltatime);
    }
}

void EditWorldWindow::CameraChangeSpeed(float _deltatime)
{
    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
    {
        m_CameraSpeedValue += _deltatime * 10;
    }

    if (ImGui::IsKeyReleased(ImGuiKey_LeftShift))
    {
        m_CameraSpeedValue = m_BaseCameraSpeed;
    }
}

void EditWorldWindow::HideCursor()
{
    ImVec2 ImMousPos = ImGui::GetIO().MousePos;
    Tbx::Vector2f mousePos = { ImMousPos.x , ImMousPos.y };
        
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        PC_CORE::App::instance->window.HideCursor(false);
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        PC_CORE::App::instance->window.HideCursor(true);
    }
   

	



}
