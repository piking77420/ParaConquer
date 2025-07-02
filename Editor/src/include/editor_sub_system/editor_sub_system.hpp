#pragma once

#include "editor_header.hpp"

BEGIN_EDITOR_PCCORE

class Editor;

class EditorSubSystem
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(EditorSubSystem);
    
    EditorSubSystem(Editor& _editor) : m_Editor(_editor) {}

    virtual void OnRender() {}

    virtual void Tick(float _deltaTime){}
    
    virtual ~EditorSubSystem() = default;

protected:
    Editor& m_Editor;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

END_EDITOR_PCCORE