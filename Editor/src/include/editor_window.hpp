﻿#pragma once

#include <string>

#include "editor_header.hpp"
#include "Imgui/imgui.h"
#include "math/toolbox_typedef.hpp"

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

    Vector2f size;

    Vector2f position;

    bool resize = false;
    
    EditorWindow(Editor& _editor, const std::string& _name);
    
    virtual ~EditorWindow() = default;

    void Begin();
    
    void End();

    void GetInfo();
    
    virtual void Render() {};
    
    virtual void Update() {}


protected:
    Editor* m_Editor = nullptr;

};

END_EDITOR_PCCORE