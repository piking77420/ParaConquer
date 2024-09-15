#include "profiler.hpp"

#include <vector>

#include "editor.hpp"
#include "time/timer.hpp"

PC_EDITOR_CORE::Profiler::Profiler(Editor& _editor, const std::string& _name) : EditorWindow(_editor,_name)
{
    
}

void PC_EDITOR_CORE::Profiler::Update()
{
    EditorWindow::Update();

    const std::vector<PC_CORE::Timer::TimePass>& timers = PC_CORE::Timer::timePasses;
    
    //ImGui::Begin("Profiler");

    //ImGui::Text("Fps : %.2f", m_Editor->io->Framerate);

    for (size_t i = 0; i < timers.size(); ++i)
    {
        std::string name = timers.at(i).timePassname;
       // ImGui::Text("Pass %s = %.2lf ms", name.c_str(), static_cast<double>(timers.at(i).elapsed_ms));
        
    }
    

    //ImGui::End();
    PC_CORE::Timer::Clear();
}
