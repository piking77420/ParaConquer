#pragma once
#include "editor_header.hpp"
#include "math/toolbox_typedef.hpp"
#include "rendering/vulkan/vulkan_shader_stage.hpp"

BEGIN_EDITOR_PCCORE
// TODO 
    class TransformGizmo
{
public:
    enum class Operation
    {
        NONE,
        TRANSLATION,
        ROTATION,
        SCALE,
        COUNT
    };
    
    void SetRendering(Vector2f _screenPos, Vector2f _screenSize);

    bool Manipulate(const Matrix4x4f& _proj, const Matrix4x4f& _view,Matrix4x4f* _model);

    void Draw();

private:
    
    Vector2f screenPos;
    Vector2f screeSize;
    
};
END_EDITOR_PCCORE