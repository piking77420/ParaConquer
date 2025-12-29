#include <backends/imgui_impl_vulkan.h>

#include "WorldViewWindow.hpp"
#include "Editor.hpp"
#include "Time/CoreTime.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/VulkanDescriptorSets.hpp"
#include "Rendering/View/CameraView.hpp"
#include "LowRenderer/CommandList.hpp"

#undef near
#undef far

using namespace PC_EDITOR_CORE;

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
    : EditorWindow(_editor, _name)
{
    for (auto& it : imguiDescriptorSet)
        it = VK_NULL_HANDLE;

    m_ViewPortSampler = PC_CORE::Sampler(m_Editor->RenderHarwareInteface,"ViewPortImageSampler");
    m_ViewPortSampler
        ->SetMagFilter(PC_CORE::Filter::Linear)
        .SetMinFilter(PC_CORE::Filter::Linear)
        .Build();

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
        auto sizeI = Tbx::Vector2i(static_cast<int>(size.x), static_cast<int>(size.y));
        const float aspect = size.x / size.y;
        camera.SetAspect(aspect);
        UpdateViewPort(sizeI);
        UpdateImguiViewPort();
    }

    /*
    if (m_View)
    {
        PC_CORE::Rendering::RenderGraphContext context
        { .Rhi = m_Editor->RenderHarwareInteface,
           .View = *m_View,
           .RenderGraph = m_RenderGraph,
           .RenderingWorldData = m_Editor->World.RenderingWorldData,
           .CommandBuffer = *_Cmd
        };

        m_RenderGraph.Update(context);
    }*/

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    uint32_t currentImage = m_Editor->RenderHarwareInteface.GetFrameIndex();

    //ImGui::Image(imguiDescriptorSet[currentImage], ImVec2{viewportPanelSize.x, viewportPanelSize.y}, ImVec2(0, 0),
      //           ImVec2(1, 1));
}

void WorldViewWindow::Render(PC_CORE::CommandList* _Cmd)
{
    PERF_REGION_SCOPED;

    EditorWindow::Render(_Cmd);
    if (size == Tbx::Vector2f{0.f, 0.f} || !m_View)
        return;
    /*
    if (m_View)
    {
        PC_CORE::Rendering::RenderGraphContext context
        {  .Rhi = m_Editor->RenderHarwareInteface,
           .View = *m_View,
           .RenderGraph = m_RenderGraph,
           .RenderingWorldData = m_Editor->World.RenderingWorldData,
           .CommandBuffer = *_Cmd
        };

        m_RenderGraph.Update(context);
    }*/
}

void WorldViewWindow::UpdateImguiViewPort()
{
    /*
    bool needFree = false;
    for (auto& it : imguiDescriptorSet)
        if (it != VK_NULL_HANDLE)
        {
            needFree = true;
            break;
        }

    
    if (needFree)
        m_Editor->IMGUIContext.DestroyVulkanTexture(imguiDescriptorSet.data(), imguiDescriptorSet.size());
    m_Editor->IMGUIContext.CreateImguiVulkanTexture(m_View->FinalImage.get(),
                                                    m_ViewPortSampler.Get(), imguiDescriptorSet.data(),
                                                    imguiDescriptorSet.size());*/
}

void WorldViewWindow::UpdateViewPort(Tbx::Vector2i _Size)
{
    /*
    if (!m_View )
    {
        m_View.reset(new PC_CORE::Rendering::CameraView(m_Editor->RenderHarwareInteface, _Size));
        m_View->DeclarePass(&m_RenderGraph);
        m_RenderGraph.Build();
    }
    if (m_View->renderSize != _Size)
    {
        m_View->SetCamera(&camera);
    }*/

}
