#pragma once

#include <Atomic>

#include "App.hpp"
#include "DockSpace.hpp"
#include "EditorHeader.hpp"
#include "EditorRenderer.hpp"
#include "EditorWindow.hpp"
#include "Command/EditorCommand.hpp"
#include "EditorSubSystem/EditorSubSystem.hpp"
#include "Io/ImguiContext.h"
#include "Physics/RigidBody.hpp"
#include "World/Transform.hpp"
#include "ShaderCompiler.hpp"
#include "AssetBrowser.hpp"
#include "Reflection/Reflector.hpp"

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

struct ProjectData
{
    std::string projectName;
    std::filesystem::path projectPath;
};

struct EditorData
{
    ProjectData projectData;
    PC_CORE::GraphicAPI graphicApi;
};

using EditableSelectedObj = std::variant<std::monostate, PC_CORE::EntityId, PC_CORE::ResourceRef<PC_CORE::Resource>>;

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

    void InitEditor();

    void EditorCommandUpdate();

    void UpdateEditor();

    static inline Editor* instance = nullptr;
    
    EditorData editorData;

    DockSpace dockSpace;
    
    EditableSelectedObj selectedObject;
    
    PC_CORE::IMGUIContext IMGUIContext;

    std::vector<std::unique_ptr<EditorSubSystem>> editorSubSystems;

    std::vector<std::unique_ptr<EditorWindow>> editorWindows;
    
    std::vector<std::unique_ptr<EditorCommand>> editorCommands;

    ShaderCompiler shaderCompiler;

    PC_CORE::App gameApp;
private:
    void CompileShader();

    void LookForEditorInit();

    void BasicOpenFile();

    void ReloadShaders();
    
    EditorRenderer m_EditorRenderer;

    AssetBrowser m_AssetBrowser;

};

template <EditorCommandDerived T, typename ... Args>
void Editor::PushCommand(Args&&... args)
{
    editorCommands.emplace_back(
           std::make_unique<T>(*this, std::forward<Args>(args)...)
   );
}


END_EDITOR_PCCORE