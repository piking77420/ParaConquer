#pragma once


#include "EditorHeader.hpp"
#include "EditorWindow.hpp"
#include "Rendering/View.hpp"
#include "LowRenderer/DescriptorSet.hpp"
#include "Rendering/Camera.hpp"
#include "Resources/Texture2d.hpp"

#include <Vulkan/Vulkan.h>

#include "Rendering/Gbuffers.hpp"


namespace PC_CORE
{
    class RhiTexture2D;
}

// TODO MAKE A VIEW CLASS

BEGIN_EDITOR_PCCORE
    class WorldViewWindow : public EditorWindow
{
public:
    PC_CORE::Camera camera;
    
    explicit WorldViewWindow(Editor& _editor, const std::string& _name);

    ~WorldViewWindow() override;
    
    void Update() override;
    
    void Render() override;

protected:
    size_t m_RenderingContextFlag = 0;

    std::shared_ptr<PC_CORE::View> m_View;

private:
    void ResizeViewports();

    void UpdateImguiViewPort();


    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> imguiDescriptorSet;

};

END_EDITOR_PCCORE