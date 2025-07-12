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
    
    m_Editor->gameApp.renderer.m_DrawTextureScreenQuadShader.lock()->AllocDescriptorSet(&m_ViewPortDescriptorSet, 0);

    for (auto& it : imguiDescriptorSet)
        it = VK_NULL_HANDLE;
}

WorldViewWindow::~WorldViewWindow()
{
    delete m_ViewPortDescriptorSet;
}


void WorldViewWindow::Update()
{
    EditorWindow::Update();

    if (size == Tbx::Vector2f{0.f, 0.f})
        return;

    if (resize)
    {
        PC_CORE::Rhi::GetRhiContext()->WaitIdle();
        ResizeViewports();
        UpdateViewPortDescriptorSet();
        camera.SetAspect(size.x / size.y);
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    
    uint32_t currentImage = PC_CORE::Rhi::GetFrameIndex();

    
    ImGui::Image( reinterpret_cast<ImTextureID>(imguiDescriptorSet[currentImage]), ImVec2{viewportPanelSize.x, viewportPanelSize.y}, ImVec2(0, 0),
          ImVec2(1, 1));
}

void WorldViewWindow::Render()
{
    PERF_REGION_SCOPED;

    EditorWindow::Render();
    if (size == Tbx::Vector2f{0.f, 0.f})
        return;
    
    
    
    PC_CORE::RenderingContext renderingContext;
    renderingContext.lowLevelCamera =
    {
        .position = camera.position,
        .front = camera.front,
        .up = camera.up,
        .aspect = camera.GetAspect(),
        .fov = camera.GetFOV(),
        .near = camera.GetNear(),
        .far = camera.GetFar(),
        .isOrthographic = camera.GetProjectionType() == PC_CORE::ProjectionType::ORTHOGRAPHIC,
    };
    renderingContext.time = PC_CORE::Time::GetTime();
    renderingContext.deltaTime = PC_CORE::Time::DeltaTime();
    renderingContext.forwardFrameBuffer = m_Gbuffers.GetForwardFrameBuffer();
    renderingContext.gbufferFrameBuffer = m_Gbuffers.GetGbufferFrameBuffer();

    renderingContext.finalImageFrameBuffer = m_FinalFrameBufferViewport;
    renderingContext.viewPortDescriptorSet = m_ViewPortDescriptorSet;
    renderingContext.gbufferDescriptorSet = m_Gbuffers.GetDescritptorSet();
    renderingContext.renderingContextSize = {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};
    renderingContext.renderingContextFlag = m_RenderingContextFlag;
    
   m_Editor->gameApp.renderer.DrawToRenderingContext(renderingContext, &m_Editor->gameApp.world);
}


void WorldViewWindow::ResizeViewports()
{
    
    PC_CORE::CreateImageInfo create_texture =
   {
        .width =  static_cast<int32_t>(size.x),
        .height = static_cast<int32_t>(size.y),
        .depth = 1,
        .layerCount = 1,
        .mipsLevels = 1,
        .textureType = PC_CORE::TextureType::Texture2D,
        .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
        .channel = PC_CORE::Channel::RGBA,
        .textureUsage = PC_CORE::TextureUsage::RenderTarget | PC_CORE::TextureUsage::Sampled,
        .memoryVisibility = PC_CORE::MemoryLocalisation::GPU_Only,
        .samples = 1,
        .GenerateMipMap = false,
        .datas = {}
    };

   // assert(false && "TO DO Handle move constructor and other case");
    m_FinalImage = PC_CORE::Texture2D(create_texture);
    create_texture.samples = PC_CORE::Rhi::GetRhiContext()->physicalDevices->GetPhysicalDevice().GetMaxUsableSampleCount();
    m_ResolvedTexture = PC_CORE::Texture2D(create_texture);
    
    std::vector<PC_CORE::FrameBufferAttachementDesriptor> attachments =
    { 
        {
            &m_ResolvedTexture,
        },
        {
            &m_FinalImage
        }
    };
    
    const PC_CORE::CreateFrameInfo create_frame_info =
        {
        .width = static_cast<uint32_t>(size.x),
        .height = static_cast<uint32_t>(size.y),
        .attachements = &attachments,
        .renderPass = m_Editor->gameApp.renderer.renderPasses.drawToFinalViewPort.get()
        };
    m_FinalFrameBufferViewport = PC_CORE::Rhi::CreateFrameBuffer(create_frame_info);
    
    m_Gbuffers.HandleResize({ static_cast<int32_t>(size.x), static_cast<int32_t>(size.y) }, m_Editor->gameApp.renderer.renderPasses.forwardPass,
        m_Editor->gameApp.renderer.renderPasses.defferedPass);
}

void WorldViewWindow::UpdateViewPortDescriptorSet()
{   
    
    m_Editor->IMGUIContext.RemoveImguiVulkanViewport(imguiDescriptorSet);
    m_Editor->IMGUIContext.CreateImguiVulkanViewport( &m_FinalImage, imguiDescriptorSet);
    
    std::shared_ptr<PC_CORE::Sampler> sampler = PC_CORE::ResourceManager::Get<PC_CORE::Sampler>("LinearRepeat");
    
    PC_CORE::ImageSamperDescriptor image_samper_descriptor =
    {
        .sampler = sampler.get(),
        .texture = &m_Gbuffers.GetImage()
    };

    PC_CORE::ShaderProgramDescriptorWrite shaderProgramDescriptorWrite =
    {
        .shaderProgramDescriptorType = PC_CORE::ShaderProgramDescriptorType::CombinedImageSampler,
        .bindingIndex = 0,
        .uniformBufferDescriptor = nullptr,
        .imageSamperDescriptor = &image_samper_descriptor
    };

    std::vector<PC_CORE::ShaderProgramDescriptorWrite> writes =
    {
        shaderProgramDescriptorWrite
    };

    m_ViewPortDescriptorSet->WriteDescriptorSets(writes);
}
