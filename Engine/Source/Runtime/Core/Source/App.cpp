#include "App.hpp"

#include <iostream>

#include "Log.hpp"
#include "Physics/SphereCollider.hpp"
#include "Resources/ResourceManager.hpp"
#include "Thread/ThreadUtils.hpp"

#include "Time/CoreTime.hpp"


using namespace PC_CORE;

void App::Destroy()
{
    ResourceManager::Destroy();
    PC_LOG("App Destroy")
}

App::App(const PC_CORE::AppCreateInfo& _AppCreateInfo)
    : MainWindow(_AppCreateInfo.appName.data())
    , RenderHarwareInteface(RenderHardwareInterfaceCreateInfo
    (
        GraphicAPI::Vulkan,
        &MainWindow,
        _AppCreateInfo.appName.data(),
        _AppCreateInfo.enableGpuDebug
    ))
    , Renderer(RenderHarwareInteface, MainWindow)
    , ThreadPool("Main Thread Pool")
{
    Instance = this;

    PERF_REGION_SCOPED;
    PC_LOG("App Init")
        // Can init without any depedancies
    MainWindow.SetIcon(_AppCreateInfo.appLogoPath.data());

    SamplerLinearReapet.reset(RenderHarwareInteface.CreateSampler());
    SamplerLinearReapet
        ->SetMagFilter(Filter::Linear)
        .SetMinFilter(Filter::Linear)
        .SetMipmapMode(SamplerMipmapMode::Linear)
        .SetU(SamplerAddressMode::Repeat)
        .SetV(SamplerAddressMode::Repeat)
        .SetW(SamplerAddressMode::Repeat)
        .SetName("SamplerLinearReapet")
        .Build();

    SamplerLinearClampToEdge.reset(RenderHarwareInteface.CreateSampler());
    SamplerLinearClampToEdge
        ->SetMagFilter(Filter::Linear)
        .SetMinFilter(Filter::Linear)
        .SetMipmapMode(SamplerMipmapMode::Linear)
        .SetU(SamplerAddressMode::ClampToEdge)
        .SetV(SamplerAddressMode::ClampToEdge)
        .SetW(SamplerAddressMode::ClampToEdge)
        .SetName("SamplerLinearClamp")
        .Build();

    // dummies Texture
    {
        std::scoped_lock _(RenderHarwareInteface.GetRhiContext().ResourceUpdateLock());
        RHI::ResourceUpdateBranch* branch = RenderHarwareInteface.GetRhiContext().ResourceUpdateBranch();
        auto InitDummyTexture = [&](std::unique_ptr<RhiTexture>& _Texture, const std::array<uint8_t, 4>& _Color, std::string_view _Name)
            {
                _Texture.reset(RenderHarwareInteface.CreateTexture());
                _Texture
                    ->SetTextureType(RhiTexture::Type::Texture2D)
                    .SetMemoryUsage(RhiTexture::MemoryUsage::StaticGPU)
                    .SetTextureUsage(RhiTexture::TextureUsageFlagBits::All)
                    .SetWidth(1)
                    .SetHeight(1)
                    .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
                    .SetName(_Name)
                    .Build();

                const PC_CORE::RhiTexture::LevelUploadOperation op = {
                    .Width = _Texture->GetWidth(),
                    .Height = _Texture->GetHeight(),
                    .Offset = 0u,
                    .Size = _Texture->GetWidth() * _Texture->GetHeight() * 4,
                };
                std::unique_ptr<uint8_t[]> dummyTextureData = std::make_unique<uint8_t[]>(_Texture->GetWidth() * _Texture->GetHeight() * 4);
                std::memcpy(dummyTextureData.get(), _Color.data(), sizeof(uint8_t) * 4);
                branch->
                    TextureUpload2D(*_Texture.get(), std::move(dummyTextureData), { op }, RhiResourceState::PixelShaderResource);
            };

        InitDummyTexture(PurpleTexture, std::array<uint8_t, 4>{255, 255, 0, 255}, "PurpleTexture"sv);
        InitDummyTexture(WhiteTexture, std::array<uint8_t, 4>{255, 255, 255, 255}, "WhiteTexture"sv);
        InitDummyTexture(BlackTexture, std::array<uint8_t, 4>{0, 0, 0, 255}, "BlackTexture"sv);
    }
    
    Time::Init();
}


void App::WorldTick(double _tick)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    World.Begin();
    World.Update(_tick);
    World.RenderingTick(_tick);
}

void App::DequeuMainThreadTask()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Core);

    while (!m_MainThreadQueue.empty())
    {
        std::function<void()> func;
        {
            std::scoped_lock _(m_MainThreadMutex);
            if (m_MainThreadQueue.empty())
                break;
            func = m_MainThreadQueue.front();
            m_MainThreadQueue.pop();
        }
        func();
    }
}
