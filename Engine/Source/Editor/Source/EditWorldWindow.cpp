#include "EditWorldWindow.hpp"

#include "App.hpp"
#include "EasingFunction.hpp"
#include "Editor.hpp"
#include "Log.hpp"
#include "Rendering/View/CameraView.hpp"
#include "Time/CoreTime.hpp"
#include <Imgui.h>

using namespace PC_EDITOR_CORE;

EditWorldWindow::EditWorldWindow(Editor& _editor, const std::string& _name) : WorldViewWindow(_editor, _name)
{
    bool dummy;
    RotateCamera(0.01f, &dummy);
}


void EditWorldWindow::Update()
{
    PERF_REGION_SCOPED;

    WorldViewWindow::Update();

    if (resize)
        deltass.Reset();

    if (ImGui::IsWindowFocused())
        MoveCameraUpdate();
}

void EditWorldWindow::MoveCameraUpdate()
{
    const float deltatime = PC_CORE::Time::DeltaTime();

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        deltass.Reset();
    }

    HideCursor();
    CameraChangeSpeed(deltatime);

    bool CameraDirty = false;
    CameratMovment(deltatime, &CameraDirty);
    RotateCamera(deltatime, &CameraDirty);
    if (CameraDirty && m_View)
        m_View->UpdateView();
}

void EditWorldWindow::RotateCamera(float _deltatime, bool* _isDirty)
{
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        deltass.Reset();
        return;
    }


    const ImGuiIO& io = ImGui::GetIO();
    const Tbx::Vector2f delta = {io.MouseDelta.x, -io.MouseDelta.y};
    if (std::abs(delta.x) < 0.01f && std::abs(delta.y))
    {
        return;
    }
    *_isDirty = true;

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

    camera.Front = forward.Normalize();

    camera.LookAt(camera.Position + forward);
    *_isDirty = true;
}

void EditWorldWindow::CameratMovment(float _deltatime, bool* isDirty)
{
    bool isPositionDirty = false;
    Tbx::Vector3d addVector = Tbx::Vector3d::Zero();
    const Tbx::Vector3d right = Tbx::Vector3d::Cross(camera.Front, camera.Up);

    if (ImGui::IsKeyDown(ImGuiKey_W))
    {
        addVector += camera.Front;
    }
    if (ImGui::IsKeyDown(ImGuiKey_S))
    {
        addVector -= camera.Front;
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
    if (mag > Tbx::Epsilon<float>())
    {
        Tbx::Vector3d desiredPosition = camera.Position + (addVector.Normalize() * m_CameraSpeedValue);
        camera.Position = SmoothDamp(camera.Position, desiredPosition, m_CameraSpeed, smoothTime, _deltatime);
        *isDirty = true;
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
