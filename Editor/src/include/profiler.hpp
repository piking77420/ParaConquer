#pragma once

#include "editor_header.hpp"
#include "editor_window.hpp"
#include "Imgui/imgui.h"

BEGIN_EDITOR_PCCORE
    class Profiler : public EditorWindow
{
public:
   
    Profiler(Editor& _editor, const std::string& _name);

    ~Profiler() = default;
    
    void Update() override;
private:
    
};

END_EDITOR_PCCORE