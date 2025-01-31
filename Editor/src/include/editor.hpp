#pragma once

#include "app.hpp"
#include "dock_space.hpp"
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "physics/rigid_body.hpp"
#include "world/transform.hpp"
#include "reflection/reflector.hpp"

BEGIN_EDITOR_PCCORE


class Editor :  public PC_CORE::App
{
public:
    void Init() override;
    
    void Destroy() override;

    Editor() = default;

    ~Editor() = default;
    
    void InitTestScene();
    
    void FuncMember(const PC_CORE::Transform& t, const PC_CORE::RigidBody& c);
    
    void RotateCube();

    void DestroyTestScene();
    
    void Run() override;

    void InitEditorWindows();

    void UpdateEditorWindows();

    void ShaderRecompileList();
    
    std::vector<EditorWindow*> m_EditorWindows;

    DockSpace dockSpace;
    
    PC_CORE::EntityId m_SelectedEntityId = PC_CORE::INVALID_ENTITY_ID;

private:
    void InitThridPartLib();

    void UnInitThridPartLib();

    void CompileShader();
};

END_PCCORE