#include "world_view_window.hpp"

#include "editor.hpp"
#include "vulkan_header.h"
#include "time/core_time.hpp"
#include "Imgui/imgui_impl_vulkan.h"

#undef near
#undef far

using namespace PC_EDITOR_CORE;

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
    : EditorWindow(_editor, _name)
{
   
}


void WorldViewWindow::Update()
{
    EditorWindow::Update();
    
    if (resize)
    {
        //m_Editor->renderer.WaitDevice();
        ResizeViewports();
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    /*
    uint32_t currentImage = PC_CORE::RHI::GetInstance().GetCurrentImageIndex();
    VkDescriptorSet& descriptorSet = m_ImaguiDescriptorSet.at(static_cast<size_t>(currentImage));
    
    ImGui::Image( reinterpret_cast<ImTextureID>(m_ImaguiDescriptorSet.at(static_cast<size_t>(currentImage))), ImVec2{viewportPanelSize.x, viewportPanelSize.y} , ImVec2(0, 0), 
            ImVec2(1, 1));*/
}

void WorldViewWindow::Render()
{
    EditorWindow::Render();
    /*

    uint32_t currentImage = PC_CORE::RHI::GetInstance().GetCurrentImageIndex();

    const Tbx::Vector2ui windowSize = m_Editor->window.GetWindowSize();
    PC_CORE::RenderingContext renderingContext;
    renderingContext.lowLevelCamera =
    {
        .position = camera.position,
        .front = camera.front,
        .up = camera.up,
        .aspect = m_Editor->window.GetAspect(),
        .fov = camera.GetFOV(),
        .near = camera.GetNear(),
        .far = camera.GetFar(),
        .isOrthographic = camera.GetProjectionType() == PC_CORE::ProjectionType::ORTHOGRAPHIC,
        
    };  
    renderingContext.time = PC_CORE::Time::GetTime(); 
    renderingContext.deltaTime = PC_CORE::Time::DeltaTime();
    renderingContext.frameBufferHandle = m_FrameBuffers.at(static_cast<size_t>(currentImage)).GetHandle();
    renderingContext.renderingContextSize = size;

    m_Editor->renderer.Render(renderingContext, m_Editor->world);*/
}

void WorldViewWindow::ResizeViewports()
{
    
}

