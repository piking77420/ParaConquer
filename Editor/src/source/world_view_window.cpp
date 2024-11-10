#include "world_view_window.hpp"

#include "editor.hpp"
#include "vulkan_header.h"
#include "time/core_time.hpp"
#include "Imgui/imgui_impl_vulkan.h"
#include "rendering/render_harware_interface/RHI.hpp"

#undef near
#undef far

using namespace PC_EDITOR_CORE;

WorldViewWindow::WorldViewWindow(Editor& _editor, const std::string& _name)
    : EditorWindow(_editor, _name)
{
    m_ImaguiDescriptorSet.resize(MAX_FRAMES_IN_FLIGHT);
    viewPortImage.resize(MAX_FRAMES_IN_FLIGHT);
    m_FrameBuffers.resize(MAX_FRAMES_IN_FLIGHT);
}


void WorldViewWindow::Update()
{
    EditorWindow::Update();
    
    
    if (resize)
    {
        m_Editor->renderer.WaitDevice();
        ResizeViewports();        
    }

    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    uint32_t currentImage = PC_CORE::RHI::GetInstance().GetCurrentImage();
    VkDescriptorSet& descriptorSet = m_ImaguiDescriptorSet.at(static_cast<size_t>(currentImage));
    
    ImGui::Image( reinterpret_cast<ImTextureID>(m_ImaguiDescriptorSet.at(static_cast<size_t>(currentImage))), ImVec2{viewportPanelSize.x, viewportPanelSize.y} , ImVec2(0, 0), 
            ImVec2(1, 1));
}

void WorldViewWindow::Render()
{
    EditorWindow::Render();

    uint32_t currentImage = PC_CORE::RHI::GetInstance().GetCurrentImage();

    const Tbx::Vector2ui windowSize = m_Editor->window.GetWindowSize();
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
    renderingContext.frameBufferHandle = m_FrameBuffers.at(static_cast<size_t>(currentImage)).GetHandle();
    renderingContext.renderingContextSize = size;

    m_Editor->renderer.Render(renderingContext, m_Editor->world);
}

void WorldViewWindow::ResizeViewports()
{

    const PC_CORE::CreateTextureInfo depthCreateInfo =
         {
        .width = static_cast<int32_t>(size.x),
       .height =  static_cast<int32_t>(size.y),
       .depth = 1,
       .mipsLevels = 1,
       .data = {},
       .imageType = PC_CORE::ImageType::TYPE_2D,
       .format = PC_CORE::RHIFormat::D32_SFLOAT_S8_UINT,
       .textureAspect = static_cast<PC_CORE::TextureAspect>(PC_CORE::TextureAspect::DEPTH | PC_CORE::TextureAspect::STENCIL),
        .GenerateMipMap = false,
        .useAsAttachement = true,
        };
  

    m_DepthTexture = PC_CORE::Texture(depthCreateInfo);
    
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        PC_CORE::CreateTextureInfo createTextureInfo =
       {
            .width = static_cast<int32_t>(size.x),
           .height =  static_cast<int32_t>(size.y),
           .depth = 1,
           .mipsLevels = 1,
           .data = {},
           .imageType = PC_CORE::ImageType::TYPE_2D,
           .format = PC_CORE::RHIFormat::R8G8B8A8_SRGB,
           .textureAspect = PC_CORE::TextureAspect::COLOR,
            .GenerateMipMap = false,
            .useAsAttachement = true,
            };
    
        viewPortImage[i] = PC_CORE::Texture(createTextureInfo);

        
       
            ImGui_ImplVulkan_RemoveTexture(m_ImaguiDescriptorSet[i]);
            m_ImaguiDescriptorSet[i] = ImGui_ImplVulkan_AddTexture(CastObjectToVkObject<vk::Sampler>(viewPortImage[i].GetSamplerHandle())
                                            , CastObjectToVkObject<vk::ImageView>(viewPortImage[i].GetImageViewHandle()), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        

        const PC_CORE::FrameBufferCreateInfo frameBufferCreateInfo =
            {
            .renderTargets = {&viewPortImage[i], &m_DepthTexture},
            .renderPass = &m_Editor->renderer.forwardRenderPass,
            .width = static_cast<uint32_t>(size.x),
            .height = static_cast<uint32_t>(size.y),
            .layers = 1
            };

    
        m_FrameBuffers[i] = PC_CORE::FrameBuffer(frameBufferCreateInfo);
    }

    
 
   
}

