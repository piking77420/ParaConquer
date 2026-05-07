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

    if (ImGui::IsWindowFocused())
        MoveCameraUpdate();

    m_View.MeshletCulling = m_Editor->editorData.CullMeshlet;
    if (!m_Editor->editorData.FreezeFrustum)
    {
        m_View.FrustumToViewSpace = m_View.ProjectionInv;
        m_View.FrustumToWorld = m_View.ViewProjectionInv;
    }
    WorldViewWindow::Update();

    if (m_Editor->editorData.DrawFrustum)
    {
        m_Editor->World.DrawWireFrustum(
            m_View.FrustumToWorld,
            Tbx::Vector3f(1, 1, 1)
        );
    }
}

void EditWorldWindow::MoveCameraUpdate()
{
    const float deltatime = ImGui::GetIO().DeltaTime;

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {

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
        return;
    }


    const ImGuiIO& io = ImGui::GetIO();
    const Tbx::Vector2f delta = {io.MouseDelta.x, -io.MouseDelta.y};
    if (std::abs(delta.x) < 0.0001f && std::abs(delta.y) < 0.0001f)
    {
        return;
    }

    yaw -= delta.x * cameraSensitivity;
    pitch += delta.y * cameraSensitivity;
    constexpr float MaxPitch = 89.0f;
    pitch = std::clamp(pitch, -MaxPitch, MaxPitch);


    const Tbx::Quaternionf qYaw = Tbx::Quaternionf::FromAxisAngle(Tbx::Vector3f(0.f, 1.f, 0.f), yaw * Tbx::fDeg2Rad);
    const Tbx::Quaternionf qPitch = Tbx::Quaternionf::FromAxisAngle(Tbx::Vector3f(1.f, 0.f, 0.f), pitch * Tbx::fDeg2Rad);
    const Tbx::Quaternionf qTarget = qYaw * qPitch;

    //const float rotationSmoothness = 7.0f; // tweak to taste
    //float t = 1.0f - std::exp(-rotationSmoothness * io.DeltaTime);
    //t = std::clamp(t, 0.f, 1.f);

    m_Orientation = qTarget;/*Tbx::Quaternionf::Nlerp(m_Orientation, qTarget, t);*/

    m_Orientation.Normalize();
    

    Tbx::Vector3f Forward = m_Orientation * Tbx::Vector3f(0.f, 0.f, -1.f );

    m_Camera.LookAt(m_Camera.Position + Tbx::Vector3d(static_cast<double>(Forward.x), static_cast<double>(Forward.y), static_cast<double>(Forward.z)));

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

    m_CameraSpeedValue += mouseWheel * ScrolWheelForce;
    if (m_CameraSpeedValue < 0.f)
        m_CameraSpeedValue = std::abs(m_BaseCameraSpeed);
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
