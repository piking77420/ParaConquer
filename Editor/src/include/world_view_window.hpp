#pragma once


#include "editor_header.hpp"
#include "editor_window.hpp"
#include "low_renderer/descriptor_set.hpp"
#include "rendering/camera.hpp"
#include "rendering/rendering_typedef.h"
#include "resources/texture.hpp"
#include "rendering/rendering_typedef.h"

#include <vulkan/vulkan.h>

BEGIN_EDITOR_PCCORE




class WorldViewWindow : public EditorWindow
{
public:
    PC_CORE::Camera camera;
    
    explicit WorldViewWindow(Editor& _editor, const std::string& _name);

    ~WorldViewWindow() override;
    
    void Update() override;
    
    void Render() override;

private:
    void ResizeViewports();

    void UpdateViewPortDescriptorSet();

    PC_CORE::Gbuffers gbuffers;
    
    std::shared_ptr<PC_CORE::Texture> viewportTexture;

    std::shared_ptr<PC_CORE::FrameBuffer> finalImageViewport;
    
    PC_CORE::ShaderProgramDescriptorSets* m_ViewPortDescriptorSet = nullptr;

    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> imguiDescriptorSet;
};

END_EDITOR_PCCORE