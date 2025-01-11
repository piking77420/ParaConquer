#pragma once

#include <vulkan/vulkan_core.h>

#include "editor_header.hpp"
#include "editor_window.hpp"
#include "rendering/camera.hpp"
#include "rendering/rendering_typedef.h"
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
  
    void ResizeViewports();
};

END_EDITOR_PCCORE