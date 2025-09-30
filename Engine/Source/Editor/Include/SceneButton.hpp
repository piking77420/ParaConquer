#pragma once
#include "EditorWindow.hpp"
#include "Io/ImguiContext.h"

BEGIN_EDITOR_PCCORE

class SceneButton : public EditorWindow
{
public:
    SceneButton(Editor& _editor, const std::string& _name);

    ~SceneButton() override;

    void Update() override;
private:
    void OnEdit();

    ImguiImage m_PlayImage;
    
    ImguiImage m_PauseImage;

    ImguiImage m_ResetImage;

};

END_EDITOR_PCCORE