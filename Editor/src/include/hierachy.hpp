#pragma once

#include "edit_world_window.hpp"

BEGIN_EDITOR_PCCORE

class Hierachy : public EditorWindow
{
public:

    Hierachy(Editor& _editor , const std::string& name);
    
    ~Hierachy() override = default;
    
    void Update() override;
private:
    void ShowGraph();
};

END_EDITOR_PCCORE