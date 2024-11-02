#include "world_view_window.hpp"

#include "editor.hpp"
#include "vulkan_header.h"
#include "time/core_time.hpp"
#include "Imgui/imgui_impl_vulkan.h"
#include "rendering/render_harware_interface/RHI.hpp"

#undef near
#undef far

using namespace PC_EDITOR_CORE;

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
    : EditorWindow(_editor, _name)
{
    m_ImaguiDescriptorSet.resize(MAX_FRAMES_IN_FLIGHT);
}

WorldViewWindow::~WorldViewWindow()
{
    //m_Editor->renderer.vulkanViewport.DestroyViewPort(viewportId);
}

void WorldViewWindow::Update()
{
    EditorWindow::Update();
    
    if (resize)
    {
        m_Editor->renderer.WaitDevice();

        const int32_t widht = static_cast<int32_t>(size.x);
        const int32_t height = static_cast<int32_t>(size.y);
        const Tbx::Vector2i textureSize = m_Texture.GetTextureSize();
        
        if (textureSize.x != widht || textureSize.y != height)
        {
            ResizeViewport();        
        }

        
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    uint32_t currentImage = PC_CORE::RHI::GetInstance().GetCurrentImage();
    VkDescriptorSet& descriptorSet = m_ImaguiDescriptorSet.at(static_cast<size_t>(currentImage));
    
    ImGui::Image( reinterpret_cast<ImTextureID>(m_ImaguiDescriptorSet.at(static_cast<size_t>(currentImage))), ImVec2{viewportPanelSize.x, viewportPanelSize.y} , ImVec2(0, 1), 
            ImVec2(1, 0));
}

void WorldViewWindow::Render()
{
    EditorWindow::Render();

    const Tbx::Vector2ui windowSize = m_Editor->window->GetWindowSize();
    PC_CORE::RenderingContext renderingContext;
    renderingContext.renderingContextSize = {static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)};
    renderingContext.lowLevelCamera =
    {
        .position = camera.position,
        .front = camera.front,
        .up = camera.up,
        .aspect = m_Editor->window->GetAspect(),
        .fov = camera.GetFOV(),
        .near = camera.GetNear(),
        .far = camera.GetFar(),
        .isOrthographic = camera.GetProjectionType() == PC_CORE::ProjectionType::ORTHOGRAPHIC,
    };
    renderingContext.time = PC_CORE::Time::GetTime(); 
    renderingContext.deltaTime = PC_CORE::Time::DeltaTime(); 

    m_Editor->renderer.Render(renderingContext, m_Editor->world);
}

void WorldViewWindow::ResizeViewport()
{
    // Wait the gpu before resizing a resource
    m_Editor->renderer.WaitDevice();
    
    const PC_CORE::CreateTextureInfo createTextureInfo =
        {
        .width = static_cast<int32_t>(m_Editor->window->GetWindowSize().x),
        .height =  static_cast<int32_t>(m_Editor->window->GetWindowSize().y),
        .format = PC_CORE::RHIFormat::R8G8B8A8_SRGB,
        .imageAspectFlagBits = PC_CORE::IMAGE_ASPECT_COLOR_BIT
        };

    m_Texture = PC_CORE::Texture(createTextureInfo);

    for (size_t i = 0; i < m_ImaguiDescriptorSet.size(); i++)
    {
        ImGui_ImplVulkan_RemoveTexture(m_ImaguiDescriptorSet[i]);
        m_ImaguiDescriptorSet[i] = ImGui_ImplVulkan_AddTexture(CastObjectToVkObject<vk::Sampler>(m_Texture.GetSamplerHandle())
            , CastObjectToVkObject<vk::ImageView>(m_Texture.GetImageViewHandle()), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}
