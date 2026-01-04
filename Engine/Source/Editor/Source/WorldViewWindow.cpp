#include <backends/imgui_impl_vulkan.h>

#include "WorldViewWindow.hpp"
#include "Editor.hpp"
#include "Time/CoreTime.hpp"
#include "Resources/ResourceManager.hpp"
#include "VulkanDescriptorSet.hpp"
#include "LowRenderer/CommandList.hpp"

#undef near
#undef far

using namespace PC_EDITOR_CORE;

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
    : EditorWindow(_editor, _name)
    , m_View(m_Editor->RenderHarwareInteface)
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

   
    m_View.Time = PC_CORE::Time::GetTime();
    m_View.Deltatime = PC_CORE::Time::DeltaTime();

    if (m_CameraViewDirty)
    {
        m_View.FromCamera(m_Camera);
        m_CameraViewDirty = false;
    }

    if (resize)
    {
        auto sizeI = Tbx::Vector2i(static_cast<int>(size.x), static_cast<int>(size.y));
        const float aspect = size.x / size.y;
        m_Camera.SetAspect(aspect);
        m_View.SetRenderSize(size);

        m_Editor->RenderHarwareInteface.GetRhiContext().WaitIdle(); // TO DO to remove thos implement vulkan deffered destroy
        m_Editor->Renderer.Build(m_View);
        UpdateImguiViewPort();
        m_CameraViewDirty = true;
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    uint32_t currentImage = m_Editor->RenderHarwareInteface.GetFrameIndex();

    ImGui::Image(imguiDescriptorSet[currentImage], ImVec2{viewportPanelSize.x, viewportPanelSize.y}, ImVec2(0, 0),
                 ImVec2(1, 1));
}

void WorldViewWindow::Render(PC_CORE::CommandList* _Cmd)
{
    PERF_REGION_SCOPED;
    EditorWindow::Render(_Cmd);
    m_Editor->Renderer.Excute(m_View);
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
    m_Editor->IMGUIContext.CreateImguiVulkanTexture(&m_Editor->Renderer.GetRenderGraph().GetOutPutImage(),
                                                    m_ViewPortSampler.Get(), imguiDescriptorSet.data(),
                                                    imguiDescriptorSet.size());
}
