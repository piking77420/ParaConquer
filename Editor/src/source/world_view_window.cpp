#include "world_view_window.hpp"

#include "editor.hpp"
#include "time/core_time.hpp"
#include "Imgui/imgui_impl_vulkan.h"
#include "resources/resource_manager.hpp"
#include "resources/vulkan_descriptor_sets.hpp"

#undef near
#undef far

using namespace PC_EDITOR_CORE;

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
    : EditorWindow(_editor, _name)
{
    for (auto& it : imguiDescriptorSet)
        it = VK_NULL_HANDLE;
}

WorldViewWindow::~WorldViewWindow()
{
    
}


void WorldViewWindow::Update()
{
    EditorWindow::Update();

    if (size == Tbx::Vector2f{0.f, 0.f})
        return;

    if (resize)
    {
        Tbx::Vector2i sizeI = Tbx::Vector2i(static_cast<int>(size.x), static_cast<int>(size.y));
        const float aspect = size.x / size.y;
        camera.SetAspect(aspect);

        if (!m_View)
        {
            m_View = m_Editor->gameApp.renderer.CreateView(sizeI);
            m_View->SetCamera(&camera);
        }
        else
        {
            m_View->Resize(sizeI);
        }

        UpdateImguiViewPort();
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    uint32_t currentImage = PC_CORE::Rhi::GetFrameIndex();

    m_View->Update();
    ImGui::Image( reinterpret_cast<ImTextureID>(imguiDescriptorSet[currentImage]), ImVec2{viewportPanelSize.x, viewportPanelSize.y}, ImVec2(0, 0), ImVec2(1, 1));
}

void WorldViewWindow::Render()
{
    PERF_REGION_SCOPED;

    EditorWindow::Render();
    if (size == Tbx::Vector2f{0.f, 0.f} || !m_View)
        return;
  
    m_Editor->gameApp.renderer.Draw(*m_View.get());
}

void WorldViewWindow::UpdateImguiViewPort()
{   
    bool needFree = false;
    for (auto& it : imguiDescriptorSet)
        if (it != VK_NULL_HANDLE)
        {
            needFree = true;
            break;
        }

    

    if (needFree) 
        m_Editor->IMGUIContext.DestroyVulkanTexture(imguiDescriptorSet.data(), imguiDescriptorSet.size());
    m_Editor->IMGUIContext.CreateImguiVulkanTexture(&m_View->finalImage, imguiDescriptorSet.data(), imguiDescriptorSet.size());

}
