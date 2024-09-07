﻿#pragma once

#include "app.hpp"
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "Imgui/imgui.h"
#include "world/transform.hpp"
#include "reflection/reflector.hpp"

BEGIN_EDITOR_PCCORE


struct SerializeTest
{
    float fvalue = 0.0256f;
    int value = 2;
};

REFLECT(SerializeTest)
REFLECT_MEMBER(SerializeTest, fvalue)
REFLECT_MEMBER(SerializeTest, value)

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

    void InitMaterial();

    std::vector<EditorWindow*> m_EditorWindows;
    
    ImGuiIO* io = nullptr;

    PC_CORE::Entity* m_Selected = nullptr;
};

END_PCCORE