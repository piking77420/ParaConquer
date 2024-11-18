#pragma once

#include <vulkan/vulkan_core.h>

#include "editor_header.hpp"
#include "editor_window.hpp"
#include "rendering/camera.hpp"
#include "rendering/frame_buffer.hpp"
#include "rendering/rendering_typedef.h"
#include "rendering/render_harware_interface/descriptor_set.hpp"
#include "resources/texture.hpp"


BEGIN_EDITOR_PCCORE
    class WorldViewWindow : public EditorWindow
{
public:
    PC_CORE::Camera camera;

    uint32_t viewportId = -1;
    
    explicit WorldViewWindow(Editor& _editor, const std::string& _name);

    ~WorldViewWindow() override = default;
    
    void Update() override;
    
    void Render() override;


private:
    enum class TMPRBuffer
    {
        COLOR,
        DEPTH,
        COUNT
    };

    std::vector<std::array<PC_CORE::Texture, static_cast<size_t>(TMPRBuffer::COUNT)>> m_ViewPortImage;

    std::vector<PC_CORE::FrameBuffer> m_FrameBuffers;
    
    std::vector<VkDescriptorSet> m_ImaguiDescriptorSet;

    std::vector<PC_CORE::DescriptorPoolHandle> descritptorSet;
    
    void ResizeViewports();
};

END_EDITOR_PCCORE