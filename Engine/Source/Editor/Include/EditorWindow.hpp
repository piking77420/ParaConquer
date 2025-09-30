#pragma once

#include <String>
#include <Imgui.h>
#include <Math/ToolboxTypedef.hpp>

#include "EditorHeader.hpp"
#include "Importer.hpp"


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


    virtual void Render() {};
    
    virtual void Update()
    {
      
    }

    virtual void OnPlayButton()
    {}

    virtual void OnResetScene(){}

protected:
    Editor* m_Editor = nullptr;

    Importer m_Importer;
    
};

END_EDITOR_PCCORE