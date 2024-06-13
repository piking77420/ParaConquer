#include "world_view_window.hpp"

#include "editor.hpp"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_vulkan.h"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_EDITOR_CORE; 

WorldViewWindow::WorldViewWindow(Editor& _editor)
: EditorWindow(_editor)
{
    viewport.renderer = &_editor.renderer;
    viewport.Init();
    m_ImaguiDescriptorSet.resize(PC_CORE::VulkanInterface::GetNbrOfImage());
    
    for (size_t i = 0; i < m_ImaguiDescriptorSet.size(); i++)
    {
        m_ImaguiDescriptorSet[i] = _editor.renderer.drawQuad.DrawQuadAddTexture(viewport.viewPortTexture[i]);
    }
}

void WorldViewWindow::Update()
{
    EditorWindow::Update();

    const Vector2i windowSize = { static_cast<int32_t>(ImGui::GetWindowWidth()) ,
        static_cast<int32_t>(ImGui::GetWindowHeight()) };
    
    if (viewport.OnResize(windowSize))
    {
        // TODO LEARB MULTITHREAD IN ORDER TO REMOVE THIS 
        
        for (size_t i = 0; i < m_ImaguiDescriptorSet.size(); i++)
        {
            m_ImaguiDescriptorSet[i] = ImGui_ImplVulkan_AddTexture(PC_CORE::VulkanInterface::vulkanTextureSampler.defaultSampler.textureSampler
                ,viewport.viewPortTexture[i].textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }
    ShowViewPort();
}

void WorldViewWindow::ShowViewPort()
{

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    ImGui::Image(m_ImaguiDescriptorSet.at(PC_CORE::VulkanInterface::GetCurrentFrame()), ImVec2{viewportPanelSize.x, viewportPanelSize.y});

}
