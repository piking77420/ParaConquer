#pragma once

#include "editor_header.hpp"

namespace PC_EDITOR_CORE
{
    class Editor;
}

BEGIN_EDITOR_PCCORE
    class EditorWindow
{
public:
    EditorWindow(Editor& _editor);
    
    virtual ~EditorWindow() = default;

    virtual void Update();

    virtual void Render() {};

protected:
    Editor* m_Editor = nullptr;

};

END_EDITOR_PCCORE