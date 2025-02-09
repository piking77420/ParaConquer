#include "world_view_window.hpp"

#include "editor.hpp"
#include "vulkan_header.h"
#include "time/core_time.hpp"
#include "Imgui/imgui_impl_vulkan.h"
#include "resources/vulkan_descriptor_sets.hpp"

#undef near
#undef far

using namespace PC_EDITOR_CORE;

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
    : EditorWindow(_editor, _name)
{
    m_Editor->renderer.m_DrawTextureScreenQuadShader->AllocDescriptorSet(&m_ViewPortDescriptorSet);
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
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    
    uint32_t currentImage = PC_CORE::Rhi::GetFrameIndex();

    Vulkan::VulkanDescriptorSets* des = reinterpret_cast<Vulkan::VulkanDescriptorSets*>(m_ViewPortDescriptorSet);
    
    ImGui::Image( reinterpret_cast<ImTextureID>(static_cast<VkDescriptorSet>(des->descriptorSets.at(static_cast<size_t>(currentImage)))), ImVec2{viewportPanelSize.x, viewportPanelSize.y} , ImVec2(0, 0), 
            ImVec2(1, 1));
}

void WorldViewWindow::Render()
{
    EditorWindow::Render();
    if (size == Tbx::Vector2f{0.f, 0.f})
        return;
    
    
    PC_CORE::RenderingContext renderingContext;
    renderingContext.lowLevelCamera =
    {
        .position = camera.position,
        .front = camera.front,
        .up = camera.up,
        .aspect = m_Editor->window.GetAspect(),
        .fov = camera.GetFOV(),
        .near = camera.GetNear(),
        .far = camera.GetFar(),
        .isOrthographic = camera.GetProjectionType() == PC_CORE::ProjectionType::ORTHOGRAPHIC,

    };
    renderingContext.time = PC_CORE::Time::GetTime();
    renderingContext.deltaTime = PC_CORE::Time::DeltaTime();
    renderingContext.gbufferFrameBuffer = gbuffers.GetFrameBuffer();
    renderingContext.finalImageFrameBuffer = finalImageViewport;
    renderingContext.viewPortDescriptorSet = m_ViewPortDescriptorSet;
    renderingContext.renderingContextSize = {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};
    
    m_Editor->renderer.DrawToRenderingContext(renderingContext, &gbuffers, &m_Editor->world);
}

void WorldViewWindow::ResizeViewports()
{
    
    const PC_CORE::CreateTextureInfo create_texture =
    {
        .width = static_cast<int32_t>(size.x),
        .height = static_cast<int32_t>(size.y),
        .depth = 1,
        .mipsLevels = 1,
        .imageType = PC_CORE::ImageType::TYPE_2D,
        .format = PC_CORE::RHIFormat::R8G8B8A8_UNORM,
        .channel = PC_CORE::Channel::RGBA,
        .textureAttachement = PC_CORE::TextureAttachement::Color,
        .textureNature = PC_CORE::TextureNature::RenderTarget,
        .canbeSampled = true,
        .GenerateMipMap = false,
        .data = nullptr
    };

    viewportTexture = std::make_unique<PC_CORE::Texture>(create_texture);

    std::vector<PC_CORE::Texture*> attachments = { viewportTexture.get() };
    
    const PC_CORE::CreateFrameInfo create_frame_info =
        {
        .width = static_cast<uint32_t>(size.x),
        .height = static_cast<uint32_t>(size.y),
        .attachements = &attachments,
        .renderPass = m_Editor->renderer.drawTextureScreenQuadPass.get()
        };
    finalImageViewport = PC_CORE::Rhi::CreateFrameBuffer(create_frame_info);

    gbuffers.HandleResize({ static_cast<int32_t>(size.x), static_cast<int32_t>(size.y) }, m_Editor->renderer.forwardPass);
}

void WorldViewWindow::UpdateViewPortDescriptorSet()
{   
    m_Editor->IMGUIContext.RemoveImguiVulkanViewport(imguiDescriptorSet);
    m_Editor->IMGUIContext.CreateImguiVulkanViewport( viewportTexture.get(),imguiDescriptorSet);
    
    PC_CORE::ImageSamperDescriptor image_samper_descriptor =
    {
        .sampler = PC_CORE::Rhi::GetRhiContext()->sampler.get(),
        .texture = gbuffers.GetTexture(PC_CORE::GbufferType::Albedo).get()
    };

    PC_CORE::ShaderProgramDescriptorWrite shaderProgramDescriptorWrite =
    {
        .shaderProgramDescriptorType = PC_CORE::ShaderProgramDescriptorType::CombineImageSampler,
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
