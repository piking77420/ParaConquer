#include "transform_gizmo.hpp"

using namespace PC_EDITOR_CORE;

void TransformGizmo::SetRendering(Vector2f _screenPos, Vector2f _screenSize)
{
    screenPos = _screenPos;
    screeSize = _screenSize;
}

bool TransformGizmo::Manipulate(const Matrix4x4f& _proj, const Matrix4x4f& _view, Matrix4x4f* _model)
{
    return false;
}

void TransformGizmo::Draw()
{
    // to DO draw operation
}
