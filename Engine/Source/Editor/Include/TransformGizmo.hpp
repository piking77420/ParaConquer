#pragma once
#include "EditorHeader.hpp"
#include <Math/ToolBoxTypeDef.hpp>

BEGIN_EDITOR_PCCORE
    // TODO 
    class TransformGizmo
    {
    public:
        enum class Operation
        {
            NoneFlag,
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
