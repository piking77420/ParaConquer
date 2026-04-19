#pragma once

#include <Imgui.h>
#include <Math/ToolboxTypedef.hpp>
#include <String>

#include "EditorHeader.hpp"


namespace PC_CORE
{
    class CommandList;
}

namespace PC_EDITOR_CORE
{
    class Editor;
}

BEGIN_EDITOR_PCCORE
    class EditorWindow
    {
    public:
        ImGuiWindowFlags windowFlags;

        std::string name = "EditorWindow";

        bool isOpen = true;

        Tbx::Vector2f size;

        Tbx::Vector2f position;

        bool resize = false;

        EditorWindow(Editor& _editor, const std::string& _name);

        virtual ~EditorWindow() = default;

        void Begin();

        void End();

        void GetInfo();

        bool IsInsideWindow(Tbx::Vector2f _point) const;

        bool IsCursorInsideWindow() const;


        virtual void Render([[maybe_unsed]] PC_CORE::CommandList* _Cmd)
        {
        };

        virtual void Update();

        virtual void OnPlayButton()
        {
        }

        virtual void OnResetScene()
        {
        }
        
        virtual void OnRenderModeDirty() {};

        void PushUpFrontAndFocus()
        {
            isOpen = true;
            m_PushUpFrontAndFocus = true;
        }

    protected:
        Editor* m_Editor = nullptr;

    private:
        bool m_PushUpFrontAndFocus = false;
    };

END_EDITOR_PCCORE
