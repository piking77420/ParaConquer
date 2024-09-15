#include "world_view_window.hpp"

#include "editor.hpp"


using namespace PC_EDITOR_CORE; 

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
: EditorWindow(_editor,_name)
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
    //m_Editor->renderer.RenderViewPort(camera, viewportId, *PC_CORE::World::world);
}
