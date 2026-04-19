#ifndef PC_EDITOR_PROJECT_SETTINGS_WINDOW
#define PC_EDITOR_PROJECT_SETTINGS_WINDOW

#include <EditorWindow.hpp>

namespace PC_EDITOR_CORE 
{
    class ProjectSettingsWindow : public EditorWindow 
    {
    public:
        ProjectSettingsWindow(Editor& _Editor, const std::string& _Name);

        ~ProjectSettingsWindow() override;

        void Update() override;
        
    private:
    };
    
} // namespace PC_EDITOR_CORE

#endif