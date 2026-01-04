#pragma once


#include "EditorHeader.hpp"
#include "EditorWindow.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Gbuffers.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderGraph.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/Sampler.hpp"
#include "Resources/Texture2d.hpp"

#include <vulkan/vulkan.h>

namespace PC_CORE
{
    class RhiTexture2D;
}

namespace PC_CORE::Rendering
{
    class RenderView;
}

// TODO MAKE A VIEW CLASS

BEGIN_EDITOR_PCCORE
    class WorldViewWindow : public EditorWindow
    {
    public:

        explicit WorldViewWindow(Editor& _editor, const std::string& _name);

        ~WorldViewWindow() override;

        void Update() override;

        void Render(PC_CORE::CommandList* _Cmd) override;

        const PC_CORE::Camera& GetCamera() const
        {
            return m_Camera;
        }

    protected:
        PC_CORE::Camera m_Camera;

        bool m_CameraViewDirty = false;

        PC_CORE::Rendering::RenderView m_View;

        PC_CORE::Rendering::Renderer m_Renderer;

    private:
        void UpdateImguiViewPort();

        PC_CORE::Sampler m_ViewPortSampler;

        std::array<VkDescriptorSet, MaxFramesInFlight> imguiDescriptorSet;
    };

END_EDITOR_PCCORE
