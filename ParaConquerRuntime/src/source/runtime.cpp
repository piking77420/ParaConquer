#include "runtime.h"
#include "time/core_time.hpp"

Runtime::Runtime(bool* m_AppSouldClose)
{
	m_App.Init();
}

Runtime::~Runtime()
{
	m_App.Destroy();
}

void Runtime::Run()
{
    while (!m_App.window.ShouldClose())
    {
        m_App.coreIo.PoolEvent();
        m_App.window.PoolEvents();
        PC_CORE::Time::UpdateTime();

        UpdateEditorWindows();

        gameApp.WorldTick();

        if (!gameApp.renderer.BeginDraw(&gameApp.window))
        {
            continue;
        }


        gameApp.renderer.SwapBuffers(&gameApp.window);
    }

    Rhi::GetRhiContext()->WaitIdle();
}
