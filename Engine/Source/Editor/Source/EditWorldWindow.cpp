#include "EditWorldWindow.hpp"

#include <Imgui.h>

#include "App.hpp"
#include "EasingFunction.hpp"
#include "Editor.hpp"
#include "Log.hpp"
#include "Time/CoreTime.hpp"

using namespace PC_EDITOR_CORE;

EditWorldWindow::EditWorldWindow(Editor& _editor, const std::string& _name) : WorldViewWindow(_editor, _name)
{
    RotateCamera(0.01f);
}


void EditWorldWindow::Update()
{
    PERF_REGION_SCOPED;

    if (resize)
        deltass.Reset();

    if (ImGui::IsWindowFocused())
        MoveCameraUpdate();

    WorldViewWindow::Update();
}

void EditWorldWindow::MoveCameraUpdate()
{
    const float deltatime = ImGui::GetIO().DeltaTime;

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        deltass.Reset();
    }

    HideCursor();
    CameraChangeSpeed(deltatime);

    
    CameratMovment(deltatime);
    RotateCamera(deltatime);
    ScroolWheelMovement(deltatime);
    UpdatePosition(deltatime);
}

void EditWorldWindow::RotateCamera(float _deltatime)
{
    // TODO USE QUATERNION TO SLERP

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        deltass.Reset();
        return;
    }


    const ImGuiIO& io = ImGui::GetIO();
    const Tbx::Vector2f delta = {io.MouseDelta.x, -io.MouseDelta.y};
    if (std::abs(delta.x) < 0.0001f && std::abs(delta.y) < 0.0001f)
    {
        return;
    }

    deltass.AddSample(delta);
    const Tbx::Vector2f average = deltass.GetAvarage<Tbx::Vector2f>();
    yaw += average.x * cameraSensitivity;
    pitch += average.y * cameraSensitivity;

    constexpr float MaxPitch = 89.0f;
    pitch = std::clamp(pitch, -MaxPitch, MaxPitch);

    Tbx::Vector3d forward;
    forward.x = std::cos(yaw * Tbx::dDeg2Rad) * std::cos(pitch * Tbx::dDeg2Rad);
    forward.y = std::sin(pitch * Tbx::dDeg2Rad);
    forward.z = std::sin(yaw * Tbx::dDeg2Rad) * std::cos(pitch * Tbx::dDeg2Rad);

    m_Camera.LookAt(m_Camera.Position + forward);

    m_CameraViewDirty = true;
}

void EditWorldWindow::CameratMovment(float _deltatime)
{
    bool isPositionDirty = false;
    Tbx::Vector3d addVector = Tbx::Vector3d::Zero();
    const Tbx::Vector3d right = Tbx::Vector3d::Cross(m_Camera.Front, m_Camera.Up);

    if (ImGui::IsKeyDown(ImGuiKey_W))
    {
        addVector += m_Camera.Front;
    }
    if (ImGui::IsKeyDown(ImGuiKey_S))
    {
        addVector -= m_Camera.Front;
    }

    if (ImGui::IsKeyDown(ImGuiKey_A))
    {
        addVector -= right;
    }
    if (ImGui::IsKeyDown(ImGuiKey_D))
    {
        addVector += right;
    }


    m_DesiredVelocity += addVector * m_CameraSpeedValue;
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
    Tbx::Vector2f mousePos = {ImMousPos.x, ImMousPos.y};

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
       m_Editor->MainWindow.HideCursor(false);
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        m_Editor->MainWindow.HideCursor(true);
    }
}

void EditWorldWindow::ScroolWheelMovement(float _Deltatime)
{
    float mouseWheel = ImGui::GetIO().MouseWheel;

    if (mouseWheel == 0.f)
        return;

    m_DesiredVelocity += m_Camera.Front * mouseWheel * ScrolWheelForce;
}

void EditWorldWindow::UpdatePosition(float _Deltatime)
{
    m_CameraVelocity = Lerp(
        m_CameraVelocity,
        m_DesiredVelocity,
        1.f - std::exp(-AccTime * _Deltatime)
    );

    m_Camera.Position += m_CameraVelocity * _Deltatime;

    m_CameraViewDirty = true;
    m_DesiredVelocity = Tbx::Vector3d::Zero();
}
