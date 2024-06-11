#include "editor.hpp"

#include "edit_world_window.hpp"
#include "world_view_window.hpp"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_vulkan.h"
#include "resources/resource_manager.hpp"

namespace PC_CORE
{
    struct SphereCollider;
}

using namespace PC_EDITOR_CORE;
using namespace PC_CORE;


void Editor::Init()
{
    App::Init();
    InitEditorWindows();
    Texture* d = ResourceManager::Get<Texture>("diamond_block.jpg");
    m_Dset.resize(1);
    for (VkDescriptorSet& s : m_Dset)
    {
        s = ImGui_ImplVulkan_AddTexture(VulkanInterface::vulkanTextureSampler.Get(d->vulkanTexture.samplerId), d->vulkanTexture.textureImageView
       , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    
}

void Editor::Destroy()
{
    for (const EditorWindow* editorWindow : m_EditorWindows)
        delete editorWindow;
    
    App::Destroy();
}



void Editor::Run()
{
    while (!windowHandle.ShouldClose())
    {
        windowHandle.PoolEvents();
        HandleResize();
        vulkanImgui.NewFrame();
        MoveObject();
        renderer.BeginFrame();

        ImGui::Begin("Test");
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImGui::Image(m_Dset[0], ImVec2{viewportPanelSize.x, viewportPanelSize.y});
        ImGui::End();
        
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Begin();
            editorWindow->Update();
            editorWindow->End();
            
        }
        for (EditorWindow* editorWindow : m_EditorWindows)
        {
            editorWindow->Render();
        }
        vulkanImgui.EndFrame();
        renderer.SwapBuffers();
    }   
}

void Editor::InitEditorWindows()
{
    m_EditorWindows.push_back(new EditWorldWindow(*this));
    m_EditorWindows[0]->name = "EditorWindow";
}

