#pragma once

#include "editor_header.hpp"
#include "world_view_window.hpp"
#include "input/low_pass_filter.hpp"

BEGIN_EDITOR_PCCORE
class EditWorldWindow : public WorldViewWindow
{
public:

    explicit EditWorldWindow(Editor& _editor, const std::string& _name);
    
    ~EditWorldWindow() override = default;
    
    void Update() override;

protected:
    void MoveCamera();
    
    PC_CORE::LowPassFilter<Vector2f, 4> deltass;
    
    float cameraSpeed = 5.f;
    
    float pitch = 0.f;
    
    float yaw = 0.f;
};

END_EDITOR_PCCORE