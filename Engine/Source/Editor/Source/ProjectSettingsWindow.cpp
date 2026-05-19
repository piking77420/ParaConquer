#include <ProjectSettingsWindow.hpp>

#include <Editor.hpp>
#include <ImguiReflectedObject.hpp>

namespace PC_EDITOR_CORE
{
    ProjectSettingsWindow::ProjectSettingsWindow(Editor& _Editor, const std::string& _Name)
        : EditorWindow(_Editor, _Name)
    {
        isOpen = false;
    }

    ProjectSettingsWindow::~ProjectSettingsWindow() = default;

    void ProjectSettingsWindow::Update()
    {
        EditorWindow::Update();

        ImGui::PushFont(m_Editor->editorData.editorFont.veryBig);
        ImGui::Text("Projects Settings : ");
        ImGui::PopFont();

        if (ImGui::BeginTabBar("Render Mode"))
        {
            ImGui::PushFont(m_Editor->editorData.editorFont.big);

            if (ImGui::BeginTabItem("Render Mode"))
            {
                ImGui::PopFont();

                ImGui::PushFont(m_Editor->editorData.editorFont.small);
                if (ImGuiReflection::DrawEnumButtonExclusive(&m_Editor->editorData.ProjectSettings.RenderMode))
                {
                    for (auto& Window : m_Editor->EditorWindows)
                    {
                        Window->OnRenderModeDirty();
                    }
                }
                ImGui::PopFont();

                ImGui::EndTabItem();
            }
            else
            {
                ImGui::PopFont();
            }

            ImGui::EndTabBar();
        }
        
    }
}