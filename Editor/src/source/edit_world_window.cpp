#include "edit_world_window.hpp"

#include "editor.hpp"
#include "Imgui/imgui.h"

PC_EDITOR_CORE::EditWorldWindow::EditWorldWindow(Editor& _editor, const std::string& _name) : WorldViewWindow(_editor,_name)
{

}



void PC_EDITOR_CORE::EditWorldWindow::Update()
{
    WorldViewWindow::Update();

    if(ImGui::IsWindowFocused())
        MoveCamera();
}

void PC_EDITOR_CORE::EditWorldWindow::MoveCamera()
{
    const auto io = ImGui::GetIO();


    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        deltass.Reset();
    }

    
    if (ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonRight))
    {
        Tbx::Vector2f vec = { io.MouseDelta.x , -io.MouseDelta.y };
        deltass.AddSample(vec);
        const Tbx::Vector2f avarage = deltass.GetAvarage<Tbx::Vector2f>();
        yaw += avarage.x;
        pitch += avarage.y;
        
        constexpr float MaxPitch = 89.f;
        
        if (pitch > MaxPitch)
            pitch = MaxPitch;
        if (pitch < -MaxPitch)
            pitch = -MaxPitch;

        camera.front.x = std::cos(yaw * Deg2Rad) * std::cos(pitch * Deg2Rad);
        camera.front.y = std::sin(pitch * Deg2Rad);
        camera.front.z = std::sin(yaw * Deg2Rad) * std::cos(pitch * Deg2Rad);
        camera.front = camera.front.Normalize();

    }

    const Tbx::Vector3f right = Tbx::Vector3f::Cross(camera.front, Tbx::Vector3f::UnitY());
    camera.up = Tbx::Vector3f::Cross(right, camera.front).Normalize();


    Tbx::Vector3f addVector;
    
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
        addVector +=  camera.up; 
    }
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        addVector -=  camera.up; 
    }

    camera.position += addVector * io.DeltaTime * cameraSpeed;
}
