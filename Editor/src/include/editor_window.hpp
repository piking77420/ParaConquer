#pragma once

#include <string>

#include "editor_header.hpp"

namespace PC_EDITOR_CORE
{
    class Editor;
}

BEGIN_EDITOR_PCCORE
class EditorWindow
{
public:

    std::string name = "EditorWindow";
    
    EditorWindow(Editor& _editor);
    
    virtual ~EditorWindow() = default;

    void Begin();
    
    void End();
    
    virtual void Render() {};
    
    virtual void Update() {}


protected:
    Editor* m_Editor = nullptr;

};

END_EDITOR_PCCORE