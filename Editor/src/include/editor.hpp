#pragma once

#include "app.hpp"
#include "dock_space.hpp"
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "command/editor_command.hpp"
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

    template <EditorCommandDerived T, typename ...Args>
    void PushCommand(Args&&... args);

    void RewindCommand();
    
    void InitTestScene();
    
    void DestroyTestScene();
    
    void Run(bool* _appShouldClose);

    void InitEditorWindows();

    void EditorCommandUpdate();

    void UpdateEditorWindows();
    
    PC_CORE::App gameApp;

    EditorData editorData;
    
    std::vector<std::unique_ptr<EditorWindow>> m_EditorWindows;

    DockSpace dockSpace;
    
    PC_CORE::EntityId m_SelectedEntityId = PC_CORE::INVALID_ENTITY_ID;

    PC_CORE::IMGUIContext IMGUIContext;

private:
    void InitThridPartLib(PC_CORE::GraphicAPI graphicApi);

    void UnInitThridPartLib();

    void CompileShader();

    void LookForEditorInit();

    void ParseEditorInit();

    void BasicOpenFile();
    
    std::vector<std::unique_ptr<EditorCommand>> m_EditorCommands;
};

template <EditorCommandDerived T, typename ... Args>
void Editor::PushCommand(Args&&... args)
{
    m_EditorCommands.emplace_back(
           std::make_unique<T>(*this, std::forward<Args>(args)...)
   );
}


END_EDITOR_PCCORE