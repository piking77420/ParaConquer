#pragma once
#include "editor_window.hpp"

BEGIN_EDITOR_PCCORE

class SceneButton : public EditorWindow
{
public:
    SceneButton(Editor& _editor, const std::string& _name);

    ~SceneButton() override = default;

    void Update() override;
private:
    void OnEdit();

};

END_EDITOR_PCCORE