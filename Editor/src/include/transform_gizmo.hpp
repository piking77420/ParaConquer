#pragma once
#include "editor_header.hpp"
#include "math/toolbox_typedef.hpp"

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
    
    void SetRendering(Tbx::Vector2f _screenPos, Tbx::Vector2f _screenSize);

    bool Manipulate(const Tbx::Matrix4x4f& _proj, const Tbx::Matrix4x4f& _view, Tbx::Matrix4x4f* _model);

    void Draw();

private:
    
    Tbx::Vector2f screenPos;
    Tbx::Vector2f screeSize;
    
};
END_EDITOR_PCCORE