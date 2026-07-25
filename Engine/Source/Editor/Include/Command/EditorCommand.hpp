#pragma once
#include <concepts>

#include "EditorHeader.hpp"

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

    template <class T>
    concept EditorCommandDerived = std::derived_from<T, EditorCommand>;


END_EDITOR_PCCORE
