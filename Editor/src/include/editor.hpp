﻿#pragma once

#include "app.hpp"
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "Imgui/imgui.h"
#include "world/transform.hpp"

BEGIN_EDITOR_PCCORE



class Editor : public PC_CORE::App
{
public:
    void Init() override;
    
    void Destroy() override;

    Editor() = default;

    ~Editor() = default;
    
    void InitTestScene();

    void DestroyTestScene();
    
    void Run() override;

    void InitEditorWindows();

    std::vector<EditorWindow*> m_EditorWindows;
    
    ImGuiIO* io = nullptr;

    Entity selected = NULL_ENTITY;
};

END_PCCORE