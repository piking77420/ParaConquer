#pragma once

#include "app.hpp"
#include "dock_space.hpp"
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "io/imgui_context.h"
#include "physics/rigid_body.hpp"
#include "world/transform.hpp"
#include "reflection/reflector.hpp"

BEGIN_EDITOR_PCCORE

enum struct EditorInitData : uint8_t 
{
    PROJECT_ABSOLUTE_PATH,
    COUNT
};

constexpr std::array<const char*, (uint8_t)(EditorInitData::COUNT)> EditorInitDataKeys =
{
    "PROJECT_ABSOLUTE_PATH"
};

constexpr const char* ParaConquerProjectFileFormat = ".Prproject";
constexpr const char* ParaConquerEditorInitFile = "editor.ini";

struct EditorData
{
    std::string projectName;
    std::filesystem::path projectPath;
};

class Editor
{
public:
    void Init();
    
    void Destroy();

    Editor();

    ~Editor();
    
    void InitTestScene();
    
    void DestroyTestScene();
    
    void Run(bool* _appShouldClose);

    void InitEditorWindows();

    void UpdateEditorWindows();

    PC_CORE::App gameApp;

    EditorData editorData;
    
    std::vector<std::unique_ptr<EditorWindow>> m_EditorWindows;

    DockSpace dockSpace;
    
    PC_CORE::EntityId m_SelectedEntityId = PC_CORE::INVALID_ENTITY_ID;

    PC_CORE::IMGUIContext IMGUIContext;

private:
    void InitThridPartLib();

    void UnInitThridPartLib();

    void CompileShader();

    void LookForEditorInit();

    void ParseEditorInit();

    void BasicOpenFile();
};

END_PCCORE