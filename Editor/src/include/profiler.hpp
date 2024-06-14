#pragma once

#include "editor_header.hpp"
#include "editor_window.hpp"

BEGIN_EDITOR_PCCORE
class Profiler : public EditorWindow
{
public:
   
    Profiler(Editor& _editor);

    ~Profiler() = default;
    
    void Update() override;
};

END_EDITOR_PCCORE