#include "edit_world_window.hpp"

#include <Imgui/imgui.h>
#include "editor.hpp"
#include "time/core_time.hpp"
#include "log.hpp"

using namespace PC_EDITOR_CORE;

PC_EDITOR_CORE::EditWorldWindow::EditWorldWindow(Editor& _editor, const std::string& _name) : WorldViewWindow(_editor,_name)
{

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
    CameraChangeSpeed();
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
    yaw += average.x;
    pitch += average.y;

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
    
    Tbx::Vector3f addVector;
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
    
    addVector = addVector.Normalize();  
    cameraSpeed += addVector * _deltatime * cameraSpeedValue;
    camera.position += addVector * 0.5f * _deltatime * _deltatime + cameraSpeed * _deltatime;
    cameraSpeed *= pow(drag,_deltatime);
}

void EditWorldWindow::CameraChangeSpeed()
{
   // const auto io = ImGui::GetIO();

    //cameraSpeedValue += io.MouseWheel * cameraSpeedValue * 0.2f;
}
