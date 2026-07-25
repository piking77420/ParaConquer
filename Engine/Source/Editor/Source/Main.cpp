#include <iostream>
#include <Editor.hpp>

#define WAIT_DEBUG_GPU_INJECTION 0

int main(int argc, char* argv[])
{
#if WAIT_DEBUG_GPU_INJECTION
    std::getchar();
#endif
    const PC_CORE::AppCreateInfo appCreateInfo =
    {
        .appName = "ParaConquerEditor",
        .appLogoPath = EDITOR_RESOURCE_PATH "/logo/ParaConquerLogoBlack.png",
        .enableGpuDebug = true,
        .graphicAPI = PC_CORE::GraphicAPI::Vulkan
    };

    PROFILER_NOOP;

    bool appShounldClose = false;
    while (!appShounldClose)
    {
        appShounldClose = true;
        PC_EDITOR_CORE::Editor Editor(appCreateInfo);
        Editor.Run(&appShounldClose);
        Editor.Destroy();
    }


    return 0;
}
