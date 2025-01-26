#pragma once
#include "resources/shader_program.h"

BEGIN_PCCORE

class Renderer
{
public:
    void Init();
    
    Renderer() = default;

    ~Renderer() = default;

private:

    PC_CORE::ShaderProgram* m_Main = nullptr;
};

END_PCCORE