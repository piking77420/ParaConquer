#include "transform_gizmo.hpp"

using namespace PC_EDITOR_CORE;

void TransformGizmo::SetRendering(Tbx::Vector2f _screenPos, Tbx::Vector2f _screenSize)
{
    screenPos = _screenPos;
    screeSize = _screenSize;
}

bool TransformGizmo::Manipulate(const Tbx::Matrix4x4f& _proj, const Tbx::Matrix4x4f& _view, Tbx::Matrix4x4f* _model)
{
    return false;
}

void TransformGizmo::Draw()
{
    // to DO draw operation
}
