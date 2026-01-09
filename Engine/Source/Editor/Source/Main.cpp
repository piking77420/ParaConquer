#include <Iostream>
#include "Editor.hpp"

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

    bool appShounldClose = false;
    while (!appShounldClose)
    {
        appShounldClose = true;
        PC_EDITOR_CORE::Editor editor;
        editor.Init(appCreateInfo);
        editor.Run(&appShounldClose);
        editor.Destroy();
    }


    return 0;
}
