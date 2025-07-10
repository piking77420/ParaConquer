#pragma once

#include "editor_header.hpp"

BEGIN_EDITOR_PCCORE

class Editor;


class EditorCommand
{
public:
    EditorCommand(Editor& _editor) : m_Editor(_editor)
    {
        
    }

    virtual ~EditorCommand() = default;
protected:
    Editor& m_Editor;
};

template<class T>
   concept EditorCommandDerived = std::is_base_of_v<EditorCommand, T>;


END_EDITOR_PCCORE