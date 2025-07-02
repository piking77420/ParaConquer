#pragma once


#include "editor_header.hpp"
#include "editor_window.hpp"
#include "low_renderer/descriptor_set.hpp"
#include "rendering/camera.hpp"
#include "rendering/rendering_typedef.h"
#include "resources/texture_2d.hpp"
#include "rendering/rendering_typedef.h"

#include <vulkan/vulkan.h>


namespace PC_CORE
{
    class RhiTexture2D;
}

BEGIN_EDITOR_PCCORE
    class WorldViewWindow : public EditorWindow
{
public:
    PC_CORE::Camera camera;
    
    explicit WorldViewWindow(Editor& _editor, const std::string& _name);

    ~WorldViewWindow() override;
    
    void Update() override;
    
    void Render() override;

protected:
    size_t m_RenderingContextFlag = 0;

private:
    void ResizeViewports();

    void UpdateViewPortDescriptorSet();

    PC_CORE::Gbuffers m_Gbuffers;
    
    PC_CORE::Texture2D m_OutRenderImage;
    
    PC_CORE::Texture2D m_ResolvedTexture;

    std::shared_ptr<PC_CORE::FrameBuffer> m_FinalFrameBufferViewport;
    
    PC_CORE::ShaderProgramDescriptorSets* m_ViewPortDescriptorSet = nullptr;

    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> imguiDescriptorSet;

};

END_EDITOR_PCCORE