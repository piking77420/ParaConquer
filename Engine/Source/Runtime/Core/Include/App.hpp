#pragma once

#include "CoreHeader.hpp"
#include "World/World.hpp"
#include "Scripting/ScriptingLua.hpp"

#include "Io/Window.hpp"
#include <Io/CoreIo.hpp>

#include "LowRenderer/Rhi.hpp"
#include "Rendering/Renderer.hpp"
#include "Thread/ThreadPool.hpp"
#include "Resources/ResourceManager.hpp"

BEGIN_PCCORE
    struct AppCreateInfo
    {
        std::string appName;
        std::string appLogoPath;

        bool enableGpuDebug;
        GraphicAPI graphicAPI;
    };

    class App
    {
    public:
        Thread::ThreadPool ThreadPool;

        ResourceManager ResourceManager;

        CoreIo CoreIo;

        World World;

        Window MainWindow;

        Rhi RenderHarwareInteface;

        PC_CORE::Rendering::Renderer Renderer; //  TODO HANDLE MULIPTLE VIEW PORT

        std::unique_ptr<CommandList> PrimaryCommandBuffer;

        std::unique_ptr<RhiSampler> TextureSampler;

        std::unique_ptr<RhiTexture> DummyTexture;

        PC_CORE_API App();

        PC_CORE_API virtual ~App() = default;

        PC_CORE_API virtual void Init(const AppCreateInfo& _appCreateInfo);

        PC_CORE_API virtual void Destroy();

        PC_CORE_API virtual void Run(bool* _appShouldClose) = 0;

        PC_CORE_API void WorldTick(double _tick);

        PC_CORE_API void RenderFrame();

        PC_CORE_API static inline App* Instance = nullptr;
    protected:
        virtual void OnRender(PC_CORE::CommandList* _Cmd) = 0;

    };

END_PCCORE
