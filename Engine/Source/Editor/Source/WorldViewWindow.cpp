#include <backends/imgui_impl_vulkan.h>

#include "WorldViewWindow.hpp"
#include "Editor.hpp"
#include "Time/CoreTime.hpp"
#include "Resources/ResourceManager.hpp"
#include "VulkanDescriptorSet.hpp"
#include "LowRenderer/CommandList.hpp"
#include "Rendering/RenderSystem.hpp"

#include <Rendering/RenderPasses/ForwardPass.hpp>
#include <Rendering/RenderPasses/ToneMapPass.hpp>
#include <DebugView/DebugPass.hpp>

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
        .SetMipmapMode(PC_CORE::SamplerMipmapMode::Nearest)
        .SetU(PC_CORE::SamplerAddressMode::ClampToEdge)
        .SetV(PC_CORE::SamplerAddressMode::ClampToEdge)
        .SetW(PC_CORE::SamplerAddressMode::ClampToEdge)
        .Build();
}

WorldViewWindow::~WorldViewWindow()
{
   
}


void WorldViewWindow::Update()
{
    EditorWindow::Update();

   
    m_View.Deltatime = PC_CORE::Time::DeltaTime();

    if (m_CameraViewDirty || resize)
    {
        m_CameraViewDirty = false;
        m_Camera.ComputeMatricies();

        auto sizeI = Tbx::Vector2i(static_cast<int>(size.x), static_cast<int>(size.y));
        const float aspect = size.x / size.y;
        m_Camera.SetAspect(aspect);
        m_View.SetRenderSize(size);
        m_View.FromCamera(m_Camera);
    }

    if (resize)
    {
        RebuildViewport();
    }

    uint32_t currentImage = m_Editor->RenderHarwareInteface.GetFrameIndex();
    ImGui::Image(imguiDescriptorSet[currentImage], ImGui::GetContentRegionAvail());
}

void WorldViewWindow::Render(PC_CORE::CommandList* _Cmd)
{
    PERF_REGION_SCOPED;
    EditorWindow::Render(_Cmd);

    const PC_CORE::Rendering::RenderingWorldData& worldData = m_Editor->World.level.GetSystem<PC_CORE::Rendering::RendererSystem>()->GetRenderRenderingWorldData(); // should be done once

    m_Editor->Renderer.Excute(m_View, worldData);
}

void WorldViewWindow::OnRenderModeDirty()
{
    RebuildViewport();
}

void WorldViewWindow::BuildRenderGraph(PC_CORE::Rendering::RenderGraph& Graph)
{
    switch (m_Editor->editorData.ProjectSettings.RenderMode)
    {
    case PC_CORE::Rendering::RenderMode::TriangleBased:
        DrawTriangledBasedGraph(Graph);
        break;
    case PC_CORE::Rendering::RenderMode::ClusterBased:
        DrawMeshletBasedGraph(Graph);
        break;
    case PC_CORE::Rendering::RenderMode::PathTracing:
        break;
    default:
        assert(false);
        break;
    }
    
    Graph.SetRenderMode(m_Editor->editorData.ProjectSettings.RenderMode);
}

void WorldViewWindow::RebuildViewport()
{
    m_Editor->RenderHarwareInteface.GetRhiContext().WaitIdle(); // TO DO to remove thos implement vulkan deffered destroy
    m_Editor->Renderer.Build(m_View, std::bind(&WorldViewWindow::BuildRenderGraph, this, std::placeholders::_1));
    UpdateImguiViewPort();
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

void WorldViewWindow::DrawTriangledBasedGraph(PC_CORE::Rendering::RenderGraph& Graph)
{
    namespace Pass = PC_CORE::Rendering::Pass;

    switch (m_Editor->editorData.DebugView)
    {
    case DebugView::Lit:
        Graph.AddRenderPass<PC_CORE::Rendering::Pass::FowardPass>();
        Graph.AddRenderPass<PC_CORE::Rendering::Pass::ToneMapPass>();
        break;
    case DebugView::Unlit:
        break;
    case DebugView::Normal:
        break;
    case DebugView::UV:
        break;
    case DebugView::AO:
        break;
    case DebugView::Triangle:
    {
        const std::string PassName = "DebuTriangle";
        const std::array<float, 4> Color = 
        {
            0.5f,
            0.8f,
            0.1f,
            1.f
        };
        std::unique_ptr<PC_CORE::RhiShaderProgram>* shaderPtrTriangle = &m_Editor->Renderer.DrawMeshTriangle;
        std::unique_ptr<PC_CORE::RhiShaderProgram>* shaderPtrTriangleMeshlet = &m_Editor->Renderer.DrawMeshTriangleMeshlet;

        Graph.AddRenderPass<PC_EDITOR::DebugView::DebugPass>(PassName, Color, shaderPtrTriangle, shaderPtrTriangleMeshlet);
    }
        break;
    case DebugView::Meshlet:
    default:
        break;
    }
}

void WorldViewWindow::DrawMeshletBasedGraph(PC_CORE::Rendering::RenderGraph& Graph)
{
    switch (m_Editor->editorData.DebugView)
    {
    case DebugView::Lit:
        Graph.AddRenderPass<PC_CORE::Rendering::Pass::FowardPass>();
        Graph.AddRenderPass<PC_CORE::Rendering::Pass::ToneMapPass>();
        break;
    case DebugView::Unlit:
        break;
    case DebugView::Normal:
        break;
    case DebugView::UV:
        break;
    case DebugView::AO:
        break;
    case DebugView::Triangle:
    {
        const std::string PassName = "DebuTriangle";
        const std::array<float, 4> Color =
        {
            0.5f,
            0.8f,
            0.1f,
            1.f
        };
        std::unique_ptr<PC_CORE::RhiShaderProgram>* shaderPtrTriangle = &m_Editor->Renderer.DrawMeshTriangle;
        std::unique_ptr<PC_CORE::RhiShaderProgram>* shaderPtrTriangleMeshlet = &m_Editor->Renderer.DrawMeshTriangleMeshlet;

        Graph.AddRenderPass<PC_EDITOR::DebugView::DebugPass>(PassName, Color, shaderPtrTriangle, shaderPtrTriangleMeshlet);
    }
        break;
    case DebugView::Meshlet:
        break;
    default:
        break;
    }
}
