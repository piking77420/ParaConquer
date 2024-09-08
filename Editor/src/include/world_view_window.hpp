#pragma once

#include "editor_header.hpp"
#include "editor_window.hpp"
#include "rendering/camera.hpp"


BEGIN_EDITOR_PCCORE

class WorldViewWindow : public EditorWindow
{
public:
    PC_CORE::Camera camera;

    uint32_t viewportId = -1;

    //std::vector<VkDescriptorSet> m_ImaguiDescriptorSet;
    
    //const PC_CORE::ViewPort* viewPort = nullptr;
    
    explicit WorldViewWindow(Editor& _editor, const std::string& _name);

    ~WorldViewWindow() override;
    
    void Update() override;
    
    void Render() override;


public:
};

END_EDITOR_PCCORE