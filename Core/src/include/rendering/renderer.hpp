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
    void Init(GraphicAPI _graphicAPI, Window* _window);

    void Destroy();
   
private:
    
    void InitRhi(GraphicAPI _graphicAPI, Window* _window);
};

END_PCCORE