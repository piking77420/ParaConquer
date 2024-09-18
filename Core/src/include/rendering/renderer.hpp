#pragma once
#include "core_header.hpp"

#include <front_end/vulkan_app.hpp>

BEGIN_PCCORE
class Window;

enum class GraphicAPI
{
    NONE,
    VULKAN,
    COUNT
};

class Renderer
{
public:
    PC_CORE_API void Init(GraphicAPI _graphicAPI, Window* _window);

    PC_CORE_API void Destroy();

    PC_CORE_API void Render();

    PC_CORE_API void BeginFrame();
    
    PC_CORE_API void EndFrame();

    PC_CORE_API void SwapBuffers();

    PC_CORE_API void WaitDevice();
private:
    PC_CORE_API void InitRhi(GraphicAPI _graphicAPI, Window* _window);

    PC_CORE_API void InitShader();

    std::string mainShader = "mainShader";
};

END_PCCORE