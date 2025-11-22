#pragma once

#include "CoreHeader.hpp"
#include "World/World.hpp"
#include "Scripting/ScriptingLua.hpp"

#include "Io/Window.hpp"
#include <Io/CoreIo.hpp>

#include "LowRenderer/Rhi.hpp"
#include "Rendering/Renderer.hpp"


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
        static constexpr auto AppName = "ParaConquer";

        CoreIo CoreIo;

        Window MainWindow;

        Rhi RenderHarwareInteface;

        RenderingWorldData RenderingWorldData;

        Renderer Renderer;

        World World;

        PC_CORE_API void Init(const AppCreateInfo& _appCreateInfo);

        PC_CORE_API void Destroy();

        PC_CORE_API App();

        PC_CORE_API ~App() = default;

        PC_CORE_API void Run();

        PC_CORE_API void WorldTick(double _tick);

        PC_CORE_API static inline App* Instance = nullptr;
    };

END_PCCORE
