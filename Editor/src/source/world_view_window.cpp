#include "world_view_window.hpp"

#include "editor.hpp"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_vulkan.h"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_EDITOR_CORE; 

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
: EditorWindow(_editor,_name)
{
    viewport.renderer = &_editor.renderer;
    viewport.Init();
    m_ImaguiDescriptorSet.resize(PC_CORE::VulkanInterface::GetNbrOfImage());
    
    for (size_t i = 0; i < m_ImaguiDescriptorSet.size(); i++)
    {
        m_ImaguiDescriptorSet[i] = ImGui_ImplVulkan_AddTexture(PC_CORE::VulkanInterface::vulkanTextureSampler.defaultSampler.textureSampler
            ,viewport.forwardAttachments[i].colorImage.textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}

void WorldViewWindow::Update()
{
    EditorWindow::Update();
    if (resize)
    {
        m_Editor->renderer.WaitGPU();
        viewport.OnResize({static_cast<int>(size.x),static_cast<int>(size.y) } );
        for (size_t i = 0; i < m_ImaguiDescriptorSet.size(); i++)
        {
            ImGui_ImplVulkan_RemoveTexture(m_ImaguiDescriptorSet[i]);
            m_ImaguiDescriptorSet[i] = ImGui_ImplVulkan_AddTexture(PC_CORE::VulkanInterface::vulkanTextureSampler.defaultSampler.textureSampler
                ,viewport.forwardAttachments[i].colorImage.textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    ImGui::Image(m_ImaguiDescriptorSet.at(PC_CORE::VulkanInterface::GetCurrentFrame()), ImVec2{viewportPanelSize.x, viewportPanelSize.y} ,    ImVec2(0, 1), 
            ImVec2(1, 0));

}

void WorldViewWindow::Render()
{
    EditorWindow::Render();
    m_Editor->renderer.RenderViewPort(camera, viewport, *PC_CORE::World::world);
}
