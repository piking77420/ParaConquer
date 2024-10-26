#include "world_view_window.hpp"

#include "editor.hpp"

#undef near
#undef far

using namespace PC_EDITOR_CORE;

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
    : EditorWindow(_editor, _name)
{
    //viewportId = _editor.renderer.vulkanViewport.CreateViewPort(true);
    //m_ImaguiDescriptorSet.resize(PC_CORE::VulkanInterface::GetNbrOfImage());

    //viewPort = &_editor.renderer.vulkanViewport.GetViewPort(viewportId);

    /*
    for (size_t i = 0; i < m_ImaguiDescriptorSet.size(); i++)
    {
        m_ImaguiDescriptorSet[i] = ImGui_ImplVulkan_AddTexture(PC_CORE::VulkanInterface::vulkanTextureSampler.defaultSampler.textureSampler
            ,viewPort->forwardAttachments[i].colorImage.textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }*/
}

WorldViewWindow::~WorldViewWindow()
{
    //m_Editor->renderer.vulkanViewport.DestroyViewPort(viewportId);
}

void WorldViewWindow::Update()
{
    EditorWindow::Update();
    /*
    if (resize)
    {
        m_Editor->renderer.WaitGPU();
        m_Editor->renderer.vulkanViewport.OnResize(viewportId, {static_cast<int>(size.x),static_cast<int>(size.y) } );
        
        for (size_t i = 0; i < m_ImaguiDescriptorSet.size(); i++)
        {
            ImGui_ImplVulkan_RemoveTexture(m_ImaguiDescriptorSet[i]);
            m_ImaguiDescriptorSet[i] = ImGui_ImplVulkan_AddTexture(PC_CORE::VulkanInterface::vulkanTextureSampler.defaultSampler.textureSampler
                ,viewPort->forwardAttachments[i].colorImage.textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    ImGui::Image(m_ImaguiDescriptorSet.at(PC_CORE::VulkanInterface::GetCurrentFrame()), ImVec2{viewportPanelSize.x, viewportPanelSize.y} , ImVec2(0, 1), 
            ImVec2(1, 0));*/
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
        .front = camera.GetFront(),
        .up = camera.GetUp(),
        .aspect = m_Editor->window->GetAspect(),
        .fov = camera.GetFOV(),
        .near = camera.GetNear(),
        .far = camera.GetFar(),
        .isOrthographic = camera.GetProjectionType() == PC_CORE::ProjectionType::ORTHOGRAPHIC,
    };

    m_Editor->renderer.Render(renderingContext);
}
