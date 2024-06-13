#pragma once

#include "editor_header.hpp"
#include "editor_window.hpp"
#include "rendering/camera.hpp"
#include "rendering/vulkan/vulkan_viewport.hpp"

BEGIN_EDITOR_PCCORE
class WorldViewWindow : public EditorWindow
{
public:
    PC_CORE::VulkanViewport viewport;

    PC_CORE::Camera camera;

    explicit WorldViewWindow(Editor& _editor);

    ~WorldViewWindow() override = default;
    
    void Update() override;

    std::vector<VkDescriptorSet> m_ImaguiDescriptorSet;

protected:
    void ShowViewPort();
};

END_EDITOR_PCCORE