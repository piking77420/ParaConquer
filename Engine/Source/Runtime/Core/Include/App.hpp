#pragma once

#include "CoreHeader.hpp"
#include "World/World.hpp"
#include "Scripting/ScriptingLua.hpp"

#include <Singleton.hpp>
#include "Io/Window.hpp"
#include <Io/CoreIo.hpp>
#include <Rendering/PipelineCache.hpp>

#include "LowRenderer/Rhi.hpp"
#include "Rendering/Renderer.hpp"
#include "Thread/ThreadPool.hpp"
#include "Resources/ResourceManager.hpp"
#include "Rendering/RenderSettings.hpp"
#include "Thread/Tasks.hpp"

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
        PC_CORE_API static inline App* Instance = nullptr;

        Thread::ThreadPool ThreadPool;

        Thread::TaskScheduler TaskScheduler;

        ResourceManager ResourceManager;

        CoreIo CoreIo;

        Window MainWindow;

        Rhi RenderHarwareInteface;

        Rendering::RenderSettings RenderSettings;

        std::unique_ptr<PC_CORE::Rendering::PipelineCache> PipelineCache;

        PC_CORE::Rendering::Renderer Renderer;

        World World;

        std::unique_ptr<RhiSampler> SamplerLinearReapet;

        std::unique_ptr<RhiSampler> SamplerLinearClampToEdge;

        std::unique_ptr<RhiTexture> PurpleTexture;

        std::unique_ptr<RhiTexture> WhiteTexture;

        std::unique_ptr<RhiTexture> BlackTexture;

        PC_CORE_API App(const PC_CORE::AppCreateInfo& _AppCreateInfo);

        PC_CORE_API virtual ~App() = default;

        PC_CORE_API virtual void Destroy();

        PC_CORE_API virtual void Run(bool* _appShouldClose) = 0;

        PC_CORE_API void WorldTick(double _tick);

        template<typename F, typename... Args>
        [[nodiscard]] auto Enqueue(F&& f, Args&&... args)
            -> std::future<std::invoke_result_t<F, Args...>>
        {
            PERF_REGION_SCOPED;
            PERF_REGION_COLOR(PerfRegion::Core);

            using ReturnType = std::invoke_result_t<F, Args...>;

            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                [func = std::forward<F>(f),
                ... params = std::forward<Args>(args)]() mutable {
                    if constexpr (std::is_void_v<ReturnType>)
                    {
                        std::invoke(func, std::move(params)...);
                    }
                    else
                    {
                        return std::invoke(func, std::move(params)...);
                    }
                }
            );

            std::future<ReturnType> future = task->get_future();
            {
                std::scoped_lock lock(m_MainThreadMutex);
                m_MainThreadQueue.emplace([task]() {
                    (*task)();
                    });
            }

            return future;
        }

    protected:
        virtual void OnRender(PC_CORE::CommandList* _Cmd) = 0;

        PC_CORE_API void DequeuMainThreadTask();

    private:
        std::mutex m_MainThreadMutex;
        std::queue<std::function<void()>> m_MainThreadQueue;

    };

END_PCCORE
