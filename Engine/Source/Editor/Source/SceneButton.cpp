#include "SceneButton.hpp"

#include "Editor.hpp"
#include "World/World.hpp"

using namespace PC_EDITOR_CORE;

SceneButton::SceneButton(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;

    auto l = [&](ImguiImage* _image, const std::filesystem::path& _path)
        {
            _image->texure = PC_CORE::Texture2D(_path.filename().generic_string(), _path.generic_string());
            m_Editor->IMGUIContext.CreateImguiVulkanTexture(_image->texure.GetRhiTexture2D().get(), 
                m_Editor->editorData.nearestSampler.GetRhiSampler().get(), 
                &_image->descritproSet, 1);
        };

    l(&m_PlayImage, EDITOR_RESOURCE_PATH "/Icons/Play.png");
    l(&m_PauseImage, EDITOR_RESOURCE_PATH "/Icons/Pause.png");
    l(&m_ResetImage, EDITOR_RESOURCE_PATH "/Icons/Reset.png");

}

SceneButton::~SceneButton()
{
    m_Editor->IMGUIContext.DestroyVulkanTexture(&m_PlayImage.descritproSet, 1);
    m_Editor->IMGUIContext.DestroyVulkanTexture(&m_PauseImage.descritproSet, 1);
    m_Editor->IMGUIContext.DestroyVulkanTexture(&m_ResetImage.descritproSet, 1);
}

void SceneButton::Update()
{
    EditorWindow::Update();
    OnEdit();
}

void SceneButton::OnEdit()
{
    const int ButtonSize = 48;

    PC_CORE::World& world = m_Editor->gameApp.world;

    const ImguiImage& button = world.run ? m_ResetImage : m_PlayImage;
    
    ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
    {
        ImGui::PushID("B1");
        if (ImGui::ImageButton(button.texure.name.c_str(), (ImTextureID)button.descritproSet, { ButtonSize, ButtonSize }))
        {
            if (!world.run)
            {
                world.begin = true;
                for (auto& it : m_Editor->editorWindows)
                    it->OnPlayButton();
            }
            else
            {
                world.begin = false;
                world.run = false;
                for (auto& it : m_Editor->editorWindows)
                    it->OnResetScene();
            }
        }
      ImGui::PopID();
    }
    ImGui::SameLine();

    {
        if (world.run)
        {
            const ImguiImage buttonName2 = m_PauseImage;

            ImGui::PushID("B2");
            if (ImGui::ImageButton(buttonName2.texure.name.c_str(), (ImTextureID)buttonName2.descritproSet, { ButtonSize, ButtonSize }))
            {
                world.run = !world.run;
            }
            ImGui::PopID();
        }

    }
   

}
