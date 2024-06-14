#include "profiler.hpp"

#include <vector>

#include "Imgui/imgui.h"
#include "time/timer.hpp"

PC_EDITOR_CORE::Profiler::Profiler(Editor& _editor) : EditorWindow(_editor)
{
    
}

void PC_EDITOR_CORE::Profiler::Update()
{
    EditorWindow::Update();

    const std::vector<PC_CORE::Timer::TimePass>& timers = PC_CORE::Timer::timePasses;
    
    ImGui::Begin("Profiler");


    for (int i = 0; i < timers.size(); ++i)
    {
        std::string name = timers.at(i).timePassname;
        //auto duration = std::to_string(timers.at(i).time);

        //ImGui::Text()
    }
    

    ImGui::End();
}
