#pragma once

#include "edit_world_window.hpp"

BEGIN_EDITOR_PCCORE

class Inspector : public EditorWindow
{
public:
    ~Inspector() override = default;
    
    void Update() override;

    Inspector(Editor& _editor, const std::string& _name);

private:
    void Show();
    
};

END_EDITOR_PCCORE