#include "EditWorldWindow.hpp"

#include <Imgui/Imgui.h>
#include "Editor.hpp"
#include "App.hpp"
#include "EasingFunction.hpp"
#include "Time/CoreTime.hpp"
#include "Log.hpp"

using namespace PC_EDITOR_CORE;

PC_EDITOR_CORE::EditWorldWindow::EditWorldWindow(Editor& _editor, const std::string& _name) : WorldViewWindow(_editor,_name)
{
    RotateCamera(0.2f);
    m_RenderingContextFlag |= PC_CORE::RenderingContextFlag::DebugDrawGeometry;
}


void PC_EDITOR_CORE::EditWorldWindow::Update()
{
    PERF_REGION_SCOPED;

    WorldViewWindow::Update();

    if(resize)
        deltass.Reset();

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

    HideCursor();
    CameraChangeSpeed(deltatime);
    CameratMovment(deltatime);
    RotateCamera(deltatime);

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

    if (pitch >= MaxPitch)
        pitch = MaxPitch;
    if (pitch <= -MaxPitch)
        pitch = -MaxPitch;

    camera.front = camera.front.Normalize();
    Tbx::Vector3d forward;
    forward.x = std::cos(yaw * Tbx::dDeg2Rad) * std::cos(pitch * Tbx::dDeg2Rad);
    forward.y = std::sin(pitch * Tbx::dDeg2Rad);
    forward.z = std::sin(yaw * Tbx::dDeg2Rad) * std::cos(pitch * Tbx::dDeg2Rad);
    
    camera.LookAt(camera.position + forward);
}

void EditWorldWindow::CameratMovment(float _deltatime)
{
    
    bool isPositionDirty = false;
    Tbx::Vector3d addVector = Tbx::Vector3d::Zero();
    const Tbx::Vector3d right = Tbx::Vector3d::Cross(camera.front, camera.up);
    
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

    float mag = addVector.Magnitude();
    if (mag <= Tbx::Epsilon<float>())
    {
        camera.position = SmoothDamp(camera.position, camera.position, m_CameraSpeed, smoothTime, _deltatime);
    }
    else
    {
        Tbx::Vector3d desiredPosition = camera.position + (addVector.Normalize() * m_CameraSpeedValue);
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
