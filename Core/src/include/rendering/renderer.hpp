#pragma once
#include "vulkan_vertex_buffer.hpp"
#include "low_renderer/command_list.hpp"
#include "low_renderer/rhi_context.hpp"
#include "resources/shader_program.h"

BEGIN_PCCORE

class Renderer
{
public:
    
    Renderer() = default;

    ~Renderer() = default;

    PC_CORE_API void Init();

    PC_CORE_API bool BeginDraw(Window* _window);

    PC_CORE_API void Render();

    PC_CORE_API void SwapBuffers(Window* _window    );
private:
    RhiContext* m_RhiContext;
    
    PC_CORE::ShaderProgram* m_Main = nullptr;

    std::shared_ptr<PC_CORE::CommandList> m_CommandList;

    std::shared_ptr<PC_CORE::VertexBuffer> m_VertexBuffer;
};

END_PCCORE