#pragma once

#include <Atomic>
#include <future>
#include <functional>
#include <optional>
#include <memory>

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
#include "EditorFiles.hpp"
#include "Reflection/Reflector.hpp"
#include "ObjectPtr.hpp"
#include "AssetsImporter.hpp"

BEGIN_EDITOR_PCCORE
    struct EditorFont
    {
        ImFont* tiny;
        ImFont* small;
        ImFont* normal;
        ImFont* big;
        ImFont* veryBig;
    };

    struct ProjectData
    {
        std::string projectName{};
        PC_CORE::GraphicAPI graphicApi{};

        ProjectData(ProjectFile& projectFile)
        {
            projectName = std::move(projectFile.name);
            graphicApi = projectFile.graphicApi;
        }

        DEFAULT_CONSTRUCTOR_DESTRUCTOR(ProjectData)

        DEFAULT_COPY_MOVE_OPERATIONS(ProjectData);
    };

    struct EditorData
    {
        EditorFont editorFont{};
        EditorFont editorFontItalic{};

        ProjectData projectData{};
        std::filesystem::path projectPath{};
    };

    using EditableSelectedObj = std::variant<std::monostate, PC_CORE::EntityId, PC_CORE::ObjectPtr<PC_CORE::Resource>>;

    class Editor : public PC_CORE::App
    {
    public:
        struct AssetImportData
        {
            std::vector<std::unique_ptr<AssetsImporter>> Imports;

            std::mutex _lock;
        };
    
        Editor();

        ~Editor() override;

        void Init(const PC_CORE::AppCreateInfo& _appCreateInfo) override;

        void Destroy() override;

        void Run(bool* _appShouldClose) override;

        template <EditorCommandDerived T, typename... Args>
        void PushCommand(Args&&... args);

        void RewindCommand();

        void InitTestScene();

        void DestroyTestScene();

        void InitEditor();

        void EditorCommandUpdate();

        void UpdateEditor();

        static inline Editor* instance = nullptr;

        ShaderCompiler shaderCompiler;

        PC_CORE::IMGUIContext IMGUIContext;

        DockSpace dockSpace;

        std::vector<std::unique_ptr<EditorSubSystem>> editorSubSystems;

        std::vector<std::unique_ptr<EditorWindow>> editorWindows;

        std::vector<std::unique_ptr<EditorCommand>> editorCommands;

        EditableSelectedObj selectedObject;

        EditorData editorData;

        AssetImportData AssetImportData;
    protected:
        void OnRender(PC_CORE::CommandList* _Cmd) override;


    private:

        void TempImportModel(const std::filesystem::path& _path);

        void LoadFromInitFiles();

        void SaveInitFiles();

        void CompileShader();

        void LookForEditorInit();

        void BasicOpenFile();

        void ReloadShaders();

        EditorRenderer m_EditorRenderer;

        std::vector<std::future<void>> m_FuturInits;

        PC_CORE::ObjectPtr<PC_CORE::Rendering::Material> testMaterial;

        PC_CORE::Thread::ThreadPool m_EditorThreadPool;
    };

    template <EditorCommandDerived T, typename... Args>
    void Editor::PushCommand(Args&&... args)
    {
        editorCommands.emplace_back(
            std::make_unique<T>(*this, std::forward<Args>(args)...)
        );
    }


END_EDITOR_PCCORE
